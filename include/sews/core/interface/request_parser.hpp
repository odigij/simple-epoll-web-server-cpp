#ifndef SEWS_CORE_INTERFACE_REQUEST_PARSER_HPP
#define SEWS_CORE_INTERFACE_REQUEST_PARSER_HPP

#include "sews/core/interface/message.hpp"
#include <memory>
#include <string>

namespace sews::interface
{
	/*
	 * Module: RequestParser (interface).
	 *
	 * Purpose: Defines the contract for protocol-based parsing of a raw string into a structured Message.
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
	struct RequestParser
	{
		virtual ~RequestParser(void);
		virtual std::unique_ptr<Message> parse(const std::string &raw) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_REQUEST_PARSER_HPP
