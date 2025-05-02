#ifndef SEWS_CORE_MESSAGE_CODEC_ENCODER_HPP
#define SEWS_CORE_MESSAGE_CODEC_ENCODER_HPP

#include "core/message/transport/message.hpp"

namespace sews::core::message::codec
{
	/*
	 * Purpose: Defines the contract for protocol-based serialization of structured Messages into raw strings.
	 *
	 * Ownership: Does not manage ownership beyond what is internally required by implementations.
	 */
	struct Encoder
	{
		virtual ~Encoder(void) = default;
		virtual std::string serialize(const transport::Message &response) const = 0;
	};
} // namespace sews::core::message::codec

#endif // !SEWS_CORE_MESSAGE_CODEC_ENCODER_HPP
