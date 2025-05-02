#ifndef SEWS_ARCHITECTURE_TELEMETRY_METRIC_MANAGER_HPP
#define SEWS_ARCHITECTURE_TELEMETRY_METRIC_MANAGER_HPP

#include "core/telemetry/metric/type/metric.hpp"
#include "core/telemetry/metric/manager.hpp"
#include "core/telemetry/metric/backend/metric.hpp"
#include "core/telemetry/diagnostic/logger/backend/logger.hpp"

#include <memory>
#include <unordered_map>
#include <chrono>

namespace sews::architecture::telemetry::metric
{

	struct Manager : public core::telemetry::metric::Manager
	{
		Manager(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger);
		~Manager(void) override;

		void registerMetric(const std::string &name, core::telemetry::metric::type::Metric type) override;
		void increment(const std::string &name) override;
		void decrement(const std::string &name) override;
		void set(const std::string &name, size_t value) override;
		std::string exportValues(void) const override;

	  private:
		std::unordered_map<std::string, core::telemetry::metric::transport::Metric> metrics;
		std::chrono::steady_clock::time_point startTime;
		std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger;
	};
} // namespace sews::architecture::telemetry::metric

#endif // !SEWS_ARCHITECTURE_TELEMETRY_METRIC_MANAGER_HPP
