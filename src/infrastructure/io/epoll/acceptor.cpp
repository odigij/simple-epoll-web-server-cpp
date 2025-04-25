#include "sews/infrastructure/io/epoll/acceptor.hpp"
#include "sews/infrastructure/transport/plain_text_channel.hpp"
#include <netinet/in.h>
#include <unistd.h>

namespace sews::io::epoll
{
	Acceptor::Acceptor(int fd) : serverChannel(std::make_unique<transport::PlainTextChannel>(fd))
	{
	}

	Acceptor::~Acceptor(void)
	{
	}

	std::unique_ptr<interface::Channel> Acceptor::accept(void)
	{
		sockaddr_in clientAddr{};
		socklen_t clientLen{sizeof(clientAddr)};
		int clientFd{::accept(serverChannel->getFd(), reinterpret_cast<sockaddr *>(&clientAddr), &clientLen)};
		if (clientFd < 0)
		{
			return nullptr;
		}
		return std::make_unique<sews::transport::PlainTextChannel>(clientFd);
	}

	interface::Channel &Acceptor::channel(void)
	{
		return *serverChannel;
	}
} // namespace sews::io::epoll
