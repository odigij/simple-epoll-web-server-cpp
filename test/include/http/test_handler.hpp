#ifndef SEWS_TEST_HTTP_HANDLER_HPP
#define SEWS_TEST_HTTP_HANDLER_HPP

#include "sews/core/interface/message_handler.hpp"
#include "sews/infrastructure/format/http/request.hpp"
#include "sews/infrastructure/format/http/response.hpp"
#include <memory>

namespace sews::test::http
{
	struct HelloWorldPlainText : public interface::MessageHandler
	{
		std::unique_ptr<interface::Message> handle(const interface::Message &message) override
		{
			const format::http::Request *request = dynamic_cast<const format::http::Request *>(&message);
			auto response = std::make_unique<format::http::Response>();
			response->headers["Content-Type"] = "text/plain";
			response->headers["Connection"] = "keep-alive";
			response->status = 200;
			response->statusText = "OK";
			response->body = "Hello World!";
			return response;
		};
	};

	struct HelloWorldHtml : public interface::MessageHandler
	{
		std::unique_ptr<interface::Message> handle(const interface::Message &message) override
		{
			const format::http::Request *request = dynamic_cast<const format::http::Request *>(&message);
			auto response = std::make_unique<format::http::Response>();
			response->headers["Content-Type"] = "text/html";
			response->headers["Connection"] = "keep-alive";
			response->status = 200;
			response->statusText = "OK";
			response->body = "<!doctype html><html lang = \"en\" > <head><meta charset = \"UTF-8\" /><meta name = "
							 "\"viewport\" content = \"width=device-width, initial-scale=1.0\" /><title>Hello World"
							 "TEST</title></head><body><p>Hello World!</p></body></html>";
			return response;
		};
	};
} // namespace sews::test::http

#endif // !SEWS_TEST_HTTP_HANDLER_HPP
