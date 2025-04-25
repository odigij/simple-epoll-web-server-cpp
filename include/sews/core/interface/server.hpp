#ifndef SEWS_CORE_INTERFACE_SERVER_HPP
#define SEWS_CORE_INTERFACE_SERVER_HPP

namespace sews::interface
{
	struct Server
	{
		virtual ~Server(void);
		virtual void run(void) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_SERVER_HPP
