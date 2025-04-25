#include "sews/infrastructure/format/http/response.hpp"

namespace sews::format::http
{
	enums::MessageType Response::type(void) const
	{
		return enums::MessageType::HttpResponse;
	}

	std::string Response::payload(void) const
	{
		return body;
	}
} // namespace sews::format::http
