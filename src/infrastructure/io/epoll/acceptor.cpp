#include "sews/infrastructure/io/epoll/acceptor.hpp"
#include "sews/core/enums/log_type.hpp"
#include "sews/infrastructure/transport/plain_text_channel.hpp"
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

namespace sews::io::epoll
{
	Acceptor::Acceptor(int fd, interface::Logger *logger)
		: serverChannel(std::make_unique<transport::PlainTextChannel>(fd)), logger(logger)
	{
		logger->log(enums::LogType::INFO, "\033[36mEpoll Acceptor:\033[0m Initialized.");
	}

	Acceptor::~Acceptor(void)
	{
		logger->log(enums::LogType::INFO, "\033[36mEpoll Acceptor:\033[0m Terminated.");
	}

	std::unique_ptr<interface::Channel> Acceptor::accept(void)
	{
		sockaddr_in clientAddr{};
		socklen_t clientLen{sizeof(clientAddr)};
		int clientFd{::accept(serverChannel->getFd(), reinterpret_cast<sockaddr *>(&clientAddr), &clientLen)};
		if (clientFd < 0)
		{
			logger->log(enums::LogType::ERROR, "\033[36mEpoll Acceptor:\033[0m Failed to accept.");
			return nullptr;
		}
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), ip, INET_ADDRSTRLEN);
		uint16_t port = ntohs(clientAddr.sin_port);

		logger->log(enums::LogType::INFO, "\033[36mEpoll Acceptor:\033[0m Accepted connection from \033[33m" +
											  std::string(ip) + ":" + std::to_string(port));

		return std::make_unique<sews::transport::PlainTextChannel>(clientFd);
	}

	interface::Channel &Acceptor::channel(void)
	{
		return *serverChannel;
	}
} // namespace sews::io::epoll
