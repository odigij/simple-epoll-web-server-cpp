#ifndef SEWS_INFRASTRUCTURE_CONNECTION_REACTOR_EPOLL_LOOP_HPP
#define SEWS_INFRASTRUCTURE_CONNECTION_REACTOR_EPOLL_LOOP_HPP

#include "core/connection/reactor/loop_info.hpp"
#include "core/connection/transport/channel.hpp"
#include "core/telemetry/diagnostic/transport/logger.hpp"
#include "core/connection/reactor/loop.hpp"
#include <memory>
#include <sys/epoll.h>

namespace sews::infrastructure::connection::reactor::epoll
{
	struct SocketLoop : public core::connection::reactor::SocketLoop, public core::connection::reactor::LoopInfo
	{
		SocketLoop(size_t size, std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger);
		~SocketLoop(void) override;
		void registerChannel(core::connection::transport::Channel &channel) override;
		void unregisterChannel(int fd) override;
		void poll(const std::vector<core::connection::transport::Channel *> &watched,
				  std::vector<core::connection::transport::SocketEvent> &outEvents) override;
		void updateEvents(core::connection::transport::Channel &channel,
						  std::initializer_list<core::connection::Events> events) override;
		size_t getEventCapacity() const override;

	  private:
		int epollFd{-1};
		std::vector<epoll_event> epoll_events;
		std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger;
	};
} // namespace sews::infrastructure::connection::reactor::epoll

#endif // !SEWS_INFRASTRUCTURE_CONNECTION_REACTOR_EPOLL_LOOP_HPP
