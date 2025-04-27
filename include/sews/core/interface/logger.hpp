#ifndef SEWS_CORE_INTERFACE_LOGGER_HPP
#define SEWS_CORE_INTERFACE_LOGGER_HPP

#include "sews/core/enums/log_type.hpp"
#include <string_view>

namespace sews::interface
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
		virtual ~Logger(void);
		// Logs given message in a format with enums::LogType.
		virtual void log(enums::LogType flag, std::string_view message) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_LOGGER_HPP
