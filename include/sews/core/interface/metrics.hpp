#ifndef SEWS_CORE_INTERFACE_METRICS_HPP
#define SEWS_CORE_INTERFACE_METRICS_HPP

#include <string>

namespace sews::interface
{
	/*
	 * Module: Metrics (interface).
	 *
	 * Purpose: Defines the contract for collecting and exporting server metrics.
	 *
	 * Ownership: No ownership of external resources.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations must document their thread-safety.
	 */
	struct Metrics
	{
		virtual ~Metrics(void);
		virtual void increment(const std::string &name) = 0;
		virtual void decrement(const std::string &name) = 0;
		virtual void set(const std::string &name, size_t value) = 0;
		// Called to export all metrics as a human-readable or machine-readable string.
		virtual std::string exportValues() const = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_METRICS_HPP
