#ifndef SEWS_BOOTSTRAP_SIGNAL_HANDLER_HPP
#define SEWS_BOOTSTRAP_SIGNAL_HANDLER_HPP

#include <csignal>

namespace sews::bootstrap
{
	void handleSigint(int);
	void setupSignalHandlers(void);
} // namespace sews::bootstrap

#endif // !SEWS_BOOTSTRAP_SIGNAL_HANDLER_HPP
