#ifndef SEWS_INFRASTRUCTURE_IO_EPOLL_ACCEPTOR_HPP
#define SEWS_INFRASTRUCTURE_IO_EPOLL_ACCEPTOR_HPP

#include "sews/core/interface/acceptor.hpp"
#include "sews/core/interface/channel.hpp"
#include "sews/core/interface/logger.hpp"
#include <memory>

namespace sews::io::epoll
{
	struct Acceptor : public interface::Acceptor
	{
		explicit Acceptor(int fd, std::shared_ptr<interface::Logger> logger);
		~Acceptor(void) override;
		std::unique_ptr<interface::Channel> accept(void) override;
		interface::Channel &getListener(void) override;

	  private:
		std::unique_ptr<interface::Channel> serverChannel;
		std::shared_ptr<interface::Logger> logger;
	};
} // namespace sews::io::epoll

#endif // !SEWS_INFRASTRUCTURE_IO_EPOLL_ACCEPTOR_HPP
