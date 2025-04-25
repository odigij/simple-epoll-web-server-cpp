#ifndef SEWS_TEST_HTTP_HANDLER_HPP
#define SEWS_TEST_HTTP_HANDLER_HPP

#include "sews/core/interface/handler.hpp"
#include "sews/infrastructure/format/http/request.hpp"
#include "sews/infrastructure/format/http/response.hpp"
#include <memory>

namespace sews::test::http
{
	struct TestHandler : public format::http::Handler
	{
		std::unique_ptr<format::http::Response> handle(const format::http::Request &request) override
		{
			auto response = std::make_unique<format::http::Response>();
			response->headers["Content-Type"] = "text/plain";
			if (request.path == "/test/http/handler")
			{
				response->status = 200;
				response->statusText = "OK";
				response->body = request.body;
				response->body.append("Hello World!");
			}
			else
			{
				response->status = 404;
				response->statusText = "Not Found";
				response->body = request.body;
				response->body.append("Oops");
			}
			return response;
		};
	};
} // namespace sews::test::http

#endif // !SEWS_TEST_HTTP_HANDLER_HPP
