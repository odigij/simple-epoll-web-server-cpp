#include "sews/bootstrap/signal_handler.hpp"
#include "sews/bootstrap/socket_factory.hpp"
#include "sews/infrastructure/connection/manager.hpp"
#include "sews/infrastructure/format/http/request_parser.hpp"
#include "sews/infrastructure/format/http/response_serializer.hpp"
#include "sews/infrastructure/format/http/router/trie.hpp"
#include "sews/infrastructure/io/epoll/acceptor.hpp"
#include "sews/infrastructure/io/epoll/socket_poll.hpp"
#include "sews/infrastructure/io/logger/basic_logger.hpp"
#include "sews/runtime/http/http_dispatcher.hpp"
#include "sews/runtime/metrics/core/manager.hpp"
#include "sews/runtime/metrics/handlers/http_exporter.hpp"

#include "../test/include/http/test_handler.hpp"

int main(int argc, char *argv[])
{
	// TODO:
	// [] Parse terminal commands.
	sews::bootstrap::setupSignalHandlers();
	auto logger = std::make_shared<sews::io::logger::BasicLogger>();
	int port{8080}, serverFd{sews::bootstrap::createSocket(port)};
	if (serverFd < 0)
	{
		logger->log(sews::enums::LogType::ERROR, "\033[36mSEWS:\033[33m Failed to create server socket.");
		return 1;
	}

	auto acceptor = std::make_unique<sews::io::epoll::Acceptor>(serverFd, logger);
	auto socketLoop = std::make_unique<sews::io::epoll::SocketLoop>(64, logger);
	auto connectionManager = std::make_unique<sews::connection::Manager>(logger);
	auto router = std::make_unique<sews::format::http::TrieRouter>(logger);
	auto parser = std::make_unique<sews::format::http::RequestParser>(logger);
	auto metricsManager = std::make_shared<sews::runtime::metrics::Manager>(logger);
	auto serializer = std::make_unique<sews::format::http::ResponseSerializer>(logger);

	auto helloWorldPlainTextHandler = std::make_unique<sews::test::http::HelloWorldPlainText>();
	auto helloWorldHtmlHandler = std::make_unique<sews::test::http::HelloWorldHtml>();
	auto metricsHandler =
		std::make_unique<sews::runtime::metrics::HttpMetricExporter>(metricsManager.get(), serializer.get());

	router->add("GET", "test/http/plain-text", helloWorldPlainTextHandler.get());
	router->add("GET", "test/http/html", helloWorldHtmlHandler.get());
	router->add("GET", "metrics", metricsHandler.get());

	logger->log(sews::enums::LogType::INFO, "\033[36mSEWS:\033[33m http://127.0.0.1:" + std::to_string(port));

	sews::runtime::http::Dispatcher server(std::move(acceptor), std::move(socketLoop), std::move(connectionManager),
										   std::move(router), std::move(parser), std::move(serializer), metricsManager,
										   logger);
	server.run();

	return 0;
}
