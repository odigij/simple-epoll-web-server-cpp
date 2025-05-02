#include "architecture/telemetry/diagnostic/logger/backend/basic.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace sews::architecture::telemetry::diagnostic::logger::transport
{
	Basic::Basic(void)
	{
		log(core::telemetry::diagnostic::logger::type::Log::INFO, "\033[36mBasic Logger:\033[0m Initialized.");
	}

	Basic::~Basic(void)
	{
		log(core::telemetry::diagnostic::logger::type::Log::INFO, "\033[36mBasic Logger:\033[0m Terminated.");
	}

	void Basic::log(core::telemetry::diagnostic::logger::type::Log flag, std::string_view message)
	{
		std::ostream &out = (flag == core::telemetry::diagnostic::logger::type::Log::ERROR) ? std::cerr : std::cout;
		switch (flag)
		{
			case core::telemetry::diagnostic::logger::type::Log::INFO:
				out << "\033[32m"; // Green
				break;
			case core::telemetry::diagnostic::logger::type::Log::WARNING:
				out << "\033[33m"; // Yellow
				break;
			case core::telemetry::diagnostic::logger::type::Log::ERROR:
				out << "\033[31m"; // Red
				break;
			default:
				out << "\033[0m"; // Reset
				break;
		}
		out << " [ " << getTimestamp() << " | ";
		switch (flag)
		{
			case core::telemetry::diagnostic::logger::type::Log::INFO:
				out << "INFO";
				break;

			case core::telemetry::diagnostic::logger::type::Log::WARNING:
				out << "WARNING";
				break;

			case core::telemetry::diagnostic::logger::type::Log::ERROR:
				out << "ERROR";
				break;

			default:
				out << "UNKNOWN";
				break;
		}
		out << " ] " << "\033[0m" << message << "\033[0m\n";
	}

	std::string Basic::getTimestamp(void)
	{
		// NOTE: MS calculation depracated for now.
		std::chrono::time_point now = std::chrono::system_clock::now();
		std::time_t in_time = std::chrono::system_clock::to_time_t(now);
		// std::chrono::duration ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) %
		// 1000;
		std::stringstream ss;
		// ss << std::put_time(std::localtime(&in_time), "%F %T") << '.' << std::setfill('0') << std::setw(3)
		//    << ms.count();
		ss << std::put_time(std::localtime(&in_time), "%F %T");
		return ss.str();
	}
} // namespace sews::architecture::telemetry::diagnostic::logger::transport
