#ifndef SEWS_CORE_MESSAGE_CODEC_DECODER_HPP
#define SEWS_CORE_MESSAGE_CODEC_DECODER_HPP

#include "core/message/transport/message.hpp"

#include <memory>

namespace sews::core::message::codec
{
	/*
	 * Purpose: Defines the contract for protocol-based parsing of a raw string into a structured Message.
	 *
	 * Ownership: Does not manage ownership beyond what is internally required by implementations.
	 */
	struct Decoder
	{
		virtual ~Decoder(void) = default;
		virtual std::unique_ptr<transport::Message> parse(const std::string &raw) = 0;
	};
} // namespace sews::core::message::codec

#endif // !SEWS_CORE_MESSAGE_CODEC_DECODER_HPP
