#ifndef SEWS_INFRASTRUCTURE_HTTP_RESPONSE_SERIALIZER_HPP

#include "sews/core/interface/message.hpp"
#include "sews/core/interface/response_serializer.hpp"
#include "sews/core/interface/logger.hpp"

namespace sews::format::http
{
	struct ResponseSerializer : public interface::ResponseSerializer
	{
		ResponseSerializer(interface::Logger *logger);
		~ResponseSerializer(void) override;
		std::string serialize(const interface::Message &response) const override;

	  private:
		interface::Logger *logger;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_HTTP_RESPONSE_SERIALIZER_HPP
