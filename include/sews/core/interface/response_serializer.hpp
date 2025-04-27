#ifndef SEWS_CORE_INTERFACE_RESPONSE_SERIALIZER_HPP
#define SEWS_CORE_INTERFACE_RESPONSE_SERIALIZER_HPP

#include "sews/core/interface/message.hpp"
#include <string>
#include <sys/types.h>

namespace sews::interface
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
		virtual ~ResponseSerializer(void);
		virtual std::string serialize(const Message &response) const = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_RESPONSE_SERIALIZER_HPP
