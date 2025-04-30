#ifndef SEWS_CORE_CONNECTION_REACTOR_LOOP_HPP
#define SEWS_CORE_CONNECTION_REACTOR_LOOP_HPP

#include "core/connection/transport/channel.hpp"
#include "core/connection/transport/event.hpp"
#include <vector>

namespace sews::core::connection::reactor
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
		virtual ~SocketLoop(void) = default;
		virtual void registerChannel(transport::Channel &channel) = 0;
		virtual void unregisterChannel(transport::Channel &channel) = 0;
		virtual void poll(const std::vector<transport::Channel *> &watched,
						  std::vector<transport::SocketEvent> &outEvents) = 0;
		virtual void updateEvents(transport::Channel &channel, std::initializer_list<connection::Events> events) = 0;
		virtual size_t getEventCapacity() const = 0;
	};
} // namespace sews::core::connection::reactor

#endif // !SEWS_CORE_CONNECTION_REACTOR_LOOP_HPP
