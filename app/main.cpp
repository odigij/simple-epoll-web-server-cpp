#include "sews/bootstrap/socket_factory.hpp"
#include "sews/infrastructure/format/http/handler_adapter.hpp"
#include "sews/infrastructure/format/http/request_parser.hpp"
#include "sews/infrastructure/format/http/response_serializer.hpp"
#include "sews/infrastructure/io/epoll/acceptor.hpp"
#include "sews/infrastructure/io/epoll/socket_poll.hpp"
#include "sews/infrastructure/io/logger/basic_logger.hpp"
#include "sews/infrastructure/runtime/http/http_dispatcher.hpp"
#include "../test/include/http/test_handler.hpp"
#include <iostream>
#include <memory>

int main(int argc, char *argv[])
{
	int serverFd = sews::bootstrap::createSocket(8080);
	if (serverFd < 0)
	{
		std::cerr << "Failed to initialize server.\n";
		return 1;
	}
	sews::test::http::TestHandler testHandler;

	auto acceptor = std::make_unique<sews::io::epoll::Acceptor>(serverFd);
	auto socketLoop = std::make_unique<sews::io::epoll::SocketLoop>();
	auto parser = std::make_unique<sews::format::http::RequestParser>();
	auto handler = std::make_unique<sews::format::http::HandlerAdapter>(testHandler);
	auto serializer = std::make_unique<sews::format::http::ResponseSerializer>();
	auto logger = std::make_unique<sews::io::logger::BasicLogger>();

	sews::runtime::http::Dispatcher server(std::move(acceptor), std::move(socketLoop), std::move(parser),
										   std::move(handler), std::move(serializer), std::move(logger));

	server.run();
	return 0;
}
