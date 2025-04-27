#ifndef SEWS_CORE_INTERFACE_CONNECTION_MANAGER_HPP
#define SEWS_CORE_INTERFACE_CONNECTION_MANAGER_HPP

#include "sews/core/interface/channel.hpp"
#include <functional>
#include <memory>

namespace sews::interface
{
	struct ConnectionManager
	{
		virtual ~ConnectionManager(void);
		virtual void add(std::unique_ptr<Channel> channel) = 0;
		virtual void remove(Channel &channel) = 0;
		virtual void clear(void) = 0;
		virtual interface::Channel *find(int fd) const = 0;
		virtual void forEach(std::function<void(Channel &)> callback) const = 0;
		virtual void forEachClosed(std::function<void(Channel &)> callback) const = 0;
		virtual size_t count(void) const = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_CONNECTION_MANAGER_HPP
