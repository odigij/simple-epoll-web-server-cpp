#include "sews/infrastructure/format/http/response.hpp"
#include "sews/infrastructure/format/http/response_serializer.hpp"
#include <sstream>

namespace sews::format::http
{
	std::string ResponseSerializer::serialize(const interface::Message &response) const
	{
		if (response.type() != enums::MessageType::HttpResponse)
		{
			return "";
		}
		const auto &httpResponse{dynamic_cast<const format::http::Response &>(response)};
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
} // namespace sews::format::http
