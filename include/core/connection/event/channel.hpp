#ifndef SEWS_CORE_CONNECTION_EVENT_CHANNEL_HPP
#define SEWS_CORE_CONNECTION_EVENT_CHANNEL_HPP

namespace sews::core::connection::event
{
	enum class Channel
	{
		READ,
		WRITE,
		HANGUP,
		ERROR,
	};
} // namespace sews::core::connection::event

#endif // !SEWS_CORE_CONNECTION_EVENT_CHANNEL_HPP
