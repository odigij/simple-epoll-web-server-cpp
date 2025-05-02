#include "architecture/message/http/transport/response.hpp"

namespace sews::architecture::message::http::transport
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

	core::message::type::Message Response::type(void) const
	{
		return core::message::type::Message::HttpResponse;
	}

	std::string Response::payload(void) const
	{
		return body;
	}
} // namespace sews::architecture::message::http::transport
