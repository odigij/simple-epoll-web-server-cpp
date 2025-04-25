#ifndef SEWS_INFRASTRUCTURE_IO_EPOLL_SOCKET_POLL_HPP
#define SEWS_INFRASTRUCTURE_IO_EPOLL_SOCKET_POLL_HPP

#include "sews/core/interface/channel.hpp"
#include "sews/core/interface/socket_loop.hpp"
#include <sys/epoll.h>
#include <vector>

namespace sews::io::epoll
{
	struct SocketLoop : public interface::SocketLoop
	{
		SocketLoop(void);
		void registerChannel(interface::Channel &channel) override;
		void unregisterChannel(interface::Channel &channel) override;
		void poll(const std::vector<interface::Channel *> &watched,
				  std::vector<interface::SocketEvent> &outEvents) override;
		void updateEvents(interface::Channel &channel, std::initializer_list<enums::SocketEvent> events) override;

	  private:
		int epollFd{-1};
		std::vector<epoll_event> epoll_events;
	};
} // namespace sews::io::epoll

#endif // !SEWS_INFRASTRUCTURE_IO_EPOLL_SOCKET_POLL_HPP
