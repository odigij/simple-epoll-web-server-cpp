#include "sews/bootstrap/signal_handler.hpp"
#include "sews/core/stop_flag.hpp"

namespace sews::bootstrap
{
	void handleSigint(int)
	{
		stopFlag.store(true);
	}

	void setupSignalHandlers(void)
	{
		std::signal(SIGINT, handleSigint);
		std::signal(SIGTERM, handleSigint);
	}
} // namespace sews::bootstrap
