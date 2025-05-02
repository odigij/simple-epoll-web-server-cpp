#ifndef SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_NULL_LOGGER_HPP
#define SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_NULL_LOGGER_HPP

#include "core/telemetry/diagnostic/logger/backend/logger.hpp"
#include "core/telemetry/diagnostic/logger/type/log.hpp"

namespace sews::architecture::telemetry::diagnostic::logger::backend
{
	/*
	 * It does nothing, just a place holder when you don't need a logger.
	 */
	struct Null : public core::telemetry::diagnostic::transport::Logger
	{
		Null(void) {};
		~Null(void) override {};
		void log(core::telemetry::diagnostic::logger::type::Log flag, std::string_view message) override
		{
		}
	};
} // namespace sews::architecture::telemetry::diagnostic::logger::backend

#endif // !SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_NULL_LOGGER_HPP
