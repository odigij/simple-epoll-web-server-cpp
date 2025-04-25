#ifndef SEWS_INFRASTRUCTURE_LOGGER_BASIC_LOGGER_HPP
#define SEWS_INFRASTRUCTURE_LOGGER_BASIC_LOGGER_HPP

#include "sews/core/interface/logger.hpp"

namespace sews::io::logger
{
	struct BasicLogger
		: public interface::Logger // Do not use this logger as hot path due to it uses std::ostringstream
	{
		BasicLogger(void);
		~BasicLogger(void) override;
		void log(enums::LogType flag, std::string_view message) override;

	  private:
		std::string getTimestamp(void);
	};
} // namespace sews::io::logger

#endif // !SEWS_INFRASTRUCTURE_LOGGER_BASIC_LOGGER_HPP
