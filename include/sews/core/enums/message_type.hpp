#ifndef SEWS_CORE_ENUMS_MESSAGE_TYPE_HPP
#define SEWS_CORE_ENUMS_MESSAGE_TYPE_HPP

namespace sews::enums
{
	enum MessageType
	{
		HttpRequest,
		HttpResponse,
		PeerPing,
		EventBroadcast,
		Unknown
	};
} // namespace sews::enums

#endif // !SEWS_CORE_ENUMS_MESSAGE_TYPE_HPP
