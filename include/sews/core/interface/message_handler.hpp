#ifndef SEWS_CORE_INTERFACE_MESSAGE_HANDLER_HPP
#define SEWS_CORE_INTERFACE_MESSAGE_HANDLER_HPP

#include "sews/core/interface/message.hpp"
#include <memory>

namespace sews::interface
{
	struct MessageHandler
	{
		virtual ~MessageHandler(void);
		virtual std::unique_ptr<Message> handle(const Message &message) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_MESSAGE_HANDLER_HPP
