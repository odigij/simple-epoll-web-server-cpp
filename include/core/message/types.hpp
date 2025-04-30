#ifndef SEWS_CORE_MESSAGE_TYPES_HPP
#define SEWS_CORE_MESSAGE_TYPES_HPP

namespace sews::core::message
{
	enum class MessageType
	{
		HttpRequest,
		HttpResponse,
		PeerPing,
		EventBroadcast,
		Unknown
	};
} // namespace sews::core::message

#endif // !SEWS_CORE_MESSAGE_TYPES_HPP
