#ifndef SEWS_INFRASTRUCTURE_HTTP_REQUEST_PARSER_HPP
#define SEWS_INFRASTRUCTURE_HTTP_REQUEST_PARSER_HPP

#include "sews/core/interface/message.hpp"
#include "sews/core/interface/request_parser.hpp"
#include <memory>

namespace sews::format::http
{
	struct RequestParser : public interface::RequestParser
	{
		std::unique_ptr<interface::Message> parse(const std::string &raw) override;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_HTTP_REQUEST_PARSER_HPP
