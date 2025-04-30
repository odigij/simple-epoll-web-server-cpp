#ifndef SEWS_ARCHITECTURE_CONNECTION_MANAGER_HPP
#define SEWS_ARCHITECTURE_CONNECTION_MANAGER_HPP

#include "core/connection/manager.hpp"
#include "core/connection/transport/channel.hpp"
#include "core/telemetry/diagnostic/transport/logger.hpp"
#include <vector>

namespace sews::architecture::connection
{
	struct Manager : public core::connection::ConnectionManager
	{
		Manager(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger);
		~Manager(void) override;
		void add(std::unique_ptr<core::connection::transport::Channel> channel) override;
		void remove(core::connection::transport::Channel &channel) override;
		void clear(void) override;
		core::connection::transport::Channel *find(int fd) const override;
		void forEach(std::function<void(core::connection::transport::Channel &)> callback) const override;
		void forEachClosed(std::function<void(core::connection::transport::Channel &)> callback) const override;
		size_t count(void) const override;

	  private:
		std::unordered_map<int, std::unique_ptr<core::connection::transport::Channel>> channels;
		std::vector<core::connection::transport::Channel *> closedChannels;
		std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger;
	};
} // namespace sews::architecture::connection

#endif // !SEWS_ARCHITECTURE_CONNECTION_MANAGER_HPP
