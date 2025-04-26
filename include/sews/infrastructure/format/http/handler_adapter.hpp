#ifndef SEWS_INFRASTRUCTURE_HTTP_HANDLER_ADAPTER_HPP
#define SEWS_INFRASTRUCTURE_HTTP_HANDLER_ADAPTER_HPP

#include "sews/core/interface/logger.hpp"
#include "sews/core/interface/message_handler.hpp"
#include "sews/core/interface/handler.hpp"

namespace sews::format::http
{
	struct HandlerAdapter : public interface::MessageHandler
	{
		HandlerAdapter(format::http::Handler &handler, interface::Logger *logger);
		~HandlerAdapter(void) override;
		std::unique_ptr<interface::Message> handle(const interface::Message &message) override;

	  private:
		format::http::Handler &impl;
		interface::Logger *logger;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_HTTP_HANDLER_ADAPTER_HPP
