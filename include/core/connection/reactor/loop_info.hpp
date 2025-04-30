#ifndef SEWS_CORE_CONNECTION_REACTOR_LOOP_INFO_HPP
#define SEWS_CORE_CONNECTION_REACTOR_LOOP_INFO_HPP

#include <cstddef>

namespace sews::core::connection::reactor
{
	struct LoopInfo
	{
		virtual ~LoopInfo() = default;
		virtual size_t getEventCapacity() const = 0;
	};
} // namespace sews::core::connection::reactor

#endif // !SEWS_CORE_CONNECTION_REACTOR_LOOP_INFO_HPP
