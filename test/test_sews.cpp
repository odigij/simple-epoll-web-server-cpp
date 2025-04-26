#include "http/test_handler.hpp"
#include "sews/infrastructure/format/http/request.hpp"
#include "sews/infrastructure/format/http/response.hpp"
#include "sews/infrastructure/format/http/handler_adapter.hpp"
#include "sews/infrastructure/format/http/request_parser.hpp"
#include "sews/infrastructure/format/http/response_serializer.hpp"
#include "sews/infrastructure/io/logger/null_logger.hpp"
#include <iostream>
#include <memory>
#include <sstream>

std::string requestSerializer(sews::format::http::Request &request)
{
	std::ostringstream oss;
	oss << request.method << " " << request.path << " " << request.version << "\r\n";
	for (auto header : request.headers)
	{
		oss << header.first << ": " << header.second << "\r\n";
	}
	oss << "\r\n" << request.body;
	return oss.str();
}

int main()
{
	using namespace sews;

	{ // HTTP
		std::string request_path_main_segment("/test/http");

		io::logger::NullLogger nullLogger;
		format::http::RequestParser requestParser(&nullLogger);
		format::http::ResponseSerializer responseSerializer(&nullLogger);

		test::http::TestHandler httpHandler;
		format::http::HandlerAdapter httpHandlerAdapter(httpHandler, &nullLogger);

		format::http::Request request;
		request.method = "GET";
		request.path = request_path_main_segment + "/handler";
		request.version = "HTTP/1.1";
		request.body = "Hello World!";
		request.headers = {
			{
				"Host",
				"localhost",
			},
			{
				"Accept",
				"text/plain",
			},
			{
				"Content-Length",
				std::to_string(request.body.size()),
			},
		};

		// Request parser
		{
			std::cout << " HTTP request parser...";

			auto rawRequest = requestSerializer(request);
			std::unique_ptr<interface::Message> msg = requestParser.parse(rawRequest);

			auto castedRequest = dynamic_cast<format::http::Request &>(*msg);
			if (castedRequest.method != request.method || castedRequest.path != request.path ||
				castedRequest.version != request.version)
			{
				std::cerr << "\t󰅘 Start line not parsed correctly.\n";
			}
			if (castedRequest.headers["Host"] != request.headers["Host"] ||
				castedRequest.headers["Accept"] != request.headers["Accept"])
			{
				std::cerr << "\t󰅘 Headers not parsed correctly.\n";
			}
			if (castedRequest.body.empty())
			{
				std::cerr << "\t󰅘 Body should not be empty.\n";
			}

			std::cout << " HTTP request parser.\n";
		}

		{ // Serializer
			std::cout << " HTTP response serializer...";

			format::http::Response response;
			response.version = "HTTP/1.1";
			response.status = 200;
			response.statusText = "OK";
			response.body = "Hello World!";
			response.headers["Content-Length"] = std::to_string(response.body.size());

			std::string serializedResponse = responseSerializer.serialize(response);

			if (serializedResponse.empty())
			{
				std::cerr << "\t󰅘 Type is wrong.\n";
			}

			std::cout << " HTTP response serializer.\n";
		}

		// Handler Positive
		{
			std::cout << " HTTP handler...";

			std::unique_ptr<interface::Message> response = httpHandlerAdapter.handle(request);
			if (!response)
			{
				std::cerr << "\t󰅘 Null response.\n";
			}

			auto &httpResponse = dynamic_cast<format::http::Response &>(*response);
			if (httpResponse.status != 200)
			{
				std::cerr << "\t󰅘 Wrong status code.\n";
			}
			std::cout << " HTTP handler.\n";
		}
	}
	std::cout << " All tests are sucessfull.\n";
	return 0;
}
