#ifndef SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_BASIC_LOGGER_HPP
#define SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_BASIC_LOGGER_HPP

#include "core/telemetry/diagnostic/logger/backend/logger.hpp"

namespace sews::architecture::telemetry::diagnostic::logger::transport
{
	struct Basic : public core::telemetry::diagnostic::transport::Logger // Do not use this logger as hot path due it
																		 // uses std::ostringstream
	{
		Basic(void);
		~Basic(void) override;
		void log(core::telemetry::diagnostic::logger::type::Log flag, std::string_view message) override;

	  private:
		std::string getTimestamp(void);
	};
} // namespace sews::architecture::telemetry::diagnostic::logger::transport

#endif // !SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_LOGGER_BACKEND_BASIC_LOGGER_HPP
