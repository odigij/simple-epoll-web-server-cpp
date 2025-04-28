#include "sews/runtime/metrics/core/manager.hpp"
#include "sews/core/enums/log_type.hpp"
#include <chrono>
#include <sstream>

namespace sews::runtime::metrics
{
	Manager::Manager(std::shared_ptr<interface::Logger> logger)
		: startTime(std::chrono::steady_clock::now()), logger(logger)
	{
		this->logger->log(enums::LogType::INFO, "\033[36mMetric Manager:\033[0m Initialized.");
	}

	Manager::~Manager(void)
	{
		this->logger->log(enums::LogType::INFO, "\033[36mMetric Manager:\033[0m Terminated.");
	}

	void Manager::registerMetric(const std::string &name, enums::MetricType type)
	{
		auto [it, inserted] = metrics.try_emplace(name, Metric(0, type));
		if (!inserted)
		{
			this->logger->log(enums::LogType::WARNING, "\033[36mMetric Manager:\033[0m Failed to register metric.");
		}
	}

	void Manager::increment(const std::string &name)
	{
		auto it = metrics.find(name);
		if (it == metrics.end())
		{
			this->logger->log(enums::LogType::WARNING,
							  "\033[36mMetric Manager:\033[0m Metric not found for increment: " + name);
			return;
		}
		it->second.value++;
	}

	void Manager::decrement(const std::string &name)
	{
		auto it = metrics.find(name);
		if (it == metrics.end())
		{
			this->logger->log(enums::LogType::WARNING,
							  "\033[36mMetric Manager:\033[0m Metric not found for increment: " + name);
			return;
		}
		if (it->second.type == enums::MetricType::COUNTER)
		{
			this->logger->log(enums::LogType::WARNING,
							  "\033[36mMetric Manager:\033[0m Inconvenient type metric tried to decreased.");
			return;
		}
		it->second.value--;
	}

	void Manager::set(const std::string &name, size_t value)
	{
		auto it = metrics.find(std::string(name));
		if (it == metrics.end())
		{
			this->logger->log(enums::LogType::WARNING, "\033[36mMetric Manager:\033[0m No metric found to set.");
			return;
		}
		if (it->second.type != enums::MetricType::GAUGE)
		{
			this->logger->log(enums::LogType::WARNING,
							  "\033[36mMetric Manager:\033[0m Inconvinient type metric tried to set.");
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
} // namespace sews::runtime::metrics
