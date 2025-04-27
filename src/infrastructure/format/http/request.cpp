#include "sews/infrastructure/format/http/request.hpp"

namespace sews::format::http
{
	Request::Request(void)
	{
	}

	Request::Request(std::string_view method, std::string_view path, std::string_view version,
					 std::unordered_map<std::string, std::string> headers, std::string_view body)
		: method(std::string(method)), path(std::string(path)), version(std::string(version)), headers(headers),
		  body(std::string(body))
	{
	}

	Request::~Request(void)
	{
	}

	enums::MessageType Request::type(void) const
	{
		return enums::MessageType::HttpRequest;
	}

	std::string Request::payload(void) const
	{
		return body;
	}
} // namespace sews::format::http
