#ifndef SEWS_CORE_STOP_FLAG_HPP
#define SEWS_CORE_STOP_FLAG_HPP

#include <atomic>

namespace sews
{
	inline std::atomic<bool> stopFlag{false};
}

#endif // !SEWS_CORE_STOP_FLAG_HPP
