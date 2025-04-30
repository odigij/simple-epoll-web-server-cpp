#ifndef SEWS_ARCHITECTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP
#define SEWS_ARCHITECTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP

#include "core/connection/transport/channel.hpp"
#include <cstdint>
#include <string>

namespace sews::architecture::connection::transport
{
	struct PlainTextChannel : public core::connection::transport::Channel
	{
		explicit PlainTextChannel(int fd);
		~PlainTextChannel(void) override;
		int getFd(void) const override;
		ssize_t readRaw(char *buffer, size_t len) override;
		ssize_t writeRaw(const char *buffer, size_t len) override;
		void close(void) override;
		std::string &getResponse(void);
		const std::pair<std::string, uint16_t> getDetails(void);
		void setDetails(const uint16_t port, const std::string ip);

	  private:
		int fd{-1};
		uint16_t port;
		std::string ip, response;
	};
} // namespace sews::architecture::connection::transport

#endif // !SEWS_ARCHITECTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP
