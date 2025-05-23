#ifndef SEWS_CORE_MESSAGE_TRANSPORT_MESSAGE_HPP
#define SEWS_CORE_MESSAGE_TRANSPORT_MESSAGE_HPP

#include "core/message/type/message.hpp"

#include <string>

namespace sews::core::message::transport
{
	/*
	 * Purpose: Defines the contract for application-level message transportation.
	 *
	 * Ownership: Does not manage external resources; concrete implementations own their internal values (e.g., payload
	 * strings).
	 *
	 * Notes:
	 *
	 * - `payload(void)` returns a serialized or raw textual representation of the message body.
	 *
	 * - Implementations must ensure that `type(void)` and `payload(void)` reflect a consistent view of the message.
	 */
	struct Message
	{
		virtual ~Message(void) = default;
		// Returns the format of the payload.
		virtual core::message::type::Message type(void) const = 0;
		// Returns raw payload.
		virtual std::string payload(void) const = 0;
	};
} // namespace sews::core::message::transport

#endif // !SEWS_CORE_MESSAGE_TRANSPORT_MESSAGE_HPP
