#include "sews/infrastructure/io/logger/basic_logger.hpp"
#include "sews/core/enums/log_type.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace sews::io::logger
{
	BasicLogger::BasicLogger(void)
	{
		log(enums::LogType::INFO, "\033[36mBasic Logger:\033[0m Initialized.");
	}

	BasicLogger::~BasicLogger(void)
	{
		log(enums::LogType::INFO, "\033[36mBasic Logger:\033[0m Terminated");
	}

	void BasicLogger::log(enums::LogType flag, std::string_view message)
	{
		std::ostream &out = (flag == enums::LogType::ERROR) ? std::cerr : std::cout;
		switch (flag)
		{
			case enums::LogType::INFO:
				out << "\033[32m"; // Green
				break;
			case enums::LogType::WARNING:
				out << "\033[33m"; // Yellow
				break;
			case enums::LogType::ERROR:
				out << "\033[31m"; // Red
				break;
			default:
				out << "\033[0m"; // Reset
				break;
		}
		out << " [ " << getTimestamp() << " | ";
		switch (flag)
		{
			case enums::LogType::INFO:
				out << "INFO";
				break;

			case enums::LogType::WARNING:
				out << "WARNING";
				break;

			case enums::LogType::ERROR:
				out << "ERROR";
				break;

			default:
				out << "UNKNOWN";
				break;
		}
		out << " ] " << "\033[0m" << message << "\033[0m\n";
	}

	std::string BasicLogger::getTimestamp(void)
	{
		std::chrono::time_point now = std::chrono::system_clock::now();
		std::time_t in_time = std::chrono::system_clock::to_time_t(now);
		std::chrono::duration ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
		std::stringstream ss;
		ss << std::put_time(std::localtime(&in_time), "%F %T") << '.' << std::setfill('0') << std::setw(3)
		   << ms.count();
		return ss.str();
	}
} // namespace sews::io::logger
