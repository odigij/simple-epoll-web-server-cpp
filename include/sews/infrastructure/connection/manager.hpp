#ifndef SEWS_CORE_INFRASTRUCTURE_CONNECTION_MANAGER_HPP
#define SEWS_CORE_INFRASTRUCTURE_CONNECTION_MANAGER_HPP

#include "sews/core/interface/connection_manager.hpp"
#include "sews/core/interface/logger.hpp"
#include <vector>

namespace sews::connection
{
	struct Manager : public interface::ConnectionManager
	{
		Manager(std::shared_ptr<interface::Logger> logger);
		~Manager(void) override;
		void add(std::unique_ptr<interface::Channel> channel) override;
		void remove(interface::Channel &channel) override;
		void clear(void) override;
		interface::Channel *find(int fd) const override;
		void forEach(std::function<void(interface::Channel &)> callback) const override;
		void forEachClosed(std::function<void(interface::Channel &)> callback) const override;
		size_t count(void) const override;

	  private:
		std::unordered_map<int, std::unique_ptr<interface::Channel>> channels;
		std::vector<interface::Channel *> closedChannels;
		std::shared_ptr<interface::Logger> logger;
	};
} // namespace sews::connection

#endif // !SEWS_CORE_INFRASTRUCTURE_CONNECTION_MANAGER_HPP
