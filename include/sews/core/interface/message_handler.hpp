#ifndef SEWS_CORE_INTERFACE_MESSAGE_HANDLER_HPP
#define SEWS_CORE_INTERFACE_MESSAGE_HANDLER_HPP

#include "sews/core/interface/message.hpp"
#include <memory>

namespace sews::interface
{
	/*
	 * Module: MessageHandler (interface).
	 *
	 * Purpose: Defines the contract for handling `interface::Message` based inputs.
	 *
	 * Ownership: Does not manage external resources; concrete implementations own their internal values.
	 *
	 * Thread Safety: `Not thread-safe`; access must be restricted to the dispatcher thread.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations may or may not be thread-safe.
	 *
	 * - Dispatcher should guarantee single-threaded access unless otherwise documented.
	 *
	 * - `handle(const Message &message)` processes the incoming `Message` and returns a new `Message` representing the
	 * response.
	 *
	 * - Ownership of the returned `Message` is transferred to the caller.
	 *
	 * - Implementations should assume that `handle()` is called from a single dispatcher thread.
	 *
	 * - It is allowed for `handle()` to throw exceptions in case of unrecoverable processing errors (unless otherwise
	 * stated).
	 */
	struct MessageHandler
	{
		virtual ~MessageHandler(void);
		virtual std::unique_ptr<Message> handle(const Message &message) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_MESSAGE_HANDLER_HPP
