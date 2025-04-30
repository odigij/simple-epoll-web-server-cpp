#ifndef SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_BASIC_LOGGER_HPP
#define SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_BASIC_LOGGER_HPP

#include "core/telemetry/diagnostic/transport/logger.hpp"

namespace sews::architecture::telemetry::diagnostic::transport
{
	struct BasicLogger : public core::telemetry::diagnostic::transport::Logger // Do not use this logger as hot path due
																			   // to it uses std::ostringstream
	{
		BasicLogger(void);
		~BasicLogger(void) override;
		void log(core::telemetry::diagnostic::LogType flag, std::string_view message) override;

	  private:
		std::string getTimestamp(void);
	};
} // namespace sews::architecture::telemetry::diagnostic::transport

#endif // !SEWS_ARCHITECTURE_TELEMETRY_DIAGNOSTIC_BASIC_LOGGER_HPP
