#include "infrastructure/connection/reactor/epoll/loop.hpp"
#include "core/telemetry/diagnostic/logger/backend/logger.hpp"
#include "infrastructure/control/stop_signal.hpp"

#include <algorithm>
#include <cstring>
#include <sstream>

namespace sews::infrastructure::connection::reactor::epoll
{
	SocketLoop::SocketLoop(size_t size, std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger)
		: epollFd(epoll_create1(0)), logger(logger)
	{
		if (epollFd < 0)
		{
			std::ostringstream oss;
			oss << "\033[36mEpoll Socket Loop:\033[0m Failed to initialize epoll fd,  \033[31merrno = " << errno
				<< " -> " << strerror(errno);
			logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR, oss.str());
			perror("epoll_create1");
			exit(EXIT_FAILURE);
		}
		epoll_events.resize(size);

		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
					"\033[36mEpoll Socket Loop:\033[0m Initialized.");
	}

	SocketLoop::~SocketLoop(void)
	{
		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
					"\033[36mEpoll Socket Loop:\033[0m Terminated.");
	}

	void SocketLoop::registerChannel(core::connection::transport::Channel &channel)
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
			std::ostringstream oss;
			oss << "\033[36mEpoll Socket Loop:\033[0m Failed to register fd, \033[31merrno = " << errno << " -> "
				<< strerror(errno);
			logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR, oss.str());
		}
	}

	void SocketLoop::unregisterChannel(int fd)
	{
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr))
		{
			std::ostringstream oss;
			oss << "\033[36mEpoll Socket Loop:\033[0m Failed to delete fd, \033[31merrno -> " << errno << " -> "
				<< strerror(errno);
			logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR, oss.str());
		}
	}

	void SocketLoop::poll(const std::vector<core::connection::transport::Channel *> &watched,
						  std::vector<core::connection::transport::Event> &outEvents)
	{
		outEvents.clear();
		const int readyEventCount{epoll_wait(epollFd, epoll_events.data(), epoll_events.size(), 300)};
		if (readyEventCount < 0)
		{
			if (infrastructure::control::stopFlag.load())
			{
				std::ostringstream oss;
				oss << "\033[36mEpoll Socket Loop:\033[0m Terminated due to \033[31merrno -> " << errno << ", "
					<< strerror(errno);
				logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING, oss.str());
			}
			else
			{
				logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR,
							std::string("\033[36mEpoll Socket Loop:\033[0m Failed to wait due to\033[31merrno= " +
										std::to_string(errno)) +
								", " + strerror(errno));
			}
		}
		for (int i{0}; i < readyEventCount; ++i)
		{
			const epoll_event &readyEvent{epoll_events[i]};
			auto it = std::find_if(watched.begin(), watched.end(),
								   [fd = readyEvent.data.fd](core::connection::transport::Channel *ch) {
									   return ch && ch->getFd() == fd;
								   });
			if (it != watched.end())
			{
				core::connection::transport::Channel *channel{*it};
				if (readyEvent.events & EPOLLERR)
				{
					outEvents.push_back({
						core::connection::event::Channel::ERROR,
						channel,
					});
				}
				if (readyEvent.events & EPOLLHUP)
				{
					outEvents.push_back({
						core::connection::event::Channel::HANGUP,
						channel,
					});
				}
				if (readyEvent.events & EPOLLIN)
				{
					outEvents.push_back({
						core::connection::event::Channel::READ,
						channel,
					});
				}
				if (readyEvent.events & EPOLLOUT)
				{
					outEvents.push_back({
						core::connection::event::Channel::WRITE,
						channel,
					});
				}
			}
		}
	}

	void SocketLoop::updateEvents(core::connection::transport::Channel &channel,
								  std::initializer_list<core::connection::event::Channel> events)
	{
		uint32_t flags{0};
		for (auto e : events)
		{
			switch (e)
			{
				case core::connection::event::Channel::READ:
					flags |= EPOLLIN;
					break;

				case core::connection::event::Channel::WRITE:
					flags |= EPOLLOUT;
					break;

				case core::connection::event::Channel::HANGUP:
					flags |= EPOLLHUP;
					break;

				case core::connection::event::Channel::ERROR:
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
			logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR,
						std::string("\033[36mEpoll Socket Loop:\033[0m Failed to wait, \033[31merrno= " +
									std::to_string(errno) + ", ") +
							strerror(errno));
			// NOTE: May need to throw an exception here.
		}
	}

	size_t SocketLoop::getEventCapacity(void) const
	{
		return epoll_events.capacity();
	}
} // namespace sews::infrastructure::connection::reactor::epoll
