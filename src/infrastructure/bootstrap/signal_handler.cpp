#include "infrastructure/bootstrap/signal_handler.hpp"
#include "infrastructure/control/stop_signal.hpp"

#include <csignal>

namespace sews::infrastructure::bootstrap
{
	void handleSigint(int)
	{
		infrastructure::control::stopFlag.store(true);
	}

	void setupSignalHandlers(void)
	{
		std::signal(SIGINT, handleSigint);
		std::signal(SIGTERM, handleSigint);
	}
} // namespace sews::infrastructure::bootstrap
