#ifndef SEWS_CORE_INTERFACE_SOCKET_EVENT_HPP
#define SEWS_CORE_INTERFACE_SOCKET_EVENT_HPP

#include "sews/core/enums/socket_event.hpp"
#include "sews/core/interface/channel.hpp"

namespace sews::interface
{
	struct SocketEvent
	{
		enums::SocketEvent flag;
		Channel &channel;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_SOCKET_EVENT_HPP
