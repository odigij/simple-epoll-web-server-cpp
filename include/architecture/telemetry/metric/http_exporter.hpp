#ifndef SEWS_ARCHITECTURE_TELEMETRY_METRIC_HTTP_EXPORTER_HPP
#define SEWS_ARCHITECTURE_TELEMETRY_METRIC_HTTP_EXPORTER_HPP

#include "core/message/dispatch/handler.hpp"
#include "core/telemetry/metric/manager.hpp"
#include "core/message/codec/encoder.hpp"

namespace sews::architecture::telemetry::metric
{
	struct HttpMetricExporter : public core::message::dispatch::MessageHandler
	{
		HttpMetricExporter(core::telemetry::metric::MetricsManager *metrics,
						   core::message::codec::ResponseSerializer *serializer);

		std::unique_ptr<core::message::transport::Message> handle(
			const core::message::transport::Message &request) override;

	  private:
		core::telemetry::metric::MetricsManager *metrics;
		core::message::codec::ResponseSerializer *serializer;
	};
} // namespace sews::architecture::telemetry::metric

#endif // !SEWS_ARCHITECTURE_TELEMETRY_METRIC_HTTP_EXPORTER_HPP
