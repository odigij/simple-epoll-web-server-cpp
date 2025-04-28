#include "sews/infrastructure/io/epoll/socket_poll.hpp"
#include "sews/core/stop_flag.hpp"
#include <algorithm>
#include <string>
#include <cstring>

namespace sews::io::epoll
{
	SocketLoop::SocketLoop(size_t size, std::shared_ptr<interface::Logger> logger)
		: epollFd(epoll_create1(0)), logger(logger)
	{
		if (epollFd < 0)
		{
			logger->log(enums::LogType::ERROR,
						std::string("\033[36mEpoll Socket Loop:\033[0m Failed to initialize epoll fd, \033[31merrno= " +
									std::to_string(errno) + ", ") +
							strerror(errno));
			perror("epoll_create1");
			exit(EXIT_FAILURE);
		}
		epoll_events.reserve(size);

		logger->log(enums::LogType::INFO, "\033[36mEpoll Socket Loop:\033[0m Initialized.");
	}

	SocketLoop::~SocketLoop(void)
	{
		logger->log(enums::LogType::INFO, "\033[36mEpoll Socket Loop:\033[0m Terminated.");
	}

	void SocketLoop::registerChannel(interface::Channel &channel)
	{
		const int clientFd{channel.getFd()};
		epoll_event epollEvent{
			.events = EPOLLIN,
			.data =
				{
					.fd = clientFd,
				},
		};

		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, channel.getFd(), &epollEvent) != 0)
		{
			logger->log(enums::LogType::ERROR,
						std::string("\033[36mEpoll Socket Loop:\033[0m Failed to register fd, \033[31merrno= " +
									std::to_string(errno) + ", ") +
							strerror(errno));
		}
	}

	void SocketLoop::unregisterChannel(interface::Channel &channel)
	{
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, channel.getFd(), nullptr))
		{
			logger->log(enums::LogType::ERROR,
						std::string("\033[36mEpoll Socket Loop:\033[0m Failed to delete fd, \033[31merrno= " +
									std::to_string(errno) + ", ") +
							strerror(errno));
		}
	}

	void SocketLoop::poll(const std::vector<interface::Channel *> &watched, std::vector<model::SocketEvent> &outEvents)
	{
		const int readyEventCount{epoll_wait(epollFd, epoll_events.data(), epoll_events.capacity(), 300)};
		if (readyEventCount < 0)
		{
			if (stopFlag.load())
			{
				logger->log(enums::LogType::WARNING,
							std::string("\033[36mEpoll Socket Loop:\033[0m Terminated due to errno= " +
										std::to_string(errno) + ", ") +
								strerror(errno));
			}
			else
			{
				logger->log(enums::LogType::ERROR,
							std::string("\033[36mEpoll Socket Loop:\033[0m Failed to wait due to\033[31merrno= " +
										std::to_string(errno)) +
								", " + strerror(errno));
			}
		}
		for (int i{0}; i < readyEventCount; ++i)
		{
			const epoll_event &readyEvent{epoll_events[i]};
			auto it = std::find_if(watched.begin(), watched.end(), [fd = readyEvent.data.fd](interface::Channel *ch) {
				return ch && ch->getFd() == fd;
			});
			if (it != watched.end())
			{
				interface::Channel *channel{*it};
				if (readyEvent.events & EPOLLERR)
				{
					outEvents.push_back({
						enums::SocketEvent::ERROR,
						*channel,
					});
				}
				if (readyEvent.events & EPOLLHUP)
				{
					outEvents.push_back({
						enums::SocketEvent::HANGUP,
						*channel,
					});
				}
				if (readyEvent.events & EPOLLIN)
				{
					outEvents.push_back({
						enums::SocketEvent::READ,
						*channel,
					});
				}
				if (readyEvent.events & EPOLLOUT)
				{
					outEvents.push_back({
						enums::SocketEvent::WRITE,
						*channel,
					});
				}
			}
		}
	}

	void SocketLoop::updateEvents(interface::Channel &channel, std::initializer_list<enums::SocketEvent> events)
	{
		uint32_t flags{0};
		for (auto e : events)
		{
			switch (e)
			{
				case enums::SocketEvent::READ:
					flags |= EPOLLIN;
					break;

				case enums::SocketEvent::WRITE:
					flags |= EPOLLOUT;
					break;

				case enums::SocketEvent::HANGUP:
					flags |= EPOLLHUP;
					break;

				case enums::SocketEvent::ERROR:
					flags |= EPOLLERR;
					break;
			}
		}
		epoll_event epollEvent{
			.events = flags,
			.data =
				{
					.fd = channel.getFd(),
				},
		};
		if (epoll_ctl(epollFd, EPOLL_CTL_MOD, channel.getFd(), &epollEvent) == -1)
		{
			perror("epoll_ctl: mod");
			logger->log(enums::LogType::ERROR,
						std::string("\033[36mEpoll Socket Loop:\033[0m Failed to wait, \033[31merrno= " +
									std::to_string(errno) + ", ") +
							strerror(errno));
			// NOTE: May need to throw an exception here.
		}
	}

	size_t SocketLoop::getEventCapacity() const
	{
		return epoll_events.capacity();
	}
} // namespace sews::io::epoll
