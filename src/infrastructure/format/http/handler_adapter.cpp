#include "sews/infrastructure/format/http/handler_adapter.hpp"
#include "sews/core/enums/log_type.hpp"

namespace sews::format::http
{
	HandlerAdapter::HandlerAdapter(format::http::Handler &handler, interface::Logger *logger)
		: impl(handler), logger(logger)
	{
		logger->log(enums::LogType::INFO, "\033[36mHTTP Handler Adapter:\033[0m Initialized.");
	}

	HandlerAdapter::~HandlerAdapter(void)
	{
		logger->log(enums::LogType::INFO, "\033[36mHTTP Handler Adapter:\033[0m Terminated.");
	}

	std::unique_ptr<sews::interface::Message> HandlerAdapter::handle(const sews::interface::Message &message)
	{
		if (message.type() != enums::MessageType::HttpRequest)
		{
			logger->log(enums::LogType::ERROR, "\033[36mHTTP Handler Adapter:\033[0m Inconvinient message type.");
			return nullptr;
		}
		auto *request{dynamic_cast<const format::http::Request *>(&message)};
		if (!request)
		{
			logger->log(enums::LogType::ERROR, "\033[36mHTTP Handler Adapter:\033[0m Failed to cast message.");
			return nullptr;
		}
		std::unique_ptr<format::http::Response> response{impl.handle(*request)};
		return std::move(response);
	}
} // namespace sews::format::http
