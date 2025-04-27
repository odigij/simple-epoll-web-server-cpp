#include "sews/runtime/http/http_dispatcher.hpp"
#include "sews/infrastructure/format/http/request.hpp"
#include "sews/infrastructure/format/http/response.hpp"
#include "sews/infrastructure/io/epoll/socket_poll.hpp"
#include "sews/infrastructure/transport/plain_text_channel.hpp"

namespace sews::runtime::http
{
	Dispatcher::Dispatcher(std::unique_ptr<interface::Acceptor> acceptor,
						   std::unique_ptr<interface::SocketLoop> socketLoop,
						   std::unique_ptr<interface::ConnectionManager> connectionManager,
						   std::unique_ptr<interface::RequestParser> parser,
						   std::unique_ptr<interface::MessageHandler> handler,
						   std::unique_ptr<interface::ResponseSerializer> serializer, interface::Logger *logger)
		: acceptor(std::move(acceptor)), socketLoop(std::move(socketLoop)),
		  connectionManager(std::move(connectionManager)), parser(std::move(parser)), handler(std::move(handler)),
		  serializer(std::move(serializer)), logger(logger)
	{
		logger->log(enums::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Initialized.");
	}

	Dispatcher::~Dispatcher(void)
	{
		logger->log(enums::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Terminated.");
	}

	void Dispatcher::run(void)
	{
		interface::Channel &serverChannel{acceptor->channel()};
		socketLoop->registerChannel(serverChannel);
		std::vector<interface::Channel *> watched;
		std::vector<interface::SocketEvent> events;
		std::vector<char> buffer(8192);
		ssize_t bytesRead{0};

		if (auto *epollLoop = dynamic_cast<sews::io::epoll::SocketLoop *>(socketLoop.get()))
		{
			// Needed cohession for performance to reserve capacity.
			const size_t cap = epollLoop->getEventCapacity();
			events.reserve(cap);
			watched.reserve(cap + 1);
		}

		// TODO:
		// [X] Handle error cases.
		// [X] Log necessary info.
		// [] No unconditional loop.
		// [X] Reserve vectors.
		// [X] Log socket details.
		// [X] Connection manager.
		// [] Implement routing.

		while (true)
		{
			watched.push_back(&serverChannel);
			connectionManager->forEach([&](interface::Channel &channel) { watched.push_back(&channel); });

			socketLoop->poll(watched, events);

			// NOTE:
			// - Do not modify container (events) while in loop.
			// - Do not clear buffer vector unless you re-assign, let it over-write.
			for (interface::SocketEvent &event : events)
			{
				if (event.channel.getFd() == serverChannel.getFd())
				{
					logger->log(enums::LogType::INFO, "\033[36mHttp dispatcher:\033[0m Incoming connection request.");
					std::unique_ptr<interface::Channel> clientChannel{acceptor->accept()};
					if (!clientChannel)
					{
						logger->log(enums::LogType::ERROR, "\033[36mHttp dispatcher:\033[0m Failed to accept channel.");
						continue;
					}

					socketLoop->registerChannel(*clientChannel);
					connectionManager->add(std::move(clientChannel));
					continue;
				}

				if (event.flag == enums::SocketEvent::HANGUP)
				{
					logger->log(enums::LogType::INFO, "\033[36mHttp dispatcher:\033[0m Connection closed by the peer.");
					connectionManager->remove(event.channel);
					continue;
				}
				else if (event.flag == enums::SocketEvent::ERROR)
				{
					logger->log(enums::LogType::ERROR, "\033[36mHttp dispatcher:\033[0m Connection lost.");
					connectionManager->remove(event.channel);
					continue;
				}

				transport::PlainTextChannel *plainTextChannel{
					dynamic_cast<transport::PlainTextChannel *>(&event.channel)};
				if (!plainTextChannel)
				{
					logger->log(enums::LogType::ERROR, "\033[36mHttp dispatcher:\033[0m Failed to cast channel.");
					continue;
				}

				if (event.flag == enums::SocketEvent::READ)
				{
					bytesRead = event.channel.readRaw(buffer.data(), buffer.size());

					if (bytesRead == 0)
					{
						connectionManager->remove(event.channel);
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::ERROR});
						logger->log(enums::LogType::WARNING,
									"\033[36mHttp dispatcher:\033[0m Failed to read bytes, peer doesn't answer.");
						continue;
					}
					if (bytesRead < 0)
					{
						connectionManager->remove(event.channel);
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::ERROR});
						logger->log(enums::LogType::ERROR,
									"\033[36mHttp dispatcher:\033[0m Failed to read bytes, closing peer socket.");
						continue;
					}

					std::string rawRequest(buffer.data(), bytesRead);
					std::unique_ptr<interface::Message> message{parser->parse(rawRequest)};

					if (!message) // Gracefully fallback
					{
						logger->log(enums::LogType::ERROR, "\033[36mHttp dispatcher:\033[0m Failed to parse request.");
						logger->log(enums::LogType::WARNING,
									"\033[36mHttp dispatcher:\033[0m Sending “Internal Error” response anyway.");
						format::http::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(errorResponse);
						// fdsToClose.push_back(event.channel.getFd());
					}
					else
					{
						auto *req = dynamic_cast<format::http::Request *>(message.get()); // Using "message" as request.

						std::unique_ptr<interface::Message> response{handler->handle(*message)};
						if (!response) // Gracefully fallback
						{
							logger->log(enums::LogType::ERROR,
										"\033[36mHttp dispatcher:\033[0m Handler returned null response.");
							logger->log(enums::LogType::WARNING,
										"\033[36mHttp dispatcher:\033[0m Sending “Internal Error” response anyway.");
							format::http::Response errorResponse;
							errorResponse.status = 500;
							errorResponse.statusText = "Internal Error";
							errorResponse.version = "HTTP/1.1";
							errorResponse.headers["Connection"] = "close";
							plainTextChannel->getResponse() = serializer->serialize(errorResponse);
							continue;
						}
						plainTextChannel->getResponse() = serializer->serialize(*response);
						if (!req)
						{
							logger->log(enums::LogType::ERROR,
										"\033[36mHttp dispatcher:\033[0m Parsed message was not a Request.");
						}
						else
						{
							logger->log(enums::LogType::INFO,
										"\033[36mHttp dispatcher:\033[0m " + req->method + ' ' + req->path);
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

			connectionManager->forEachClosed(
				[&](interface::Channel &channel) { socketLoop->unregisterChannel(channel); });
			connectionManager->clear();

			bytesRead = 0;
			events.clear();
			watched.clear();
		}
	};
} // namespace sews::runtime::http
