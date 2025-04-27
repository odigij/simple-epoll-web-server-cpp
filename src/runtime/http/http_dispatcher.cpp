#include "sews/runtime/http/http_dispatcher.hpp"
#include "sews/core/stop_flag.hpp"
#include "sews/infrastructure/format/http/request.hpp"
#include "sews/infrastructure/format/http/response.hpp"
#include "sews/infrastructure/io/epoll/socket_poll.hpp"
#include "sews/infrastructure/transport/plain_text_channel.hpp"

namespace sews::runtime::http
{
	Dispatcher::Dispatcher(std::unique_ptr<interface::Acceptor> acceptor,
						   std::unique_ptr<interface::SocketLoop> socketLoop,
						   std::unique_ptr<interface::ConnectionManager> connectionManager,
						   std::unique_ptr<interface::Router> router, std::unique_ptr<interface::RequestParser> parser,
						   std::unique_ptr<interface::ResponseSerializer> serializer,
						   std::shared_ptr<interface::Logger> logger)
		: acceptor(std::move(acceptor)), socketLoop(std::move(socketLoop)),
		  connectionManager(std::move(connectionManager)), router(std::move(router)), parser(std::move(parser)),
		  serializer(std::move(serializer)), logger(logger)
	{
		this->logger->log(enums::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Initialized.");
	}

	Dispatcher::~Dispatcher(void)
	{
		logger->log(enums::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Terminated.");
	}

	void Dispatcher::run(void)
	{
		interface::Channel &serverChannel{acceptor->getListener()};
		socketLoop->registerChannel(serverChannel);
		std::vector<interface::Channel *> watched;
		std::vector<model::SocketEvent> events;
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
		// [X] Implement routing.

		while (!stopFlag.load())
		{
			watched.push_back(&serverChannel);
			connectionManager->forEach([&](interface::Channel &channel) { watched.push_back(&channel); });
			socketLoop->poll(watched, events);

			// NOTE:
			// - Do not modify container (events) while in loop.
			// - Do not clear buffer vector unless you re-assign, let it over-write.
			for (model::SocketEvent &event : events)
			{
				if (event.channel.getFd() == serverChannel.getFd())
				{
					logger->log(enums::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Incoming connection request.");
					std::unique_ptr<interface::Channel> clientChannel{acceptor->accept()};
					if (!clientChannel)
					{
						logger->log(enums::LogType::ERROR, "\033[36mHttp Dispatcher:\033[0m Failed to accept channel.");
						continue;
					}

					socketLoop->registerChannel(*clientChannel);
					connectionManager->add(std::move(clientChannel));
					continue;
				}

				if (event.flag == enums::SocketEvent::HANGUP)
				{
					logger->log(
						enums::LogType::INFO,
						"\033[36mHttp Dispatcher:\033[0m Connection closed by the peer, marking it for termination.");
					connectionManager->remove(event.channel);
					continue;
				}

				if (event.flag == enums::SocketEvent::ERROR)
				{
					logger->log(enums::LogType::ERROR,
								"\033[36mHttp Dispatcher:\033[0m Connection lost, marking it for termination.");
					connectionManager->remove(event.channel);
					continue;
				}

				transport::PlainTextChannel *plainTextChannel{
					dynamic_cast<transport::PlainTextChannel *>(&event.channel)};
				if (!plainTextChannel)
				{
					logger->log(enums::LogType::ERROR, "\033[36mHttp Dispatcher:\033[0m Failed to cast channel.");
					continue;
				}

				if (event.flag == enums::SocketEvent::READ)
				{
					bytesRead = event.channel.readRaw(buffer.data(), buffer.size());

					if (bytesRead == 0)
					{
						connectionManager->remove(event.channel);
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::ERROR});
						logger->log(enums::LogType::WARNING, "\033[36mHttp Dispatcher:\033[0m Failed to read bytes, "
															 "peer doesn't answer. Channel marked to be closed.");
						continue;
					}

					if (bytesRead < 0)
					{
						connectionManager->remove(event.channel);
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::ERROR});
						logger->log(enums::LogType::ERROR, "\033[36mHttp Dispatcher:\033[0m Failed to read bytes, "
														   "connection abruptly closed. Channel marked to be closed.");
						continue;
					}

					std::string rawRequest(buffer.data(), bytesRead);
					std::unique_ptr<interface::Message> message{parser->parse(rawRequest)};

					if (!message) // Gracefully fallback
					{
						logger->log(enums::LogType::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Failed to parse raw request.");
						logger->log(
							enums::LogType::WARNING,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						format::http::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(errorResponse);
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::WRITE});
						continue;
					}

					format::http::Request *req =
						dynamic_cast<format::http::Request *>(message.get()); // Using "message" as request.

					if (!req)
					{
						logger->log(enums::LogType::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Failed to cast message as request.");
						logger->log(
							enums::LogType::WARNING,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						format::http::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(errorResponse);
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::WRITE});
						continue;
					}

					logger->log(enums::LogType::INFO,
								"\033[36mHttp Dispatcher: \033[33m" + req->method + ' ' + req->path);

					interface::MessageHandler *handler{router->match(*req)};

					if (!handler) // Gracefully fallback
					{
						logger->log(enums::LogType::WARNING, "\033[36mHttp Dispatcher:\033[0m No handler found for "
															 "request, sending \033[33m404 Not Found\033[0m anyway.");
						format::http::Response notFoundResp;
						notFoundResp.status = 404;
						notFoundResp.statusText = "Not Found";
						notFoundResp.version = "HTTP/1.1";
						notFoundResp.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(notFoundResp);
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::WRITE});
						continue;
					}

					std::unique_ptr<interface::Message> response{handler->handle(*message)};
					if (!response) // Gracefully fallback
					{
						logger->log(enums::LogType::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Handler returned null pointer.");
						logger->log(
							enums::LogType::WARNING,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						format::http::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(errorResponse);
						socketLoop->updateEvents(*plainTextChannel, {enums::SocketEvent::WRITE});
						continue;
					}

					plainTextChannel->getResponse() = serializer->serialize(*response);

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

		logger->log(enums::LogType::INFO,
					"\033[36mHttp Dispatcher:\033[0m Shutting down, closing active connections...");

		connectionManager->forEach([&](interface::Channel &channel) { socketLoop->unregisterChannel(channel); });
		socketLoop->unregisterChannel(serverChannel);
		connectionManager->clear();
		serverChannel.close();

		logger->log(enums::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Shutdown complete.");
	};
} // namespace sews::runtime::http
