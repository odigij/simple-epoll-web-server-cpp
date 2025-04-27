#ifndef SEWS_CORE_INTERFACE_ACCEPTOR_HPP
#define SEWS_CORE_INTERFACE_ACCEPTOR_HPP

#include "sews/core/interface/channel.hpp"
#include <memory>

namespace sews::interface
{
	/*
	 * Module: Acceptor (interface).
	 *
	 * Purpose: Defines the contract for accepting incoming connection requests.
	 *
	 * Ownership: Owns the listener `interface::Channel` (socket) resource.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations may or may not be thread-safe.
	 *
	 * - Dispatcher should guarantee single-threaded access unless otherwise documented.
	 */
	struct Acceptor
	{
		virtual ~Acceptor(void);
		// Returns a reference to the listener interface::Channel currently used for accepting connections.
		virtual std::unique_ptr<Channel> accept(void) = 0;
		// Accepts a new connection request and returns its associated interface::Channel.
		virtual interface::Channel &getListener(void) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_ACCEPTOR_HPP
