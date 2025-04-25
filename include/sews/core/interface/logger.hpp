#ifndef SEWS_CORE_INTERFACE_LOGGER_HPP
#define SEWS_CORE_INTERFACE_LOGGER_HPP

#include "sews/core/enums/log_type.hpp"
#include <string_view>

namespace sews::interface
{
	struct Logger
	{
		virtual ~Logger(void);
		virtual void log(enums::LogType flag, std::string_view message) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_LOGGER_HPP
