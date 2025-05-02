#include <sstream>

#include "core/connection/reactor/loop_info.hpp"

#include "architecture/connection/transport/plain_text_channel.hpp"
#include "architecture/message/http/transport/request.hpp"
#include "architecture/message/http/transport/response.hpp"

#include "infrastructure/control/stop_signal.hpp"

#include "architecture/runtime/orchestrator/http/http_dispatcher.hpp"

namespace sews::architecture::runtime::orchestrator::http
{
	Dispatcher::Dispatcher(std::unique_ptr<core::connection::reactor::Acceptor> acceptor,
						   std::unique_ptr<core::connection::reactor::Loop> socketLoop,
						   std::unique_ptr<core::connection::Manager> connectionManager,
						   std::unique_ptr<core::message::dispatch::Router> router,
						   std::unique_ptr<core::message::codec::Decoder> parser,
						   std::unique_ptr<core::message::codec::Encoder> serializer,
						   std::shared_ptr<core::telemetry::metric::Manager> metricsManager,
						   std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger)
		: acceptor(std::move(acceptor)), socketLoop(std::move(socketLoop)),
		  connectionManager(std::move(connectionManager)), router(std::move(router)), parser(std::move(parser)),
		  serializer(std::move(serializer)), metricsManager(metricsManager), logger(logger)
	{
		metricsManager->registerMetric("requests_total", core::telemetry::metric::type::Metric::COUNTER);
		metricsManager->registerMetric("responses_404", core::telemetry::metric::type::Metric::COUNTER);
		metricsManager->registerMetric("responses_500", core::telemetry::metric::type::Metric::COUNTER);
		metricsManager->registerMetric("connections_total", core::telemetry::metric::type::Metric::GAUGE);
		this->logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
						  "\033[36mHttp Dispatcher:\033[0m Initialized.");
	}

	Dispatcher::~Dispatcher(void)
	{
		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
					"\033[36mHttp Dispatcher:\033[0m Terminated.");
	}

	void Dispatcher::run(void)
	{
		core::connection::transport::Channel &serverChannel{acceptor->getListener()};
		socketLoop->registerChannel(serverChannel);
		std::vector<core::connection::transport::Channel *> watched;
		std::vector<core::connection::transport::Event> events;
		std::vector<char> buffer(8192);
		ssize_t bytesRead{0};
		// TODO:
		// [] Inject capacity
		constexpr size_t DEFAULT_EVENT_CAPACITY = 64;
		{
			size_t cap{DEFAULT_EVENT_CAPACITY};
			// NOTE:
			// - One time cost.
			// - Cohession for performance, reserves vector capacities.
			// - Resizing vectors is meaningless, only reserving for backend makes sense.
			if (auto *info = dynamic_cast<core::connection::reactor::LoopInfo *>(socketLoop.get()))
			{
				cap = info->getEventCapacity();
			}
			events.reserve(cap);
			watched.reserve(cap + 1);
		}

		// TODO:
		// [X] Store ip & port at channel when client connected.
		// [] Store a buffer at channel for large file transfers, AKA partial write.
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
			for (core::connection::transport::Event &event : events)
			{
				if (event.channel->getFd() == serverChannel.getFd())
				{
					logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
								"\033[36mHttp Dispatcher:\033[0m Incoming connection request.");
					std::unique_ptr<core::connection::transport::Channel> clientChannel{acceptor->accept()};
					if (!clientChannel)
					{
						logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Failed to accept channel.");
						continue;
					}

					socketLoop->registerChannel(*clientChannel);
					connectionManager->add(std::move(clientChannel));
					metricsManager->increment("connections_total");
					continue;
				}

				if (event.flag == core::connection::event::Channel::HANGUP)
				{
					logger->log(
						core::telemetry::diagnostic::logger::type::Log::INFO,
						"\033[36mHttp Dispatcher:\033[0m Connection closed by the peer, marking it for termination.");
					connectionManager->remove(*event.channel);
					continue;
				}

				if (event.flag == core::connection::event::Channel::ERROR)
				{
					logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR,
								"\033[36mHttp Dispatcher:\033[0m Connection lost, marking it for termination.");
					connectionManager->remove(*event.channel);
					continue;
				}

				sews::architecture::connection::transport::PlainTextChannel *plainTextChannel{
					dynamic_cast<connection::transport::PlainTextChannel *>(event.channel)};
				std::pair<std::string, uint16_t> channelDetails{plainTextChannel->getDetails()};

				if (!plainTextChannel)
				{
					logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR,
								"\033[36mHttp Dispatcher:\033[0m Failed to cast channel.");
					continue;
				}

				if (event.flag == core::connection::event::Channel::READ)
				{
					bytesRead = event.channel->readRaw(buffer.data(), buffer.size());

					if (bytesRead == 0)
					{
						connectionManager->remove(*event.channel);
						socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::ERROR});
						std::ostringstream oss;
						oss << "\033[36mHttp Dispatcher:\033[0m Failed to read bytes, connection closed by peer. "
							<< "Channel marked for closure -> \033[33m" << channelDetails.first << ':'
							<< channelDetails.second << "\033[0m, fd = \033[33m" << plainTextChannel->getFd();
						logger->log(core::telemetry::diagnostic::logger::type::Log::INFO, oss.str());
						continue;
					}

					if (bytesRead < 0)
					{
						connectionManager->remove(*event.channel);
						socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::ERROR});
						std::ostringstream oss;
						oss << "\033[36mHttp Dispatcher:\033[0m Failed to read bytes, connection abruptly closed. "
							<< "Channel marked for closure -> \033[33m" << channelDetails.first << ':'
							<< channelDetails.second << "\033[0m, fd = \033[33m" << plainTextChannel->getFd();
						logger->log(core::telemetry::diagnostic::logger::type::Log::INFO, oss.str());
						continue;
					}

					std::string rawRequest(buffer.data(), bytesRead);
					std::unique_ptr<core::message::transport::Message> message{parser->parse(rawRequest)};

					if (!message) // Gracefully fallback
					{
						logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Failed to parse raw request.");
						logger->log(
							core::telemetry::diagnostic::logger::type::Log::INFO,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						architecture::message::http::transport::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						std::string serialized = serializer->serialize(errorResponse);
						std::vector<char> &buf = plainTextChannel->getWriteBuffer();
						buf.assign(serialized.begin(), serialized.end());
						plainTextChannel->getWriteOffset() = 0;
						socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::WRITE});
						metricsManager->increment("responses_500");
						continue;
					}

					architecture::message::http::transport::Request *req =
						dynamic_cast<architecture::message::http::transport::Request *>(
							message.get()); // Using "message" as request.

					if (!req)
					{
						logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR,
									"\033[36mHttp Dispatcher:\033[0m Failed to cast message as request.");
						logger->log(
							core::telemetry::diagnostic::logger::type::Log::INFO,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						architecture::message::http::transport::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						std::string serialized = serializer->serialize(errorResponse);
						std::vector<char> &buf = plainTextChannel->getWriteBuffer();
						buf.assign(serialized.begin(), serialized.end());
						plainTextChannel->getWriteOffset() = 0;
						socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::WRITE});
						metricsManager->increment("responses_500");
						continue;
					}

					{
						std::ostringstream oss;
						oss << "\033[36mHttp Dispatcher:\033[33m " << req->method << ' ' << req->path
							<< "\033[0m from \033[33m" << channelDetails.first << ':' << channelDetails.second
							<< "\033[0m, fd = \033[33m" << plainTextChannel->getFd();
						logger->log(core::telemetry::diagnostic::logger::type::Log::INFO, oss.str());
					}

					core::message::dispatch::Handler *handler{router->match(*req)};
					metricsManager->increment("requests_total");

					if (!handler) // Gracefully fallback
					{
						logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
									"\033[36mHttp Dispatcher:\033[0m No handler found for "
									"request, sending \033[33m404 Not Found\033[0m anyway.");
						architecture::message::http::transport::Response notFoundResp;
						notFoundResp.status = 404;
						notFoundResp.statusText = "Not Found";
						notFoundResp.version = "HTTP/1.1";
						notFoundResp.headers["Connection"] = "keep-alive";
						std::string serialized = serializer->serialize(notFoundResp);
						std::vector<char> &buf = plainTextChannel->getWriteBuffer();
						buf.assign(serialized.begin(), serialized.end());
						plainTextChannel->getWriteOffset() = 0;
						socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::WRITE});
						metricsManager->increment("responses_404");
						continue;
					}

					std::unique_ptr<core::message::transport::Message> response{handler->handle(*message)};
					if (!response) // Gracefully fallback
					{
						logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
									"\033[36mHttp Dispatcher:\033[0m Handler returned null pointer.");
						logger->log(
							core::telemetry::diagnostic::logger::type::Log::INFO,
							"\033[36mHttp Dispatcher:\033[0m Sending \033[33mInternal Error\033[0m response anyway.");
						architecture::message::http::transport::Response errorResponse;
						errorResponse.status = 500;
						errorResponse.statusText = "Internal Error";
						errorResponse.version = "HTTP/1.1";
						errorResponse.headers["Connection"] = "close";
						std::string serialized = serializer->serialize(errorResponse);
						std::vector<char> &buf = plainTextChannel->getWriteBuffer();
						buf.assign(serialized.begin(), serialized.end());
						plainTextChannel->getWriteOffset() = 0;
						socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::WRITE});
						metricsManager->increment("responses_500");
						continue;
					}

					{ // Don't want any left-over, so these are in a scope.
						std::string serialized = serializer->serialize(*response);
						std::vector<char> &buf = plainTextChannel->getWriteBuffer();
						buf.assign(serialized.begin(), serialized.end());
						plainTextChannel->getWriteOffset() = 0;
					}

					socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::WRITE});
				}

				if (event.flag == core::connection::event::Channel::WRITE)
				{
					sews::core::connection::event::Write result{plainTextChannel->flush()};
					std::ostringstream oss;

					switch (result)
					{
						case core::connection::event::Write::DONE:
							oss << "\033[36mHttp Dispatcher:\033[0m Response successfully flushed to \033[33m"
								<< channelDetails.first << ':' << channelDetails.second << "\033[0m, fd = \033[33m"
								<< plainTextChannel->getFd();
							logger->log(core::telemetry::diagnostic::logger::type::Log::INFO, oss.str());
							socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::READ});
							break;

						case core::connection::event::Write::WOULDBLOCK:
							oss << "\033[36mHttp Dispatcher:\033[0m Response will continue flushing to \033[33m"
								<< channelDetails.first << ':' << channelDetails.second << "\033[0m, fd = \033[33m"
								<< plainTextChannel->getFd();
							logger->log(core::telemetry::diagnostic::logger::type::Log::INFO, oss.str());
							socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::WRITE});
							break;

						case core::connection::event::Write::FAILED:
							oss << "\033[36mHttp Dispatcher:\033[0m Failed to flush response to \033[33m"
								<< channelDetails.first << ':' << channelDetails.second << "\033[0m, fd = \033[33m"
								<< plainTextChannel->getFd();
							logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR, oss.str());
							connectionManager->remove(*event.channel); // Don't forget this!
							socketLoop->updateEvents(*plainTextChannel, {core::connection::event::Channel::ERROR});
							break;
					}
				}
			}

			connectionManager->forEachClosed([&](int &fd) {
				socketLoop->unregisterChannel(fd);
				metricsManager->decrement("connections_total");
			});

			connectionManager->clear();

			bytesRead = 0;
			events.clear();
			watched.clear();
		}

		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
					"\033[36mHttp Dispatcher:\033[0m Shutting down, closing active connections...");

		connectionManager->forEach(
			[&](core::connection::transport::Channel &channel) { socketLoop->unregisterChannel(channel.getFd()); });
		socketLoop->unregisterChannel(serverChannel.getFd());
		connectionManager->clear();
		serverChannel.close();

		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
					"\033[36mHttp Dispatcher:\033[0m Shutdown complete.");
	};
} // namespace sews::architecture::runtime::orchestrator::http
