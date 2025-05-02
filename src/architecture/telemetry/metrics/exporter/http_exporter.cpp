#include "architecture/telemetry/metric/exporter/http_exporter.hpp"
#include "architecture/message/http/transport/response.hpp"

namespace sews::architecture::telemetry::metric::exporter
{
	HttpMetricExporter::HttpMetricExporter(core::telemetry::metric::Manager *metrics,
										   core::message::codec::Encoder *serializer)
		: metrics(metrics), serializer(serializer)
	{
	}

	std::unique_ptr<core::message::transport::Message> HttpMetricExporter::handle(
		const core::message::transport::Message &request)
	{
		architecture::message::http::transport::Response response;
		response.version = "HTTP/1.1";
		response.status = 200;
		response.statusText = "OK";
		response.headers["Content-Type"] = "text/plain";
		response.headers["Connection"] = "keep-alive";
		response.body = metrics->exportValues();

		return std::make_unique<architecture::message::http::transport::Response>(std::move(response));
	}
} // namespace sews::architecture::telemetry::metric::exporter
