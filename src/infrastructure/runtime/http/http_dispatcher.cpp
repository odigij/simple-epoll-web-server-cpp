#include "sews/infrastructure/runtime/http/http_dispatcher.hpp"
#include "sews/infrastructure/format/http/request.hpp"
#include "sews/infrastructure/format/http/response.hpp"
#include "sews/infrastructure/transport/plain_text_channel.hpp"

namespace sews::runtime::http
{
	Dispatcher::Dispatcher(std::unique_ptr<interface::Acceptor> acceptor,
						   std::unique_ptr<interface::SocketLoop> socketLoop,
						   std::unique_ptr<interface::RequestParser> parser,
						   std::unique_ptr<interface::MessageHandler> handler,
						   std::unique_ptr<interface::ResponseSerializer> serializer,
						   std::unique_ptr<interface::Logger> logger)
		: acceptor(std::move(acceptor)), socketLoop(std::move(socketLoop)), parser(std::move(parser)),
		  handler(std::move(handler)), serializer(std::move(serializer)), logger(std::move(logger))
	{
	}

	Dispatcher::~Dispatcher(void)
	{
	}

	void Dispatcher::run(void)
	{
		interface::Channel &serverChannel{acceptor->channel()};
		socketLoop->registerChannel(serverChannel);
		std::vector<interface::Channel *> watched;
		std::vector<interface::SocketEvent> events;
		std::vector<char> buffer(8192);
		std::vector<int> fdsToClose;
		ssize_t bytesRead{0};

		// TODO:
		// [X] Handle error cases.
		// [X] Log necessary info.
		// [] No unconditional loop.
		// [] Reserve vectors.

		while (true)
		{
			watched.push_back(&serverChannel);
			for (std::pair<const int, std::unique_ptr<sews::interface::Channel>> &pair : activeChannels)
			{
				watched.push_back(pair.second.get());
			}

			socketLoop->poll(watched, events);

			// NOTE:
			// - Do not modify container (events) while in loop.
			// - Do not clear buffer vector unless you re-assign, let it over-write.
			for (interface::SocketEvent &event : events)
			{
				if (event.channel.getFd() == serverChannel.getFd())
				{
					logger->log(enums::LogType::INFO, "Http dispatcher: Incomming connection request.");
					std::unique_ptr<interface::Channel> clientChannel{acceptor->accept()};
					if (!clientChannel)
					{
						logger->log(enums::LogType::ERROR, "Http dispatcher: Failed to accept channel.");
						continue;
					}

					socketLoop->registerChannel(*clientChannel);
					activeChannels[clientChannel->getFd()] = std::move(clientChannel);
					continue;
				}

				if (event.flag == enums::SocketEvent::HANGUP)
				{
					logger->log(enums::LogType::INFO, "Http dispatcher: Connection closed by the peer.");
					fdsToClose.push_back(event.channel.getFd());
					continue;
				}
				else if (event.flag == enums::SocketEvent::ERROR)
				{
					logger->log(enums::LogType::ERROR, "Http dispatcher: Connection lost.");
					fdsToClose.push_back(event.channel.getFd());
					continue;
				}

				transport::PlainTextChannel *plainTextChannel{
					dynamic_cast<transport::PlainTextChannel *>(&event.channel)};
				if (!plainTextChannel)
				{
					logger->log(enums::LogType::ERROR, "Http dispatcher: Failed to cast channel.");
					continue;
				}

				if (event.flag == enums::SocketEvent::READ)
				{
					bytesRead = event.channel.readRaw(buffer.data(), buffer.size());

					if (bytesRead <= 0)
					{
						fdsToClose.push_back(event.channel.getFd());
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::ERROR});
						logger->log(enums::LogType::ERROR,
									"Http dispatcher: Failed to read bytes, closing the socket.");
						continue;
					}

					std::string rawRequest(buffer.data(), bytesRead);
					std::unique_ptr<interface::Message> message{parser->parse(rawRequest)};

					if (!message) // Gracefully fallback
					{
						logger->log(enums::LogType::ERROR, "Http dispatcher: Failed to parse request.");
						logger->log(enums::LogType::WARNING, "Http dispatcher: Sending a response anyway.");
						format::http::Response errResp;
						errResp.status = 500;
						errResp.statusText = "Internal Error";
						errResp.version = "HTTP/1.1";
						errResp.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(errResp);
						// fdsToClose.push_back(event.channel.getFd());
					}
					else
					{
						auto *req = dynamic_cast<format::http::Request *>(
							message.get()); // Using "message" as it is casted as request.

						std::unique_ptr<interface::Message> response{
							handler->handle(*message)}; // This is null pointer.
						if (!response)					// Gracefully fallback
						{
							logger->log(enums::LogType::ERROR, "Http dispatcher: Handler returned null response.");
							logger->log(enums::LogType::WARNING, "Http dispatcher: Sending a response anyway.");
							format::http::Response errResp;
							errResp.status = 500;
							errResp.statusText = "Internal Error";
							errResp.version = "HTTP/1.1";
							errResp.headers["Connection"] = "close";
							plainTextChannel->getResponse() = serializer->serialize(errResp);
							continue;
						}
						plainTextChannel->getResponse() = serializer->serialize(*response);
						if (!req)
						{
							logger->log(enums::LogType::ERROR, "Http dispatcher: Parsed message was not a Request.");
						}
						else
						{
							logger->log(enums::LogType::INFO, "Http dispatcher: " + req->method + ' ' + req->path);
						}
					}

					socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::WRITE});
				}

				if (event.flag == enums::SocketEvent::WRITE)
				{
					std::string &out = plainTextChannel->getResponse();
					plainTextChannel->writeRaw(out.c_str(), out.size());
					out.clear();
					socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::READ});
				}
			}

			for (int fd : fdsToClose)
			{
				auto it = activeChannels.find(fd);
				if (it != activeChannels.end())
				{
					socketLoop->unregisterChannel(*it->second);
					activeChannels.erase(it);
				}
			}

			bytesRead = 0;
			fdsToClose.clear();
			events.clear();
			watched.clear();
		}
	};
} // namespace sews::runtime::http
