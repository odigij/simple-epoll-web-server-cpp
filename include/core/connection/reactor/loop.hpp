#ifndef SEWS_CORE_CONNECTION_REACTOR_LOOP_HPP
#define SEWS_CORE_CONNECTION_REACTOR_LOOP_HPP

#include "core/connection/event/channel.hpp"
#include "core/connection/transport/event.hpp"

#include <vector>

namespace sews::core::connection::reactor
{
	/*
	 * Purpose: Defines the contract for application-level message transportation channel.
	 *
	 * Ownership: Does not manage external resources; concrete implementations own their internal values (e.g., payload
	 * strings). fd is assumed to refer to a channel previously registered; no ownership assumed.
	 */
	struct Loop
	{
		virtual ~Loop(void) = default;
		virtual void registerChannel(transport::Channel &channel) = 0;
		virtual void unregisterChannel(int fd) = 0;
		virtual void poll(const std::vector<transport::Channel *> &watched,
						  std::vector<transport::Event> &outEvents) = 0;
		virtual void updateEvents(transport::Channel &channel, std::initializer_list<event::Channel> events) = 0;
		virtual size_t getEventCapacity(void) const = 0;
	};
} // namespace sews::core::connection::reactor

#endif // !SEWS_CORE_CONNECTION_REACTOR_LOOP_HPP
