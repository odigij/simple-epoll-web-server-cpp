#ifndef SEWS_TEST_HTTP_HANDLER_HPP
#define SEWS_TEST_HTTP_HANDLER_HPP

#include "architecture/message/http/transport/request.hpp"
#include "core/message/dispatch/handler.hpp"
#include "architecture/message/http/transport/response.hpp"
#include <memory>

namespace sews::test::http
{
	struct HelloWorldPlainText : public core::message::dispatch::MessageHandler
	{
		std::unique_ptr<core::message::transport::Message> handle(
			const core::message::transport::Message &message) override
		{
			const architecture::message::http::transport::Request *request =
				dynamic_cast<const architecture::message::http::transport::Request *>(&message);
			auto response = std::make_unique<architecture::message::http::transport::Response>();
			response->headers["Content-Type"] = "text/plain";
			response->headers["Connection"] = "keep-alive";
			response->status = 200;
			response->statusText = "OK";
			response->body = "Hello World!";
			return response;
		};
	};

	struct HelloWorldHtml : public core::message::dispatch::MessageHandler
	{
		std::unique_ptr<core::message::transport::Message> handle(
			const core::message::transport::Message &message) override
		{
			const architecture::message::http::transport::Request *request =
				dynamic_cast<const architecture::message::http::transport::Request *>(&message);
			auto response = std::make_unique<architecture::message::http::transport::Response>();
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
