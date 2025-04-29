#include "sews/infrastructure/transport/plain_text_channel.hpp"
#include <unistd.h>
#include <utility>

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

	const std::pair<std::string, uint16_t> PlainTextChannel::getDetails(void)
	{
		return std::make_pair(ip, port);
	}

	void PlainTextChannel::setDetails(const uint16_t port, const std::string ip)
	{
		this->port = port;
		this->ip = ip;
	}

} // namespace sews::transport
