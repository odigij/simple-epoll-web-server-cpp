#include "sews/runtime/metrics/handlers/http_exporter.hpp"
#include "sews/infrastructure/format/http/response.hpp"

namespace sews::runtime::metrics
{
	HttpMetricExporter::HttpMetricExporter(interface::Metrics *metrics, interface::ResponseSerializer *serializer)
		: metrics(metrics), serializer(serializer)
	{
	}

	std::unique_ptr<interface::Message> HttpMetricExporter::handle(const interface::Message &request)
	{
		format::http::Response response;
		response.version = "HTTP/1.1";
		response.status = 200;
		response.statusText = "OK";
		response.headers["Content-Type"] = "text/plain";
		response.headers["Connection"] = "close";
		response.body = metrics->exportValues();

		return std::make_unique<format::http::Response>(std::move(response));
	}
} // namespace sews::runtime::metrics
