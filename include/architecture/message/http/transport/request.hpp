#ifndef SEWS_ARCHITECTURE_MESSAGE_HTTP_TRANSPORT_REQUEST_HPP
#define SEWS_ARCHITECTURE_MESSAGE_HTTP_TRANSPORT_REQUEST_HPP

#include "core/message/transport/message.hpp"
#include <unordered_map>

namespace sews::architecture::message::http::transport
{
	struct Request : public core::message::transport::Message
	{
		Request(void);
		Request(std::string_view method, std::string_view path, std::string_view version,
				std::unordered_map<std::string, std::string> headers, std::string_view body);
		~Request(void) override;

		std::string method, path, version, body;
		std::unordered_map<std::string, std::string> headers;

		core::message::MessageType type() const override;
		std::string payload() const override;
	};
} // namespace sews::architecture::message::http::transport

#endif // !SEWS_ARCHITECTURE_MESSAGE_HTTP_TRANSPORT_REQUEST_HPP
