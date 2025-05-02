#ifndef SEWS_CORE_CONNECTION_TRANSPORT_BUFFERED_CHANNEL_HPP
#define SEWS_CORE_CONNECTION_TRANSPORT_BUFFERED_CHANNEL_HPP

#include "core/connection/event/write.hpp"
#include "core/connection/transport/channel.hpp"

#include <vector>

namespace sews::core::connection::transport
{
	struct BufferedChannel : public sews::core::connection::transport::Channel
	{
		virtual std::vector<char> &getWriteBuffer(void) = 0;
		virtual size_t &getWriteOffset(void) = 0;
		virtual sews::core::connection::event::Write flush(void) = 0;
	};
} // namespace sews::core::connection::transport

#endif // !SEWS_CORE_CONNECTION_TRANSPORT_BUFFERED_CHANNEL_HPP
