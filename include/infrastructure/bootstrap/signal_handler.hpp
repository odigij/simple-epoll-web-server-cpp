#ifndef SEWS_INFRASTRUCTURE_BOOTSTRAP_SIGNAL_HANDLER_HPP
#define SEWS_INFRASTRUCTURE_BOOTSTRAP_SIGNAL_HANDLER_HPP

#include <csignal>

namespace sews::infrastructure::bootstrap
{
	void handleSigint(int);
	void setupSignalHandlers(void);
} // namespace sews::infrastructure::bootstrap

#endif // !SEWS_INFRASTRUCTURE_BOOTSTRAP_SIGNAL_HANDLER_HPP
