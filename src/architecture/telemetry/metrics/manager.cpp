#include "architecture/telemetry/metric/manager.hpp"

#include <chrono>
#include <sstream>

namespace sews::architecture::telemetry::metric
{
	Manager::Manager(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger)
		: startTime(std::chrono::steady_clock::now()), logger(logger)
	{
		this->logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
						  "\033[36mMetric MetricsManager:\033[0m Initialized.");
	}

	Manager::~Manager(void)
	{
		this->logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
						  "\033[36mMetric MetricsManager:\033[0m Terminated.");
	}

	void Manager::registerMetric(const std::string &name, core::telemetry::metric::type::Metric type)
	{
		// WARNING: Metric initialization must be in try_emplace() directly, so std::atomic<> would work.
		auto [it, inserted] = metrics.try_emplace(name, core::telemetry::metric::transport::Metric(0, type));
		if (!inserted)
		{
			this->logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
							  "\033[36mMetric MetricsManager:\033[0m Failed to register metric.");
		}
	}

	void Manager::increment(const std::string &name)
	{
		auto it = metrics.find(name);
		if (it == metrics.end())
		{
			this->logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
							  "\033[36mMetric MetricsManager:\033[0m Metric not found for increment: \033[33m" + name);
			return;
		}
		it->second.value++;
	}

	void Manager::decrement(const std::string &name)
	{
		auto it = metrics.find(name);
		if (it == metrics.end())
		{
			this->logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
							  "\033[36mMetric MetricsManager:\033[0m Metric not found for decrement: \033[33m" + name);
			return;
		}
		if (it->second.type == core::telemetry::metric::type::Metric::COUNTER)
		{
			this->logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
							  "\033[36mMetric MetricsManager:\033[0m Inconvenient type metric tried to decreased.");
			return;
		}
		it->second.value--;
	}

	void Manager::set(const std::string &name, size_t value)
	{
		auto it = metrics.find(std::string(name));
		if (it == metrics.end())
		{
			this->logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
							  "\033[36mMetric MetricsManager:\033[0m No metric found to set.");
			return;
		}
		if (it->second.type != core::telemetry::metric::type::Metric::GAUGE)
		{
			this->logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
							  "\033[36mMetric MetricsManager:\033[0m Inconvinient type metric tried to set.");
			return;
		}
		it->second.value = value;
	}

	std::string Manager::exportValues(void) const
	{
		auto now = std::chrono::steady_clock::now();
		long uptimeSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count(),
			 seconds = uptimeSeconds % 60, minutes = (uptimeSeconds / 60) % 60, hours = (uptimeSeconds / 3600);

		std::ostringstream output;
		output << "uptime_seconds " << uptimeSeconds << "\r\n";
		output << "uptime_human " << hours << ":" << minutes << ":" << seconds << "\r\n";
		for (const auto &metric : metrics)
		{
			output << metric.first << ' ' << metric.second.value.load() << "\r\n";
		}
		return output.str();
	}
} // namespace sews::architecture::telemetry::metric
