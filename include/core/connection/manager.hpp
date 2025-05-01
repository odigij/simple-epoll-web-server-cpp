#ifndef SEWS_CORE_CONNECTION_MANAGER_HPP
#define SEWS_CORE_CONNECTION_MANAGER_HPP

#include "core/connection/transport/channel.hpp"
#include <functional>
#include <memory>

namespace sews::core::connection
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
		virtual ~ConnectionManager(void) = default;
		// Stores Channel to inner container.
		virtual void add(std::unique_ptr<transport::Channel> channel) = 0;
		// Marks a Channel for future closure and removal.
		virtual void remove(transport::Channel &channel) = 0;
		// Closes and removes all channels that were marked for closure.
		virtual void clear(void) = 0;
		// Returns the active Channel associated with the given file descriptor, or nullptr if not found.
		virtual transport::Channel *find(int fd) const = 0;
		// Traverses all active Channels.
		virtual void forEach(std::function<void(transport::Channel &)> callback) const = 0;
		// Traverses all Channels marked for closure.
		virtual void forEachClosed(std::function<void(int &)> callback) const = 0;
		// Returns the number of currently active Channels.
		virtual size_t count(void) const = 0;
	};
} // namespace sews::core::connection

#endif // !SEWS_CORE_CONNECTION_MANAGER_HPP
