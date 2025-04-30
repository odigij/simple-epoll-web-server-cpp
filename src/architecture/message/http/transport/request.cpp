#include "architecture/message/http/transport/request.hpp"

namespace sews::architecture::message::http::transport
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

	core::message::MessageType Request::type(void) const
	{
		return core::message::MessageType::HttpRequest;
	}

	std::string Request::payload(void) const
	{
		return body;
	}
} // namespace sews::architecture::message::http::transport
