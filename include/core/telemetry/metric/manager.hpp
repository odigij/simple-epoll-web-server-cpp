#ifndef SEWS_CORE_TELEMETRY_METRIC_MANAGER_HPP
#define SEWS_CORE_TELEMETRY_METRIC_MANAGER_HPP

#include "core/telemetry/metric/type/metric.hpp"

#include <string>

namespace sews::core::telemetry::metric
{
	/*
	 * Purpose: Defines the contract for collecting and exporting server metrics.
	 *
	 * Ownership: No ownership of external resources.
	 */
	struct Manager
	{
		virtual ~Manager(void) = default;
		virtual void registerMetric(const std::string &name, core::telemetry::metric::type::Metric type) = 0;
		virtual void increment(const std::string &name) = 0;
		virtual void decrement(const std::string &name) = 0;
		virtual void set(const std::string &name, size_t value) = 0;
		// Called to export all metrics as a human-readable or machine-readable string.
		virtual std::string exportValues(void) const = 0;
	};
} // namespace sews::core::telemetry::metric

#endif // !SEWS_CORE_TELEMETRY_METRIC_MANAGER_HPP
