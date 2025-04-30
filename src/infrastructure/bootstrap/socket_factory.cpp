#include "infrastructure/bootstrap/socket_factory.hpp"
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace sews::infrastructure::bootstrap
{
	const int createSocket(int port)
	{
		int serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (serverFd < 0)
		{
			perror("socket");
			return -1;
		}

		int opt = 1;
		if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			perror("setsockopt SO_REUSEADDR");
			::close(serverFd);
			return -1;
		}

#ifdef SO_REUSEPORT
		if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		{
			perror("setsockopt SO_REUSEPORT");
			::close(serverFd);
			return -1;
		}
#endif

		sockaddr_in address{
			.sin_family = AF_INET,
			.sin_port = htons(port),
			.sin_addr =
				{
					.s_addr = INADDR_ANY,
				},
		};

		if (::bind(serverFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
		{
			perror("bind");
			::close(serverFd);
			return -1;
		}

		if (::listen(serverFd, 10) < 0)
		{
			perror("listen");
			::close(serverFd);
			return -1;
		}

		return serverFd;
	}
} // namespace sews::infrastructure::bootstrap
