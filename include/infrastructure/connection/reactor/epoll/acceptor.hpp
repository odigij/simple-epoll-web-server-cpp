#ifndef SEWS_INFRASTRUCTURE_CONNECTION_REACTOR_EPOLL_ACCEPTOR_HPP
#define SEWS_INFRASTRUCTURE_CONNECTION_REACTOR_EPOLL_ACCEPTOR_HPP

#include "core/connection/reactor/acceptor.hpp"
#include "core/telemetry/diagnostic/logger/backend/logger.hpp"

#include <memory>

namespace sews::infrastructure::connection::reactor::epoll
{
	struct Acceptor : public core::connection::reactor::Acceptor
	{
		explicit Acceptor(int fd, std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger);
		~Acceptor(void) override;
		std::unique_ptr<core::connection::transport::Channel> accept(void) override;
		core::connection::transport::Channel &getListener(void) override;

	  private:
		std::unique_ptr<core::connection::transport::Channel> serverChannel;
		std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger;
	};
} // namespace sews::infrastructure::connection::reactor::epoll

#endif // !SEWS_INFRASTRUCTURE_CONNECTION_REACTOR_EPOLL_ACCEPTOR_HPP
