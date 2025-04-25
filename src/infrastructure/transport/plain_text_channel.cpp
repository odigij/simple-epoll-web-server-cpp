#include "sews/infrastructure/transport/plain_text_channel.hpp"
#include <unistd.h>

namespace sews::transport
{
	PlainTextChannel::PlainTextChannel(int fd) : fd(fd)
	{
	}

	PlainTextChannel::~PlainTextChannel(void)
	{
		close();
	}

	int PlainTextChannel::getFd(void) const
	{
		return fd;
	}

	ssize_t PlainTextChannel::readRaw(char *buffer, size_t len)
	{
		return ::read(fd, buffer, len);
	}

	ssize_t PlainTextChannel::writeRaw(const char *buffer, size_t len)
	{
		return ::write(fd, buffer, len);
	}

	void PlainTextChannel::close(void)
	{
		if (fd >= 0)
		{
			::close(fd);
			fd = -1;
		}
	}

	std::string &PlainTextChannel::getResponse(void)
	{
		return response;
	}

} // namespace sews::transport
