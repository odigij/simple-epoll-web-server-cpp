#include "architecture/runtime/orchestrator/http/http_dispatcher.hpp"
#include "architecture/message/http/transport/request.hpp"
#include "core/connection/event_types.hpp"
#include "core/connection/reactor/loop_info.hpp"
#include "core/telemetry/metric/manager.hpp"
#include "core/telemetry/metric/types.hpp"
#include "infrastructure/control/stop_signal.hpp"
#include "architecture/connection/transport/plain_text_channel.hpp"
#include "architecture/message/http/transport/response.hpp"
#include <string>
#include <sstream>

namespace sews::architecture::runtime::orchestrator::http
{
	Dispatcher::Dispatcher(std::unique_ptr<core::connection::reactor::Acceptor> acceptor,
						   std::unique_ptr<core::connection::reactor::SocketLoop> socketLoop,
						   std::unique_ptr<core::connection::ConnectionManager> connectionManager,
						   std::unique_ptr<core::message::dispatch::Router> router,
						   std::unique_ptr<core::message::codec::RequestParser> parser,
						   std::unique_ptr<core::message::codec::ResponseSerializer> serializer,
						   std::shared_ptr<core::telemetry::metric::MetricsManager> metricsManager,
						   std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger)
		: acceptor(std::move(acceptor)), socketLoop(std::move(socketLoop)),
		  connectionManager(std::move(connectionManager)), router(std::move(router)), parser(std::move(parser)),
		  serializer(std::move(serializer)), metricsManager(metricsManager), logger(logger)
	{
		metricsManager->registerMetric("requests_total", core::telemetry::metric::MetricType::COUNTER);
		metricsManager->registerMetric("responses_404", core::telemetry::metric::MetricType::COUNTER);
		metricsManager->registerMetric("responses_500", core::telemetry::metric::MetricType::COUNTER);
		metricsManager->registerMetric("connections_total", core::telemetry::metric::MetricType::GAUGE);
		this->logger->log(core::telemetry::diagnostic::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Initialized.");
	}

	Dispatcher::~Dispatcher(void)
	{
		logger->log(core::telemetry::diagnostic::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Terminated.");
	}

	void Dispatcher::run(void)
	{
		core::connection::transport::Channel &serverChannel{acceptor->getListener()};
		socketLoop->registerChannel(serverChannel);
		std::vector<core::connection::transport::Channel *> watched;
		std::vector<core::connection::transport::SocketEvent> events;
		std::vector<char> buffer(8192);
		ssize_t bytesRead{0};
		constexpr size_t DEFAULT_EVENT_CAPACITY = 64;
		// TODO:
		// [] Inject capacity.
		size_t cap{64};

		// NOTE:
		// - One time cost.
		// - Cohession for performance, reserves vector capacities.
		if (auto *info = dynamic_cast<core::connection::reactor::LoopInfo *>(socketLoop.get()))
		{
			cap = info->getEventCapacity();
		}
		events.reserve(cap);
		watched.reserve(cap + 1);

		// TODO:
		// [X] Store ip & port at channel when client connected.
		// [] Store a buffer at channel for large file transfers.
		// [X] Log socket fd, connection ip & port at event logs.
		// [] An asset manager to register static files to endpoints.
		// [] Implement middleware.
		// [] Reduce dispatcher constructor bloat.
		// [] Implement handler adapter.
		// [] Avoid “PlainTextChannel” casting.
		// [] Extract per-event handling into functions.
		// [] Helper for log formatting.
		// [] Inject stopFlag dependency.

		while (!infrastructure::control::stopFlag.load())
		{
			watched.push_back(&serverChannel);
			connectionManager->forEach(
				[&](core::connection::transport::Channel &channel) { watched.push_back(&channel); });
			socketLoop->poll(watched, events);

			// NOTE:
			// - Do not modify container (events) while in loop.
			// - Do not clear buffer vector unless you re-assign, let it over-write.
			for (core::connection::transport::SocketEvent &event : events)
			{
				if (event.channel.getFd() == serverChannel.getFd())
				{
					logger->log(core::telemetry::diagnostic::LogType::INFO,
								"\033[36mHttp Dispatcher:\033[0m Incoming connection request.");
					std::unique_ptr<core::connection::transport::Channel> clientChannel{acceptor->accept()};
					if (!clientChannel)
					{
						logger->log(core::telemetry::diagnostic::LogType::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Failed to accept channel.");
						continue;
					}

					socketLoop->registerChannel(*clientChannel);
					connectionManager->add(std::move(clientChannel));
					metricsManager->increment("connections_total");
					continue;
				}

				if (event.flag == core::connection::Events::HANGUP)
				{
					logger->log(
						core::telemetry::diagnostic::LogType::INFO,
						"\033[36mHttp Dispatcher:\033[0m Connection closed by the peer, marking it for termination.");
					connectionManager->remove(event.channel);
					continue;
				}

				if (event.flag == core::connection::Events::ERROR)
				{
					logger->log(core::telemetry::diagnostic::LogType::ERROR,
								"\033[36mHttp Dispatcher:\033[0m Connection lost, marking it for termination.");
					connectionManager->remove(event.channel);
					continue;
				}

				connection::transport::PlainTextChannel *plainTextChannel{
					dynamic_cast<connection::transport::PlainTextChannel *>(&event.channel)};
				std::pair<std::string, uint16_t> channelDetails{plainTextChannel->getDetails()};

				if (!plainTextChannel)
				{
					logger->log(core::telemetry::diagnostic::LogType::ERROR,
								"\033[36mHttp Dispatcher:\033[0m Failed to cast channel.");
					continue;
				}

				if (event.flag == core::connection::Events::READ)
				{
					bytesRead = event.channel.readRaw(buffer.data(), buffer.size());

					if (bytesRead == 0)
					{
						connectionManager->remove(event.channel);
						socketLoop->updateEvents(*plainTextChannel, {core::connection::Events::ERROR});
						std::ostringstream oss;
						oss << "\033[36mHttp Dispatcher:\033[0m Failed to read bytes, connection closed by peer. "
							<< "Channel marked for closure -> \033[33m" << channelDetails.first << ':'
							<< channelDetails.second << "\033[0m, fd = \033[33m" << plainTextChannel->getFd();
						logger->log(core::telemetry::diagnostic::LogType::INFO, oss.str());
						continue;
					}

					if (bytesRead < 0)
					{
						connectionManager->remove(event.channel);
						socketLoop->updateEvents(*plainTextChannel, {core::connection::Events::ERROR});
						std::ostringstream oss;
						oss << "\033[36mHttp Dispatcher:\033[0m Failed to read bytes, connection abruptly closed. "
							<< "Channel marked for closure -> \033[33m" << channelDetails.first << ':'
							<< channelDetails.second << "\033[0m, fd = \033[33m" << plainTextChannel->getFd();
						logger->log(core::telemetry::diagnostic::LogType::INFO, oss.str());
						continue;
					}

					std::string rawRequest(buffer.data(), bytesRead);
					std::unique_ptr<core::message::transport::Message> message{parser->parse(rawRequest)};

					if (!message) // Gracefully fallback
					{
						logger->log(core::telemetry::diagnostic::LogType::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Failed to parse raw request.");
						logger->log(
							core::telemetry::diagnostic::LogType::INFO,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						architecture::message::http::transport::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(errorResponse);
						socketLoop->updateEvents(*plainTextChannel, {core::connection::Events::WRITE});
						metricsManager->increment("responses_500");
						continue;
					}

					architecture::message::http::transport::Request *req =
						dynamic_cast<architecture::message::http::transport::Request *>(
							message.get()); // Using "message" as request.

					if (!req)
					{
						logger->log(core::telemetry::diagnostic::LogType::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Failed to cast message as request.");
						logger->log(
							core::telemetry::diagnostic::LogType::INFO,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						architecture::message::http::transport::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(errorResponse);
						socketLoop->updateEvents(*plainTextChannel, {core::connection::Events::WRITE});
						metricsManager->increment("responses_500");
						continue;
					}

					{
						std::ostringstream oss;
						oss << "\033[36mHttp Dispatcher:\033[33m " << req->method << ' ' << req->path
							<< "\033[0m from \033[33m" << channelDetails.first << ':' << channelDetails.second
							<< "\033[0m, fd = \033[33m" << plainTextChannel->getFd();
						logger->log(core::telemetry::diagnostic::LogType::INFO, oss.str());
					}

					core::message::dispatch::MessageHandler *handler{router->match(*req)};
					metricsManager->increment("requests_total");

					if (!handler) // Gracefully fallback
					{
						logger->log(core::telemetry::diagnostic::LogType::INFO,
									"\033[36mHttp Dispatcher:\033[0m No handler found for "
									"request, sending \033[33m404 Not Found\033[0m anyway.");
						architecture::message::http::transport::Response notFoundResp;
						notFoundResp.status = 404;
						notFoundResp.statusText = "Not Found";
						notFoundResp.version = "HTTP/1.1";
						notFoundResp.headers["Connection"] = "keep-alive";
						plainTextChannel->getResponse() = serializer->serialize(notFoundResp);
						socketLoop->updateEvents(*plainTextChannel, {core::connection::Events::WRITE});
						metricsManager->increment("responses_404");
						continue;
					}

					std::unique_ptr<core::message::transport::Message> response{handler->handle(*message)};
					if (!response) // Gracefully fallback
					{
						logger->log(core::telemetry::diagnostic::LogType::WARNING,
									"\033[36mHttp Dispatcher:\033[0m Handler returned null pointer.");
						logger->log(
							core::telemetry::diagnostic::LogType::INFO,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						architecture::message::http::transport::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						plainTextChannel->getResponse() = serializer->serialize(errorResponse);
						socketLoop->updateEvents(*plainTextChannel, {core::connection::Events::WRITE});
						metricsManager->increment("responses_500");
						continue;
					}

					plainTextChannel->getResponse() = serializer->serialize(*response);

					socketLoop->updateEvents(*plainTextChannel, {core::connection::Events::WRITE});
				}

				if (event.flag == core::connection::Events::WRITE)
				{
					std::string &out = plainTextChannel->getResponse();
					plainTextChannel->writeRaw(out.c_str(), out.size());
					out.clear();
					socketLoop->updateEvents(*plainTextChannel, {core::connection::Events::READ});
				}
			}

			connectionManager->forEachClosed([&](core::connection::transport::Channel &channel) {
				socketLoop->unregisterChannel(channel);
				metricsManager->decrement("connections_total");
			});

			connectionManager->clear();

			bytesRead = 0;
			events.clear();
			watched.clear();
		}

		logger->log(core::telemetry::diagnostic::LogType::INFO,
					"\033[36mHttp Dispatcher:\033[0m Shutting down, closing active connections...");

		connectionManager->forEach(
			[&](core::connection::transport::Channel &channel) { socketLoop->unregisterChannel(channel); });
		socketLoop->unregisterChannel(serverChannel);
		connectionManager->clear();
		serverChannel.close();

		logger->log(core::telemetry::diagnostic::LogType::INFO, "\033[36mHttp Dispatcher:\033[0m Shutdown complete.");
	};
} // namespace sews::architecture::runtime::orchestrator::http
