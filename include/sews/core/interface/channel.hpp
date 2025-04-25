#ifndef SEWS_CORE_INTERFACE_CHANNEL_HPP
#define SEWS_CORE_INTERFACE_CHANNEL_HPP

#include <sys/types.h>

namespace sews::interface
{
	struct Channel
	{
		virtual ~Channel(void);
		virtual int getFd(void) const = 0;
		virtual ssize_t readRaw(char *buffer, size_t len) = 0;
		virtual ssize_t writeRaw(const char *buffer, size_t len) = 0;
		virtual void close(void) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_CHANNEL_HPP
