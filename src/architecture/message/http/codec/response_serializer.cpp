#include <sstream>

#include "core/telemetry/diagnostic/logger/backend/logger.hpp"
#include "architecture/message/http/transport/response.hpp"
#include "architecture/message/http/codec/response_serializer.hpp"

namespace sews::architecture::message::http::codec
{
	ResponseSerializer::ResponseSerializer(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger)
		: logger(logger)
	{
		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
					"\033[36mHttp Response Serializer:\033[0m Initialized.");
	}

	ResponseSerializer::~ResponseSerializer(void)
	{
		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
					"\033[36mHttp Response Serializer:\033[0m Terminated.");
	}

	std::string ResponseSerializer::serialize(const core::message::transport::Message &response) const
	{
		if (response.type() != core::message::type::Message::HttpResponse)
		{
			logger->log(core::telemetry::diagnostic::logger::type::Log::ERROR,
						"\033[36mHttp Response Serializer:\033[0m Inconvinient message type.");
			return std::string();
		}
		const auto &httpResponse{dynamic_cast<const architecture::message::http::transport::Response &>(response)};
		std::ostringstream oss{""};
		oss << (httpResponse.version.empty() ? "HTTP/1.1" : httpResponse.version) << " " << httpResponse.status << " "
			<< httpResponse.statusText << "\r\n";
		for (auto header : httpResponse.headers)
		{
			oss << header.first << ": " << header.second << "\r\n";
		}
		if (httpResponse.headers.find("Content-Length") == httpResponse.headers.end())
		{
			oss << "Content-Length: " << httpResponse.body.size() << "\r\n";
		}
		oss << "\r\n" << httpResponse.body;
		return oss.str();
	}
} // namespace sews::architecture::message::http::codec
