#ifndef SEWS_INFRASTRUCTURE_LOGGER_NULL_LOGGER_HPP
#define SEWS_INFRASTRUCTURE_LOGGER_NULL_LOGGER_HPP

#include "sews/core/interface/logger.hpp"

namespace sews::io::logger
{
	/*
	 * It does nothing, just a place holder when you don't need a logger.
	 */
	struct NullLogger : public interface::Logger
	{
		NullLogger(void) {};
		~NullLogger(void) override {};
		void log(enums::LogType flag, std::string_view message) override
		{
		}
	};
} // namespace sews::io::logger

#endif // !SEWS_INFRASTRUCTURE_LOGGER_NULL_LOGGER_HPP
