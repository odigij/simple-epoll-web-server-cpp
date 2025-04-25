#ifndef SEWS_CORE_INTERFACE_SOCKET_LOOP_HPP
#define SEWS_CORE_INTERFACE_SOCKET_LOOP_HPP

#include "sews/core/interface/channel.hpp"
#include "sews/core/interface/socket_event.hpp"
#include <vector>

namespace sews::interface
{
	struct SocketLoop
	{
		virtual ~SocketLoop(void);
		virtual void registerChannel(Channel &channel) = 0;
		virtual void unregisterChannel(Channel &channel) = 0;
		virtual void poll(const std::vector<Channel *> &watched, std::vector<SocketEvent> &outEvents) = 0;
		virtual void updateEvents(Channel &channel, std::initializer_list<enums::SocketEvent> events) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_SOCKET_LOOP_HPP
