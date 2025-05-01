#ifndef SEWS_CORE_CONNECTION_EVENT_TYPES_HPP
#define SEWS_CORE_CONNECTION_EVENT_TYPES_HPP

namespace sews::core::connection
{
	enum class Events
	{
		READ,
		WRITE,
		HANGUP,
		ERROR,
	};

	enum class WriteResult
	{
		Done,
		WouldBlock,
		Failed
	};
} // namespace sews::core::connection

#endif // !SEWS_CORE_CONNECTION_EVENT_TYPES_HPP
