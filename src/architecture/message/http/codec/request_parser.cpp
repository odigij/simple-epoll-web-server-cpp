#include "architecture/message/http/codec/request_parser.hpp"
#include "architecture/message/http/transport/request.hpp"
#include <sstream>

namespace sews::architecture::message::http::codec
{
	RequestParser::RequestParser(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger)
		: logger(logger)
	{
		logger->log(core::telemetry::diagnostic::LogType::INFO, "\033[36mHTTP Request Parser:\033[0m Initialized.");
	}

	RequestParser::~RequestParser(void)
	{
		logger->log(core::telemetry::diagnostic::LogType::INFO, "\033[36mHTTP Request Parser:\033[0m Terminated.");
	}

	std::unique_ptr<core::message::transport::Message> RequestParser::parse(const std::string &raw)
	{
		std::istringstream iss(raw);
		std::unique_ptr<transport::Request> request = std::make_unique<transport::Request>();

		iss >> request->method >> request->path >> request->version;

		std::string buffer{};
		std::getline(iss, buffer);
		while (std::getline(iss, buffer))
		{
			if (!buffer.empty() && buffer.back() == '\r')
			{
				buffer.pop_back();
			}

			if (buffer.empty())
			{
				break;
			}

			size_t delimiterPos{buffer.find(':')};
			if (delimiterPos == std::string::npos)
			{
				continue;
			}

			std::string key{buffer.substr(0, delimiterPos)}, value{buffer.substr(delimiterPos + 1)};

			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);

			request->headers[key] = value;
		}

		auto it = request->headers.find("Content-Length");
		if (it != request->headers.end())
		{
			size_t contentLength{std::stoul(it->second)};
			std::string body(contentLength, '\0');
			iss.read(&body[0], contentLength);
			request->body = body;
		}
		return request;
	}
} // namespace sews::architecture::message::http::codec
