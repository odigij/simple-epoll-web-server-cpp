#ifndef SEWS_INFRASTRUCTURE_FORMAT_HTTP_REQUEST_HPP
#define SEWS_INFRASTRUCTURE_FORMAT_HTTP_REQUEST_HPP

#include "sews/core/interface/message.hpp"
#include <unordered_map>

namespace sews::format::http
{
	struct Request : public sews::interface::Message
	{
		Request(void);
		Request(std::string_view method, std::string_view path, std::string_view version,
				std::unordered_map<std::string, std::string> headers, std::string_view body);
		~Request(void) override;

		std::string method, path, version, body;
		std::unordered_map<std::string, std::string> headers;

		enums::MessageType type() const override;
		std::string payload() const override;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_FORMAT_HTTP_REQUEST_HPP
