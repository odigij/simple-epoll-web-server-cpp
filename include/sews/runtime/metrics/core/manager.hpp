#ifndef SEWS_RUNTIME_METRICS_CORE_MANAGER_HPP
#define SEWS_RUNTIME_METRICS_CORE_MANAGER_HPP

#include "sews/core/enums/metric_type.hpp"
#include "sews/core/interface/logger.hpp"
#include "sews/core/interface/metrics.hpp"
#include "sews/runtime/metrics/core/data.hpp"
#include <memory>
#include <unordered_map>
#include <chrono>

namespace sews::runtime::metrics
{

	struct Manager : public interface::Metrics
	{
		Manager(std::shared_ptr<interface::Logger> logger);
		~Manager(void) override;

		void registerMetric(const std::string &name, enums::MetricType type);
		void increment(const std::string &name) override;
		void decrement(const std::string &name) override;
		void set(const std::string &name, size_t value) override;
		std::string exportValues(void) const override;

	  private:
		std::unordered_map<std::string, Metric> metrics;
		std::chrono::steady_clock::time_point startTime;
		std::shared_ptr<interface::Logger> logger;
	};
} // namespace sews::runtime::metrics

#endif // !SEWS_RUNTIME_METRICS_CORE_MANAGER_HPP
