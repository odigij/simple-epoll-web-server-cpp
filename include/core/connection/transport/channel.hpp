#ifndef SEWS_CORE_CONNECTION_TRANSPORT_CHANNEL_HPP
#define SEWS_CORE_CONNECTION_TRANSPORT_CHANNEL_HPP

#include <sys/types.h>

namespace sews::core::connection::transport
{
	/*
	 * Module: Channel (interface).
	 *
	 * Purpose: Defines the contract for `low-level socket I/O` operations and resource management.
	 *
	 * Ownership: Fully responsible for managing and closing the associated socket file descriptor.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations may or may not be thread-safe.
	 *
	 * - Dispatcher should guarantee single-threaded access unless otherwise documented.
	 */
	struct Channel
	{
		virtual ~Channel(void) = default;
		// Returns the owned low-level socket file descriptor.
		virtual int getFd(void) const = 0;
		// Reads limited size of bytes from the low-level socket and stores to external buffer.
		virtual ssize_t readRaw(char *buffer, size_t len) = 0;
		// Writes limited size of bytes from external buffer through the socket.
		virtual ssize_t writeRaw(const char *buffer, size_t len) = 0;
		// Closes the low-level socket.
		virtual void close(void) = 0;
	};
} // namespace sews::core::connection::transport

#endif // !SEWS_CORE_CONNECTION_TRANSPORT_CHANNEL_HPP
