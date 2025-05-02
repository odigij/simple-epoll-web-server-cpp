#ifndef SEWS_ARCHITECTURE_HTTP_CODEC_REQUEST_PARSER_HPP
#define SEWS_ARCHITECTURE_HTTP_CODEC_REQUEST_PARSER_HPP

#include "core/message/codec/decoder.hpp"
#include "core/telemetry/diagnostic/logger/backend/logger.hpp"
#include "core/message/transport/message.hpp"
#include <memory>

namespace sews::architecture::message::http::codec
{
	struct RequestParser : public core::message::codec::Decoder
	{
		RequestParser(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger);
		~RequestParser(void) override;
		std::unique_ptr<core::message::transport::Message> parse(const std::string &raw) override;

	  private:
		std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger;
	};
} // namespace sews::architecture::message::http::codec

#endif // !SEWS_ARCHITECTURE_HTTP_CODEC_REQUEST_PARSER_HPP
