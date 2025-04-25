#include "sews/infrastructure/format/http/request_parser.hpp"
#include "sews/infrastructure/format/http/request.hpp"
#include <sstream>

namespace sews::format::http
{
	std::unique_ptr<interface::Message> RequestParser::parse(const std::string &raw)
	{
		std::istringstream iss(raw);
		std::unique_ptr<format::http::Request> request = std::make_unique<format::http::Request>();

		iss >> request->method >> request->path >> request->version;

		std::string buffer{""};
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
} // namespace sews::format::http
