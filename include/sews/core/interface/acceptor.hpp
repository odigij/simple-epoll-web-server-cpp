#ifndef SEWS_CORE_INTERFACE_ACCEPTOR_HPP
#define SEWS_CORE_INTERFACE_ACCEPTOR_HPP

#include "sews/core/interface/channel.hpp"
#include <memory>

namespace sews::interface
{
	struct Acceptor
	{
		virtual ~Acceptor(void);
		virtual std::unique_ptr<Channel> accept(void) = 0;
		virtual interface::Channel &channel(void) = 0;
	};
} // namespace sews::interface

#endif // !SEWS_CORE_INTERFACE_ACCEPTOR_HPP
