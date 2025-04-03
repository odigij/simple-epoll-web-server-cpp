/*
Copyright (c) 2025 Yiğit Leblebicier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "sews/logger.hpp"
#include <chrono>
#include <fmt/format.h>
#include <iomanip>
#include <iostream>

std::string sews::logger::get_timestamp()
{
	std::chrono::time_point now = std::chrono::system_clock::now();
	std::time_t in_time = std::chrono::system_clock::to_time_t(now);
	std::chrono::duration ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time), "%F %T") << '.' << std::setfill('0') << std::setw(3) << ms.count();
	return ss.str();
}

void sews::logger::log(Mode mode, const std::string &message)
{
	std::ostream &out = (mode == ERROR) ? std::cerr : std::cout;
	std::string tag;
	switch (mode)
	{
		case INFO:
			tag = "INFO";
			break;
		case WARN:
			tag = "WARN";
			break;
		case ERROR:
			tag = "ERROR";
			break;
	}
	out << fmt::format("[ {} | {} ] {}\n", get_timestamp(), tag, message);
}
