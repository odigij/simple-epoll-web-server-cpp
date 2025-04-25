#ifndef SEWS_INFRASTRUCTURE_HTTP_RESPONSE_SERIALIZER_HPP

#include "sews/core/interface/message.hpp"
#include "sews/core/interface/response_serializer.hpp"

namespace sews::format::http
{
	struct ResponseSerializer : public interface::ResponseSerializer
	{
		std::string serialize(const interface::Message &response) const override;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_HTTP_RESPONSE_SERIALIZER_HPP
