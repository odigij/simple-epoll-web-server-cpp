#ifndef SEWS_CORE_CONNECTION_TRANSPORT_BUFFERED_CHANNEL_HPP
#define SEWS_CORE_CONNECTION_TRANSPORT_BUFFERED_CHANNEL_HPP

#include "core/connection/event_types.hpp"
#include "core/connection/transport/channel.hpp"
#include <vector>

namespace sews::core::connection::transport
{
	struct BufferedChannel : public transport::Channel
	{
		virtual std::vector<char> &getWriteBuffer(void) = 0;
		virtual size_t &getWriteOffset(void) = 0;
		virtual core::connection::WriteResult flush(void) = 0;
	};
} // namespace sews::core::connection::transport

#endif // !SEWS_CORE_CONNECTION_TRANSPORT_BUFFERED_CHANNEL_HPP
