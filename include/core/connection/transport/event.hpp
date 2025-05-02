#ifndef SEWS_CORE_CONNECTION_EVENT_HPP
#define SEWS_CORE_CONNECTION_EVENT_HPP

#include "core/connection/event/channel.hpp"
#include "core/connection/transport/channel.hpp"

namespace sews::core::connection::transport
{
	/*
	 * Module: SocketEvent (data structure).
	 *
	 * Purpose: Represents a single I/O event associated with a Channel.
	 *
	 * Ownership:
	 *
	 * - Does not manage any resources.
	 *
	 * - Holds non-owning reference to an external Channel.
	 *
	 * Notes:
	 *
	 * - This is a simple value type (POD-like).
	 *
	 * - No thread-safety guarantees: dispatcher must handle concurrency if needed.
	 */
	struct Event
	{
		event::Channel flag;
		Channel *channel;
	};
} // namespace sews::core::connection::transport

#endif // !SEWS_CORE_CONNECTION_EVENT_HPP
