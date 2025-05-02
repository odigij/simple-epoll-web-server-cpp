#include "architecture/connection/transport/plain_text_channel.hpp"

#include <unistd.h>

namespace sews::architecture::connection::transport
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

	const std::pair<std::string, uint16_t> PlainTextChannel::getDetails(void)
	{
		return std::make_pair(ip, port);
	}

	void PlainTextChannel::setDetails(const uint16_t port, const std::string ip)
	{
		this->port = port;
		this->ip = ip;
	}

	std::vector<char> &PlainTextChannel::getWriteBuffer(void)
	{
		return writeBuffer;
	}

	size_t &PlainTextChannel::getWriteOffset(void)
	{
		return writeOffset;
	}

	core::connection::event::Write PlainTextChannel::flush(void)
	{
		while (writeOffset < writeBuffer.size())
		{
			ssize_t written = writeRaw(writeBuffer.data() + writeOffset, writeBuffer.size() - writeOffset);
			if (written < 0)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					return core::connection::event::Write::WOULDBLOCK;
				}
				return core::connection::event::Write::FAILED;
			}
			writeOffset += written;
		}
		writeBuffer.clear();
		writeOffset = 0;
		return core::connection::event::Write::DONE;
	}
} // namespace sews::architecture::connection::transport
