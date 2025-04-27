#include "sews/infrastructure/format/http/response.hpp"

namespace sews::format::http
{
	Response::Response(void)
	{
	}

	Response::Response(int status, std::string_view statusText, std::string_view version,
					   std::unordered_map<std::string, std::string> headers, std::string body)
		: status(status), statusText(std::string(statusText)), version(std::string(version)), headers(headers),
		  body(body)
	{
	}

	Response::~Response(void)
	{
	}

	enums::MessageType Response::type(void) const
	{
		return enums::MessageType::HttpResponse;
	}

	std::string Response::payload(void) const
	{
		return body;
	}
} // namespace sews::format::http
