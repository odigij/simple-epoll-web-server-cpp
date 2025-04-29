#ifndef SEWS_INFRASTRUCTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP
#define SEWS_INFRASTRUCTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP

#include "sews/core/interface/channel.hpp"
#include <cstdint>
#include <string>

namespace sews::transport
{
	struct PlainTextChannel : public interface::Channel
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
} // namespace sews::transport

#endif // !SEWS_INFRASTRUCTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP
