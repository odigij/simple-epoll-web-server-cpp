#include "sews/infrastructure/format/http/handler_adapter.hpp"

namespace sews::format::http
{
	HandlerAdapter::HandlerAdapter(format::http::Handler &handler) : impl(handler)
	{
	}

	std::unique_ptr<sews::interface::Message> HandlerAdapter::handle(const sews::interface::Message &message)
	{
		if (message.type() != enums::MessageType::HttpRequest)
		{
			return nullptr;
		}
		auto *request{dynamic_cast<const format::http::Request *>(&message)};
		if (!request)
		{
			return nullptr;
		}
		std::unique_ptr<format::http::Response> response{impl.handle(*request)};
		return std::move(response);
	}
} // namespace sews::format::http
