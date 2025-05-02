#ifndef SEWS_ARCHITECTURE_MESSAGE_HTPP_TRANSPORT_RESPONSE_HPP
#define SEWS_ARCHITECTURE_MESSAGE_HTPP_TRANSPORT_RESPONSE_HPP

#include "core/message/transport/message.hpp"

#include <unordered_map>

namespace sews::architecture::message::http::transport
{
	struct Response : public core::message::transport::Message
	{
		Response(void);
		Response(int status, std::string_view statusText, std::string_view version,
				 std::unordered_map<std::string, std::string> headers, std::string body);
		~Response(void) override;
		int status{200};
		std::string statusText{"OK"}, version{"HTTP/1.1"}, body{};
		std::unordered_map<std::string, std::string> headers;

		core::message::type::Message type(void) const override;
		std::string payload(void) const override;
	};
} // namespace sews::architecture::message::http::transport

#endif // !SEWS_ARCHITECTURE_MESSAGE_HTPP_TRANSPORT_RESPONSE_HPP
