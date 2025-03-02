#ifndef APP_HANDLERS_HPP
#define APP_HANDLERS_HPP

#include "../include/request.hpp"

#include <string>

namespace app {
	const std::string handleIndex(const sews::Request& request);
	const std::string handleStyle(const sews::Request& request);
} // namespace app

#endif
