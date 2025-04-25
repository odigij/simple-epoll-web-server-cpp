#ifndef SEWS_CORE_INTERFACE_REQUEST_PARSER_HPP
#define SEWS_CORE_INTERFACE_REQUEST_PARSER_HPP

#include "sews/core/interface/message.hpp"
#include <memory>
#include <string>

namespace sews::interface
{
	struct RequestParser
	{
		virtual ~RequestParser(void);
		virtual std::unique_ptr<Message> parse(const std::string &raw) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_REQUEST_PARSER_HPP
