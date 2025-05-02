#ifndef SEWS_CORE_RUNTIME_ORCHESTRATOR_DISPATCHER_HPP
#define SEWS_CORE_RUNTIME_ORCHESTRATOR_DISPATCHER_HPP

namespace sews::core::runtime::orchestrator
{
	/*
	 * Purpose: Defines the control flow for orchestrating module interactions and connection handling.
	 *
	 * Ownership: Responsible for managing the lifecycle of all modules used (e.g., acceptors, socket loops, parsers,
	 * handlers).
	 */
	struct Dispatcher
	{
		virtual ~Dispatcher(void) = default;
		virtual void run(void) = 0;
	};
} // namespace sews::core::runtime::orchestrator

#endif // !SEWS_CORE_RUNTIME_ORCHESTRATOR_DISPATCHER_HPP
