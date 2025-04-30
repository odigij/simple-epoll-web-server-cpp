#ifndef SEWS_CORE_TELEMETRY_DIAGNOSTIC_TRANSPORT_LOGGER_HPP
#define SEWS_CORE_TELEMETRY_DIAGNOSTIC_TRANSPORT_LOGGER_HPP

#include "core/telemetry/diagnostic/log_types.hpp"
#include <string_view>

namespace sews::core::telemetry::diagnostic::transport
{
	/*
	 * Module: Logger (interface).
	 *
	 * Purpose: Defines the contract for logger to use in modules.
	 *
	 * Ownership: Does not manage ownership beyond what is internally required by implementations.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations may or may not be thread-safe.
	 *
	 * - Dispatcher should guarantee single-threaded access unless otherwise documented.
	 */
	struct Logger
	{
		virtual ~Logger(void) = default;
		// Logs given message in a format with enums::LogType.
		virtual void log(diagnostic::LogType flag, std::string_view message) = 0;
	};
} // namespace sews::core::telemetry::diagnostic::transport

#endif // !SEWS_CORE_TELEMETRY_DIAGNOSTIC_TRANSPORT_LOGGER_HPP
