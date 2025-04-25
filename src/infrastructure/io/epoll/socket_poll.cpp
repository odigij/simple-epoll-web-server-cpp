#include "sews/infrastructure/io/epoll/socket_poll.hpp"
#include "sews/core/enums/socket_event.hpp"
#include <algorithm>
#include <cstdio>
#include <sys/epoll.h>

namespace sews::io::epoll
{
	SocketLoop::SocketLoop() : epollFd(epoll_create1(0))
	{
		if (epollFd < 0)
		{
			perror("epoll_create1");
			exit(EXIT_FAILURE);
		}
		epoll_events.resize(64);
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
		epoll_ctl(epollFd, EPOLL_CTL_ADD, channel.getFd(), &epollEvent);
		// TODO: Implement "epoll_ctl" error handling.
	}

	void SocketLoop::unregisterChannel(interface::Channel &channel)
	{
		const int clientFd{channel.getFd()};
		epoll_ctl(epollFd, EPOLL_CTL_DEL, channel.getFd(), nullptr);
	}

	void SocketLoop::poll(const std::vector<interface::Channel *> &watched,
						  std::vector<interface::SocketEvent> &outEvents)
	{
		const int readyEventCount{epoll_wait(epollFd, epoll_events.data(), epoll_events.size(), 300)};
		if (readyEventCount < 0)
		{
			// TODO: Handle epoll_wait error.
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
		epoll_event ev{
			.events = flags,
			.data =
				{
					.fd = channel.getFd(),
				},
		};
		if (epoll_ctl(epollFd, EPOLL_CTL_MOD, channel.getFd(), &ev) == -1)
		{
			// TODO: Handle error & log it.
			perror("epoll_ctl: mod");
			// optional: throw or log error
		}
	}
} // namespace sews::io::epoll
