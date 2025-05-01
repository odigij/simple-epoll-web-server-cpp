#ifndef SEWS_ARCHITECTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP
#define SEWS_ARCHITECTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP

#include "core/connection/transport/buffered_channel.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace sews::architecture::connection::transport
{
	struct PlainTextChannel : public core::connection::transport::BufferedChannel
	{
		explicit PlainTextChannel(int fd);
		~PlainTextChannel(void) override;
		int getFd(void) const override;
		ssize_t readRaw(char *buffer, size_t len) override;
		ssize_t writeRaw(const char *buffer, size_t len) override;
		void close(void) override;
		const std::pair<std::string, uint16_t> getDetails(void);
		void setDetails(const uint16_t port, const std::string ip);
		std::vector<char> &getWriteBuffer(void) override;
		size_t &getWriteOffset(void) override;
		core::connection::WriteResult flush(void) override;

	  private:
		int fd{-1};
		uint16_t port;
		size_t writeOffset;
		std::string ip, response;
		std::vector<char> writeBuffer;
	};
} // namespace sews::architecture::connection::transport

#endif // !SEWS_ARCHITECTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP
