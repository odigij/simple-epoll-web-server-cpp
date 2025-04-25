#ifndef SEWS_INFRASTRUCTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP
#define SEWS_INFRASTRUCTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP

#include "sews/core/interface/channel.hpp"
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
		// TODO: Refactor response bullshit.
		std::string &getResponse(void);

	  private:
		int fd{-1};
		std::string response;
	};
} // namespace sews::transport

#endif // !SEWS_INFRASTRUCTURE_TRANSPORT_PLAIN_TEXT_CHANNEL_HPP
