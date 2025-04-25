#ifndef SEWS_INFRASTRUCTURE_HTTP_HANDLER_ADAPTER_HPP
#define SEWS_INFRASTRUCTURE_HTTP_HANDLER_ADAPTER_HPP

#include "sews/core/interface/message_handler.hpp"
#include "sews/core/interface/handler.hpp"

namespace sews::format::http
{
	class HandlerAdapter : public interface::MessageHandler
	{
	  public:
		HandlerAdapter(format::http::Handler &handler);
		std::unique_ptr<interface::Message> handle(const interface::Message &message) override;

	  private:
		format::http::Handler &impl;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_HTTP_HANDLER_ADAPTER_HPP
