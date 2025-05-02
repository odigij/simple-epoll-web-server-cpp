#ifndef SEWS_CORE_CONNECTION_EVENT_WRITE_HPP
#define SEWS_CORE_CONNECTION_EVENT_WRITE_HPP

namespace sews::core::connection::event
{
	enum class Write
	{
		DONE,
		WOULDBLOCK,
		FAILED
	};
} // namespace sews::core::connection::event

#endif // !SEWS_CORE_CONNECTION_EVENT_WRITE_HPP
