#ifndef SEWS_RUNTIME_METRICS_HANDLERS_HTTP_EXPORTER_HPP
#define SEWS_RUNTIME_METRICS_HANDLERS_HTTP_EXPORTER_HPP

#include "sews/core/interface/message_handler.hpp"
#include "sews/core/interface/metrics.hpp"
#include "sews/core/interface/response_serializer.hpp"

namespace sews::runtime::metrics
{
	struct HttpMetricExporter : public interface::MessageHandler
	{
		HttpMetricExporter(interface::Metrics *metrics, interface::ResponseSerializer *serializer);

		std::unique_ptr<interface::Message> handle(const interface::Message &request) override;

	  private:
		interface::Metrics *metrics;
		interface::ResponseSerializer *serializer;
	};
} // namespace sews::runtime::metrics

#endif // !SEWS_RUNTIME_METRICS_HANDLERS_HTTP_EXPORTER_HPP
