#ifndef SEWS_CORE_RUNTIME_ORCHESTRATOR_DISPATCHER_HPP
#define SEWS_CORE_RUNTIME_ORCHESTRATOR_DISPATCHER_HPP

namespace sews::core::runtime::orchestrator
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
		virtual ~Dispatcher(void) = default;
		virtual void run(void) = 0;
	};
} // namespace sews::core::runtime::orchestrator

#endif // !SEWS_CORE_RUNTIME_ORCHESTRATOR_DISPATCHER_HPP
