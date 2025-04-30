#ifndef SEWS_INFRASTRUCTURE_CONTROL_STOP_SIGNAL_HPP
#define SEWS_INFRASTRUCTURE_CONTROL_STOP_SIGNAL_HPP

#include <atomic>

namespace sews::infrastructure::control
{
	inline std::atomic<bool> stopFlag{false};
}

#endif // !SEWS_INFRASTRUCTURE_CONTROL_STOP_SIGNAL_HPP
