#ifndef SEWS_INFRASTRUCTURE_IO_EPOLL_SOCKET_POLL_HPP
#define SEWS_INFRASTRUCTURE_IO_EPOLL_SOCKET_POLL_HPP

#include "sews/core/interface/channel.hpp"
#include "sews/core/interface/logger.hpp"
#include "sews/core/interface/socket_loop.hpp"
#include <memory>
#include <sys/epoll.h>

namespace sews::io::epoll
{
	struct SocketLoop : public interface::SocketLoop
	{
		SocketLoop(size_t size, std::shared_ptr<interface::Logger> logger);
		~SocketLoop(void) override;
		void registerChannel(interface::Channel &channel) override;
		void unregisterChannel(interface::Channel &channel) override;
		void poll(const std::vector<interface::Channel *> &watched,
				  std::vector<model::SocketEvent> &outEvents) override;
		void updateEvents(interface::Channel &channel, std::initializer_list<enums::SocketEvent> events) override;
		size_t getEventCapacity() const;

	  private:
		int epollFd{-1};
		std::vector<epoll_event> epoll_events;
		std::shared_ptr<interface::Logger> logger;
	};
} // namespace sews::io::epoll

#endif // !SEWS_INFRASTRUCTURE_IO_EPOLL_SOCKET_POLL_HPP
