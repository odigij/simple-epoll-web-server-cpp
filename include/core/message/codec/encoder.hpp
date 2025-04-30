#ifndef SEWS_CORE_MESSAGE_CODEC_ENCODER_HPP
#define SEWS_CORE_MESSAGE_CODEC_ENCODER_HPP

#include "core/message/transport/message.hpp"
#include <string>
#include <sys/types.h>

namespace sews::core::message::codec
{
	/*
	 * Module: ResponseSerializer (interface).
	 *
	 * Purpose: Defines the contract for protocol-based serialization of structured Messages into raw strings.
	 *
	 * Ownership: Does not manage ownership beyond what is internally required by implementations.
	 *
	 * Notes:
	 *
	 * - This is an abstract interface.
	 *
	 * - Concrete implementations may or may not be thread-safe.
	 *
	 * - Dispatcher should guarantee single-threaded access unless otherwise documented.
	 */
	struct ResponseSerializer
	{
		virtual ~ResponseSerializer(void) = default;
		virtual std::string serialize(const transport::Message &response) const = 0;
	};
} // namespace sews::core::message::codec

#endif // !SEWS_CORE_MESSAGE_CODEC_ENCODER_HPP
