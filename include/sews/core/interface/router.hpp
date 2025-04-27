#ifndef SEWS_CORE_INTERFACE_ROUTER_HPP
#define SEWS_CORE_INTERFACE_ROUTER_HPP

#include "sews/core/interface/message_handler.hpp"

namespace sews::interface
{
	/*
	 * Module: Router (interface).
	 *
	 * Purpose: Defines the contract for managing and path based access of handlers.
	 *
	 * Ownership: Fully responsible for managing the lifecycle of path-associated handler nodes.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations may or may not be thread-safe.
	 *
	 * - Dispatcher should guarantee single-threaded access unless otherwise documented.
	 */
	struct Router
	{
		virtual ~Router(void);
		virtual void add(std::string_view method, std::string_view path, MessageHandler *handler) = 0;
		virtual void remove(std::string_view method, std::string_view path) = 0;
		// Finds and returns the handler associated with method and path.
		virtual MessageHandler *match(const Message &message) const = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_ROUTER_HPP
