#include "sews/bootstrap/socket_factory.hpp"
#include "sews/infrastructure/format/http/handler_adapter.hpp"
#include "sews/infrastructure/format/http/request_parser.hpp"
#include "sews/infrastructure/format/http/response_serializer.hpp"
#include "sews/infrastructure/io/epoll/acceptor.hpp"
#include "sews/infrastructure/io/epoll/socket_poll.hpp"
#include "sews/infrastructure/io/logger/basic_logger.hpp"
#include "sews/infrastructure/runtime/http/http_dispatcher.hpp"
#include "../test/include/http/test_handler.hpp"
#include <memory>
#include <string>

int main(int argc, char *argv[])
{
	sews::io::logger::BasicLogger logger;
	int port{8080}, serverFd{sews::bootstrap::createSocket(port)};
	if (serverFd < 0)
	{
		logger.log(sews::enums::LogType::ERROR, "Failed to create server socket.");
		return 1;
	}
	sews::test::http::TestHandler testHandler;

	auto acceptor = std::make_unique<sews::io::epoll::Acceptor>(serverFd, &logger);
	auto socketLoop = std::make_unique<sews::io::epoll::SocketLoop>(64, &logger);
	auto parser = std::make_unique<sews::format::http::RequestParser>(&logger);
	auto handler = std::make_unique<sews::format::http::HandlerAdapter>(testHandler, &logger);
	auto serializer = std::make_unique<sews::format::http::ResponseSerializer>(&logger);

	logger.log(sews::enums::LogType::INFO, "\033[36mSEWS: \033[33mhttp://127.0.0.1:" + std::to_string(port));

	sews::runtime::http::Dispatcher server(std::move(acceptor), std::move(socketLoop), std::move(parser),
										   std::move(handler), std::move(serializer), &logger);
	server.run();

	return 0;
}
