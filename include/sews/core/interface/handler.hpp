#ifndef SEWS_INFRASTRUCTURE_FORMAT_HTTP_HANDLER_HPP
#define SEWS_INFRASTRUCTURE_FORMAT_HTTP_HANDLER_HPP

#include "sews/infrastructure/format/http/request.hpp"
#include "sews/infrastructure/format/http/response.hpp"
#include <memory>

namespace sews::format::http
{
	struct Handler
	{
		virtual std::unique_ptr<Response> handle(const Request &request) = 0;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_FORMAT_HTTP_HANDLER_HPP
