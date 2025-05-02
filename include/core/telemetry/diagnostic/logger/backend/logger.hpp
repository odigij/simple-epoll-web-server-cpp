#ifndef SEWS_CORE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_LOGGER_HPP
#define SEWS_CORE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_LOGGER_HPP

#include "core/telemetry/diagnostic/logger/type/log.hpp"
#include <string_view>

namespace sews::core::telemetry::diagnostic::transport
{
	/*
	 * Purpose: Defines the contract for logger to use in modules.
	 *
	 * Ownership: Does not manage ownership beyond what is internally required by implementations.
	 */
	struct Logger
	{
		virtual ~Logger(void) = default;
		// Logs given message in a format with enums::LogType.
		virtual void log(core::telemetry::diagnostic::logger::type::Log flag, std::string_view message) = 0;
	};
} // namespace sews::core::telemetry::diagnostic::transport

#endif // !SEWS_CORE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_LOGGER_HPP
