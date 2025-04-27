#ifndef SEWS_CORE_INTERFACE_SERVER_HPP
#define SEWS_CORE_INTERFACE_SERVER_HPP

namespace sews::interface
{
	/*
	 * Module: Dispatcher (interface).
	 *
	 * Purpose: Defines the control flow for orchestrating module interactions and connection handling.
	 *
	 * Ownership: Responsible for managing the lifecycle of all modules used (e.g., acceptors, socket loops, parsers,
	 * handlers).
	 *
	 * Notes:
	 *
	 * - Abstract interface; no thread-safety guarantees.
	 *
	 * - Concrete implementations must document their thread-safety.
	 *
	 * - Single-threaded execution is assumed unless otherwise stated.
	 */
	struct Dispatcher
	{
		virtual ~Dispatcher(void);
		virtual void run(void) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_SERVER_HPP
