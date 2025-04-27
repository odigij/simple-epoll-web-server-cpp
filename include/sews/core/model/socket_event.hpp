#ifndef SEWS_CORE_MODEL_SOCKET_EVENT_HPP
#define SEWS_CORE_MODEL_SOCKET_EVENT_HPP

#include "sews/core/enums/socket_event.hpp"
#include "sews/core/interface/channel.hpp"

namespace sews::model
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
	struct SocketEvent
	{
		enums::SocketEvent flag;
		interface::Channel &channel;
	};
} // namespace sews::model

#endif // !SEWS_CORE_MODEL_SOCKET_EVENT_HPP
