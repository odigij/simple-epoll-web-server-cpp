#ifndef SEWS_INFRASTRUCTURE_FORMAT_HTPP_RESPONSE_HPP
#define SEWS_INFRASTRUCTURE_FORMAT_HTPP_RESPONSE_HPP

#include "sews/core/interface/message.hpp"
#include <string>
#include <sys/types.h>
#include <unordered_map>

namespace sews::format::http
{
	struct Response : public sews::interface::Message
	{
		int status{200};
		std::string statusText{"OK"}, version{"HTTP/1.1"}, body{""};
		std::unordered_map<std::string, std::string> headers;

		enums::MessageType type(void) const override;
		std::string payload(void) const override;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_FORMAT_HTPP_RESPONSE_HPP
