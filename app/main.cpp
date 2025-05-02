#include "architecture/connection/manager.hpp"
#include "architecture/message/http/codec/request_parser.hpp"
#include "architecture/message/http/codec/response_serializer.hpp"
#include "architecture/message/http/dispatch/trie_router.hpp"
#include "architecture/runtime/orchestrator/http/http_dispatcher.hpp"
#include "architecture/telemetry/metric/exporter/http_exporter.hpp"
#include "architecture/telemetry/metric/manager.hpp"
#include "infrastructure/bootstrap/signal_handler.hpp"
#include "architecture/telemetry/diagnostic/logger/backend/basic.hpp"
#include "infrastructure/bootstrap/socket_factory.hpp"
#include "infrastructure/connection/reactor/epoll/acceptor.hpp"
#include "infrastructure/connection/reactor/epoll/loop.hpp"

#include "../test/include/http/test_handler.hpp"

int main(int argc, char *argv[])
{
	// TODO:
	// [] Parse terminal commands.
	sews::infrastructure::bootstrap::setupSignalHandlers();
	auto logger = std::make_shared<sews::architecture::telemetry::diagnostic::logger::backend::Basic>();
	int port{8080}, serverFd{sews::infrastructure::bootstrap::createSocket(port)};
	if (serverFd < 0)
	{
		logger->log(sews::core::telemetry::diagnostic::logger::type::Log::ERROR,
					"\033[36mSEWS:\033[33m Failed to create server socket.");
		return 1;
	}

	auto acceptor = std::make_unique<sews::infrastructure::connection::reactor::epoll::Acceptor>(serverFd, logger);
	auto socketLoop = std::make_unique<sews::infrastructure::connection::reactor::epoll::SocketLoop>(64, logger);
	auto connectionManager = std::make_unique<sews::architecture::connection::Manager>(logger);
	auto router = std::make_unique<sews::architecture::message::http::dispatch::TrieRouter>(logger);
	auto parser = std::make_unique<sews::architecture::message::http::codec::RequestParser>(logger);
	auto metricsManager = std::make_shared<sews::architecture::telemetry::metric::Manager>(logger);
	auto serializer = std::make_unique<sews::architecture::message::http::codec::ResponseSerializer>(logger);

	auto helloWorldPlainTextHandler = std::make_unique<sews::test::http::HelloWorldPlainText>();
	auto helloWorldHtmlHandler = std::make_unique<sews::test::http::HelloWorldHtml>();
	auto metricsHandler = std::make_unique<sews::architecture::telemetry::metric::exporter::HttpMetricExporter>(
		metricsManager.get(), serializer.get());

	router->add("GET", "test/http/plain-text", helloWorldPlainTextHandler.get());
	router->add("GET", "test/http/html", helloWorldHtmlHandler.get());
	router->add("GET", "metrics", metricsHandler.get());

	logger->log(sews::core::telemetry::diagnostic::logger::type::Log::INFO,
				"\033[36mSEWS:\033[33m http://127.0.0.1:" + std::to_string(port));

	sews::architecture::runtime::orchestrator::http::Dispatcher server(
		std::move(acceptor), std::move(socketLoop), std::move(connectionManager), std::move(router), std::move(parser),
		std::move(serializer), metricsManager, logger);
	server.run();

	return 0;
}
