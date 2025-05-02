#ifndef SEWS_CORE_CONNECTION_REACTOR_ACCEPTOR_HPP
#define SEWS_CORE_CONNECTION_REACTOR_ACCEPTOR_HPP

#include "core/connection/transport/channel.hpp"

#include <memory>

namespace sews::core::connection::reactor
{
	/*
	 * Purpose: Defines the contract for accepting incoming connection requests.
	 *
	 * Ownership: Owns a listener channel.
	 */
	struct Acceptor
	{
		virtual ~Acceptor(void) = default;
		// Returns a reference to the listener interface::Channel currently used for accepting connections.
		virtual std::unique_ptr<transport::Channel> accept(void) = 0;
		// Accepts a new connection request and returns its associated interface::Channel.
		virtual transport::Channel &getListener(void) = 0;
	};
} // namespace sews::core::connection::reactor

#endif // !SEWS_CORE_CONNECTION_REACTOR_ACCEPTOR_HPP
