#ifndef SEWS_CORE_ENUMS_SOCKET_EVENT_HPP
#define SEWS_CORE_ENUMS_SOCKET_EVENT_HPP

namespace sews::enums
{
	enum class SocketEvent
	{
		READ,
		WRITE,
		HANGUP,
		ERROR,
	};
} // namespace sews::enums

#endif // !SEWS_CORE_ENUMS_SOCKET_EVENT_HPP
