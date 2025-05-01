#include "architecture/connection/manager.hpp"

namespace sews::architecture::connection
{
	Manager::Manager(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger) : logger(logger)
	{
		logger->log(core::telemetry::diagnostic::LogType::INFO, "\033[36mConnection Manager:\033[0m Initialized.");
	}

	Manager::~Manager(void)
	{
		logger->log(core::telemetry::diagnostic::LogType::INFO, "\033[36mConnection Manager:\033[0m Terminated.");
	}

	void Manager::add(std::unique_ptr<core::connection::transport::Channel> channel)
	{
		channels[channel->getFd()] = std::move(channel);
	}

	void Manager::remove(core::connection::transport::Channel &channel)
	{
		closedChannels.push_back(channel.getFd());
	}

	void Manager::clear(void)
	{
		for (int fd : closedChannels)
		{
			auto it = channels.find(fd);
			if (it != channels.end())
			{
				it->second->close();
				channels.erase(it);
			}
		}
		closedChannels.clear();
	}

	core::connection::transport::Channel *Manager::find(int fd) const
	{
		auto it = channels.find(fd);
		if (it == channels.end())
		{
			return nullptr;
		}
		return it->second.get();
	}

	void Manager::forEach(std::function<void(core::connection::transport::Channel &)> callback) const
	{
		for (auto &[fd, channelPtr] : channels)
		{
			if (channelPtr)
			{
				callback(*channelPtr);
			}
		}
	}

	void Manager::forEachClosed(std::function<void(int &)> callback) const
	{
		for (int fd : closedChannels)
		{
			callback(fd);
		}
	}

	size_t Manager::count(void) const
	{
		return channels.size();
	}
} // namespace sews::architecture::connection
