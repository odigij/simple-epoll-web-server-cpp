#ifndef SEWS_CORE_MESSAGE_TYPE_MESSAGE_HPP
#define SEWS_CORE_MESSAGE_TYPE_MESSAGE_HPP

namespace sews::core::message::type
{
	enum class Message
	{
		HttpRequest,
		HttpResponse,
		Unknown
	};
} // namespace sews::core::message::type

#endif // !SEWS_CORE_MESSAGE_TYPE_MESSAGE_HPP
