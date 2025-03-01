#ifndef SEWS_SIGNAL_HPP
#define SEWS_SIGNAL_HPP

#include <csignal>

namespace sews {
    class SignalHandler {
      public:
        static void init();
        static void handle(int signal);
        static int getSignal();

      private:
        static volatile sig_atomic_t flags;
    };
} // namespace sews

#endif // !SEWS_SIGNAL_HPP
