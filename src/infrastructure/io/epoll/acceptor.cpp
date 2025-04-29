#include "sews/infrastructure/io/epoll/acceptor.hpp"
#include "sews/core/enums/log_type.hpp"
#include "sews/infrastructure/transport/plain_text_channel.hpp"
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>

namespace sews::io::epoll
{
	Acceptor::Acceptor(int fd, std::shared_ptr<interface::Logger> logger)
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
		uint16_t port{ntohs(clientAddr.sin_port)};
		std::ostringstream os;
		os << "\033[36mEpoll Acceptor:\033[0m Accepted connection from;\033[33m " << ' ' << ip << ':' << port
		   << "\033[0m, fd = \033[33m" << clientFd;
		logger->log(enums::LogType::INFO, os.str());

		std::unique_ptr channel{std::make_unique<sews::transport::PlainTextChannel>(clientFd)};
		channel->setDetails(port, ip);
		return std::move(channel);
	}

	interface::Channel &Acceptor::getListener(void)
	{
		return *serverChannel;
	}
} // namespace sews::io::epoll
