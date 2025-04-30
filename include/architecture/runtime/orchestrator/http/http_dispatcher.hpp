#ifndef SEWS_ARCHITECTURE_RUNTIME_HTTP_DISPATCHER_HPP
#define SEWS_ARCHITECTURE_RUNTIME_HTTP_DISPATCHER_HPP

#include "core/connection/manager.hpp"
#include "core/connection/reactor/acceptor.hpp"
#include "core/connection/reactor/loop.hpp"
#include "core/message/codec/decoder.hpp"
#include "core/message/codec/encoder.hpp"
#include "core/message/dispatch/router.hpp"
#include "core/runtime/orchestrator/dispatcher.hpp"
#include "core/telemetry/diagnostic/transport/logger.hpp"
#include "core/telemetry/metric/manager.hpp"
#include <memory>

namespace sews::architecture::runtime::orchestrator::http
{
	struct Dispatcher : public core::runtime::orchestrator::Dispatcher
	{
		Dispatcher(std::unique_ptr<core::connection::reactor::Acceptor> acceptor,
				   std::unique_ptr<core::connection::reactor::SocketLoop> socketLoop,
				   std::unique_ptr<core::connection::ConnectionManager> connectionManager,
				   std::unique_ptr<core::message::dispatch::Router> router,
				   std::unique_ptr<core::message::codec::RequestParser> parser,
				   std::unique_ptr<core::message::codec::ResponseSerializer> serializer,
				   std::shared_ptr<core::telemetry::metric::MetricsManager> metricsManager,
				   std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger);

		~Dispatcher(void) override;
		void run(void) override;

	  private:
		std::unique_ptr<core::connection::reactor::Acceptor> acceptor;
		std::unique_ptr<core::connection::reactor::SocketLoop> socketLoop;
		std::unique_ptr<core::connection::ConnectionManager> connectionManager;
		std::unique_ptr<core::message::dispatch::Router> router;
		std::unique_ptr<core::message::codec::RequestParser> parser;
		std::unique_ptr<core::message::codec::ResponseSerializer> serializer;
		std::shared_ptr<core::telemetry::metric::MetricsManager> metricsManager;
		std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger;
	};
} // namespace sews::architecture::runtime::orchestrator::http

#endif // !SEWS_ARCHITECTURE_RUNTIME_HTTP_DISPATCHER_HPP
