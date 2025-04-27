#include "sews/infrastructure/connection/manager.hpp"

namespace sews::connection
{
	Manager::Manager(interface::Logger *logger) : logger(logger)
	{
		logger->log(enums::LogType::INFO, "\033[36mConnection Manager:\033[0m Initialized.");
	}

	Manager::~Manager(void)
	{
		logger->log(enums::LogType::INFO, "\033[36mConnection Manager:\033[0m Terminated.");
	}

	void Manager::add(std::unique_ptr<interface::Channel> channel)
	{
		channels[channel->getFd()] = std::move(channel);
	}

	void Manager::remove(interface::Channel &channel)
	{
		closedChannels.push_back(&channel);
	}

	void Manager::clear(void)
	{
		for (interface::Channel *channel : closedChannels)
		{
			auto it = channels.find(channel->getFd());
			if (it != channels.end())
			{
				channels.erase(it);
			}
		}
		closedChannels.clear();
	}

	interface::Channel *Manager::find(int fd) const
	{
		auto it = channels.find(fd);
		if (it == channels.end())
		{
			return nullptr;
		}
		return it->second.get();
	}

	void Manager::forEach(std::function<void(interface::Channel &)> callback) const
	{
		for (auto &[fd, channelPtr] : channels)
		{
			if (channelPtr)
			{
				callback(*channelPtr);
			}
		}
	}

	void Manager::forEachClosed(std::function<void(interface::Channel &)> callback) const
	{
		for (interface::Channel *channel : closedChannels)
		{
			callback(*channel);
		}
	}

	size_t Manager::count(void) const
	{
		return channels.size();
	}
} // namespace sews::connection
