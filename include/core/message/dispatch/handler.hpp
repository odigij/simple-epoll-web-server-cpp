#ifndef SEWS_CORE_MESSAGE_DISPATCH_HANDLER_HPP
#define SEWS_CORE_MESSAGE_DISPATCH_HANDLER_HPP

#include "core/message/transport/message.hpp"

#include <memory>

namespace sews::core::message::dispatch
{
	/*
	 * Purpose: Defines the contract for handling `interface::Message` based inputs.
	 *
	 * Ownership: Does not manage external resources; concrete implementations own their internal values.
	 *
	 * Thread Safety: `Not thread-safe`; access must be restricted to the dispatcher thread.
	 *
	 * Notes:
	 *
	 * - `handle(const Message &message)` processes the incoming `Message` and returns a new `Message` representing the
	 * response.
	 *
	 * - Ownership of the returned `Message` is transferred to the caller.
	 *
	 * - Implementations should assume that `handle()` is called from a single thread.
	 */
	struct Handler
	{
		virtual ~Handler(void) = default;
		virtual std::unique_ptr<transport::Message> handle(const transport::Message &message) = 0;
	};
} // namespace sews::core::message::dispatch

#endif // !SEWS_CORE_MESSAGE_DISPATCH_HANDLER_HPP
