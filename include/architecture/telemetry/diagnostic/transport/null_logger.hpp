#ifndef SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_NULL_LOGGER_HPP
#define SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_NULL_LOGGER_HPP

#include "core/telemetry/diagnostic/transport/logger.hpp"

namespace sews::architecture::telemetry::diagnostic::transport
{
	/*
	 * It does nothing, just a place holder when you don't need a logger.
	 */
	struct NullLogger : public core::telemetry::diagnostic::transport::Logger
	{
		NullLogger(void) {};
		~NullLogger(void) override {};
		void log(core::telemetry::diagnostic::LogType flag, std::string_view message) override
		{
		}
	};
} // namespace sews::architecture::telemetry::diagnostic::transport

#endif // !SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_NULL_LOGGER_HPP
