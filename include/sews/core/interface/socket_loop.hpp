#ifndef SEWS_CORE_INTERFACE_SOCKET_LOOP_HPP
#define SEWS_CORE_INTERFACE_SOCKET_LOOP_HPP

#include "sews/core/interface/channel.hpp"
#include "sews/core/model/socket_event.hpp"
#include <vector>

namespace sews::interface
{
	/*
	 * Module: SockerLoop (interface).
	 *
	 * Purpose: Defines the contract for application-level message transportation.
	 *
	 * Ownership: Does not manage external resources; concrete implementations own their internal values (e.g., payload
	 * strings).
	 *
	 * Thread Safety: Not thread-safe; access must be restricted to the dispatcher thread.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations may or may not be thread-safe.
	 *
	 * - Dispatcher should guarantee single-threaded access unless otherwise documented.
	 *
	 * - `payload(void)` returns a serialized or raw textual representation of the message body.
	 *
	 * - Implementations must ensure that `type(void)` and `payload(void)` reflect a consistent view of the message.
	 */
	struct SocketLoop
	{
		virtual ~SocketLoop(void);
		virtual void registerChannel(Channel &channel) = 0;
		virtual void unregisterChannel(Channel &channel) = 0;
		virtual void poll(const std::vector<Channel *> &watched, std::vector<model::SocketEvent> &outEvents) = 0;
		virtual void updateEvents(Channel &channel, std::initializer_list<enums::SocketEvent> events) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_SOCKET_LOOP_HPP
