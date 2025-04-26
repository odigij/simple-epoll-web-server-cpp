#ifndef SEWS_INFRASTRUCTURE_RUNTIME_HTTP_DISPATCHER_HPP
#define SEWS_INFRASTRUCTURE_RUNTIME_HTTP_DISPATCHER_HPP

#include "sews/core/interface/acceptor.hpp"
#include "sews/core/interface/message_handler.hpp"
#include "sews/core/interface/request_parser.hpp"
#include "sews/core/interface/response_serializer.hpp"
#include "sews/core/interface/server.hpp"
#include "sews/core/interface/socket_loop.hpp"
#include "sews/core/interface/logger.hpp"
#include <unordered_map>

namespace sews::runtime::http
{
	struct Dispatcher : public interface::Dispatcher
	{
		Dispatcher(std::unique_ptr<interface::Acceptor> acceptor, std::unique_ptr<interface::SocketLoop> socketLoop,
				   std::unique_ptr<interface::RequestParser> parser, std::unique_ptr<interface::MessageHandler> handler,
				   std::unique_ptr<interface::ResponseSerializer> serializer, interface::Logger *logger);

		~Dispatcher(void) override;
		void run(void) override;

	  private:
		std::unique_ptr<interface::Acceptor> acceptor;
		std::unique_ptr<interface::SocketLoop> socketLoop;
		std::unique_ptr<interface::RequestParser> parser;
		std::unique_ptr<interface::MessageHandler> handler;
		std::unique_ptr<interface::ResponseSerializer> serializer;
		std::unique_ptr<interface::Logger> logger;

		std::unordered_map<int, std::unique_ptr<interface::Channel>> activeChannels;
	};
} // namespace sews::runtime::http

#endif // !SEWS_INFRASTRUCTURE_RUNTIME_HTTP_DISPATCHER_HPP
