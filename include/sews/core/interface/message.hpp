#ifndef SEWS_CORE_INTERFACE_MESSAGE_HPP
#define SEWS_CORE_INTERFACE_MESSAGE_HPP

#include "sews/core/enums/message_type.hpp"
#include <string>

namespace sews::interface
{
	struct Message
	{
		virtual ~Message(void);
		virtual enums::MessageType type(void) const = 0;
		virtual std::string payload(void) const = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_MESSAGE_HPP
