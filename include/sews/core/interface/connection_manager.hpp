#ifndef SEWS_CORE_INTERFACE_CONNECTION_MANAGER_HPP
#define SEWS_CORE_INTERFACE_CONNECTION_MANAGER_HPP

#include "sews/core/interface/channel.hpp"
#include <functional>
#include <memory>

namespace sews::interface
{
	/*
	 * Module: ConnectionManager (interface).
	 *
	 * Purpose: Defines the contract for managing the lifecycle of active Channels.
	 *
	 * Ownership: Fully responsible for closing and removing associated Channels.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations may or may not be thread-safe.
	 *
	 * - Dispatcher should guarantee single-threaded access unless otherwise documented.
	 */
	struct ConnectionManager
	{
		virtual ~ConnectionManager(void);
		// Stores Channel to inner container.
		virtual void add(std::unique_ptr<Channel> channel) = 0;
		// Marks a Channel for future closure and removal.
		virtual void remove(Channel &channel) = 0;
		// Closes and removes all channels that were marked for closure.
		virtual void clear(void) = 0;
		// Returns the active Channel associated with the given file descriptor, or nullptr if not found.
		virtual interface::Channel *find(int fd) const = 0;
		// Traverses all active Channels.
		virtual void forEach(std::function<void(Channel &)> callback) const = 0;
		// Traverses all Channels marked for closure.
		virtual void forEachClosed(std::function<void(Channel &)> callback) const = 0;
		// Returns the number of currently active Channels.
		virtual size_t count(void) const = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_CONNECTION_MANAGER_HPP
