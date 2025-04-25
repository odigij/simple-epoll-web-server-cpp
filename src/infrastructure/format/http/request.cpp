#include "sews/infrastructure/format/http/request.hpp"

namespace sews::format::http
{
	enums::MessageType Request::type(void) const
	{
		return enums::MessageType::HttpRequest;
	}

	std::string Request::payload(void) const
	{
		return body;
	}
} // namespace sews::format::http
