#ifndef SEWS_CORE_INTERFACE_RESPONSE_SERIALIZER_HPP
#define SEWS_CORE_INTERFACE_RESPONSE_SERIALIZER_HPP

#include "sews/core/interface/message.hpp"
#include <string>
#include <sys/types.h>

namespace sews::interface
{
	struct ResponseSerializer
	{
		virtual ~ResponseSerializer(void);
		virtual std::string serialize(const Message &response) const = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_RESPONSE_SERIALIZER_HPP
