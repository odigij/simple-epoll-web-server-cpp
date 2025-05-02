#ifndef SEWS_CORE_METRIC_BACKEND_METRIC_HPP
#define SEWS_CORE_METRIC_BACKEND_METRIC_HPP

#include "core/telemetry/metric/type/metric.hpp"
#include <atomic>

namespace sews::core::telemetry::metric::transport
{
	/*
	 * Module: Metric (struct).
	 *
	 * Purpose:
	 *   Represents a single server metric, either a counter or a gauge.
	 *   Provides thread-safe access to the metric value.
	 *
	 * Ownership:
	 *   - Stores its own independent value and type.
	 *
	 *   - No external ownership or resource management required.
	 *
	 * Notes:
	 *
	 *   - The `value` field is a `std::atomic<size_t>` for safe concurrent increments/decrements.
	 *
	 *   - Even though SEWS is currently single-threaded, atomic operations future-proof this struct against
	 * multi-threaded or asynchronous access patterns.
	 *
	 *   - Metrics are moved, not copied. (Copy constructor and assignment are explicitly deleted.)
	 *
	 *   - Moving a Metric copies the current atomic value at move time. (Atomicity is not transferred, only the value
	 * snapshot.)
	 */
	struct Metric
	{
		std::atomic<std::size_t> value;
		type::Metric type;

		Metric(void) : value(0), type(type::Metric::COUNTER)
		{
		}

		Metric(std::size_t initialValue, type::Metric t) : value(initialValue), type(t)
		{
		}

		Metric(const Metric &) = delete;
		Metric &operator=(const Metric &) = delete;

		Metric(Metric &&other) noexcept : value(other.value.load()), type(other.type)
		{
		}

		Metric &operator=(Metric &&other) noexcept
		{
			if (this != &other)
			{
				value.store(other.value.load());
				type = other.type;
			}
			return *this;
		}

		~Metric(void)
		{
		}
	};
} // namespace sews::core::telemetry::metric::transport

#endif // !SEWS_CORE_METRIC_BACKEND_METRIC_HPP
