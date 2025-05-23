#ifndef SEWS_INFRASTRUCTURE_HTTP_RESPONSE_SERIALIZER_HPP

#include "core/message/codec/encoder.hpp"
#include "core/telemetry/diagnostic/logger/backend/logger.hpp"

#include <memory>

namespace sews::architecture::message::http::codec
{
	struct ResponseSerializer : public core::message::codec::Encoder
	{
		ResponseSerializer(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger);
		~ResponseSerializer(void) override;
		std::string serialize(const core::message::transport::Message &response) const override;

	  private:
		std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger;
	};
} // namespace sews::architecture::message::http::codec

#endif // !SEWS_INFRASTRUCTURE_HTTP_RESPONSE_SERIALIZER_HPP
