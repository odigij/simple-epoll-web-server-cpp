#ifndef SEWS_CORE_CONNECTION_EVENT_HPP
#define SEWS_CORE_CONNECTION_EVENT_HPP

#include "core/connection/event/channel.hpp"
#include "core/connection/transport/channel.hpp"

namespace sews::core::connection::transport
{
	/*
	 * Purpose: Represents a single I/O event associated with a Channel.
	 *
	 * Ownership: Does not responsible to manage any resources.
	 */
	struct Event
	{
		event::Channel flag;
		Channel *channel;
	};
} // namespace sews::core::connection::transport

#endif // !SEWS_CORE_CONNECTION_EVENT_HPP
