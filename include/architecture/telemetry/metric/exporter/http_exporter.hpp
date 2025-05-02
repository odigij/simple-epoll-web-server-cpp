#ifndef SEWS_ARCHITECTURE_TELEMETRY_METRIC_HTTP_EXPORTER_HPP
#define SEWS_ARCHITECTURE_TELEMETRY_METRIC_HTTP_EXPORTER_HPP

#include "core/message/dispatch/handler.hpp"
#include "core/telemetry/metric/manager.hpp"
#include "core/message/codec/encoder.hpp"

namespace sews::architecture::telemetry::metric::exporter
{
	struct HttpMetricExporter : public core::message::dispatch::Handler
	{
		HttpMetricExporter(core::telemetry::metric::Manager *metrics, core::message::codec::Encoder *serializer);

		std::unique_ptr<core::message::transport::Message> handle(
			const core::message::transport::Message &request) override;

	  private:
		core::telemetry::metric::Manager *metrics;
		core::message::codec::Encoder *serializer;
	};
} // namespace sews::architecture::telemetry::metric::exporter

#endif // !SEWS_ARCHITECTURE_TELEMETRY_METRIC_HTTP_EXPORTER_HPP
