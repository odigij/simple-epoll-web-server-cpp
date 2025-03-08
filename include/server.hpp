/*
Copyright (c) 2025 Yiğit Leblebicier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SEWS_SERVER_HPP
#define SEWS_SERVER_HPP

#include "router.hpp"

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <set>
#include <string>
#include <sys/epoll.h>

namespace sews {
	class Server {
	  public:
		Server(Router& router);
		Server(Server&&) = default;
		Server(const Server&) = default;
		Server& operator=(Server&&) = delete;
		Server& operator=(const Server&) = delete;
		~Server();
		void start(int port, int backlog, int timeout, int flags);
		void update(int poll_size);

	  private:
		int _flags, _file_descriptor, _epoll_file_descriptor, _timeout;
		struct Connection {
			int file_descriptor;
			SSL* ssl;
		};
		std::set<Server::Connection*> _connections;
		SSL_CTX* _ssl_ctx;
		Router& _router;
		void _createSocket(int port);
		void _initSocket(int backlog);
		void _handleEvents(epoll_event& poll_event);
		std::string _handleSocketData(epoll_event& poll_event);
		void _setUpTls();
	};
} // namespace sews

#endif // !SEWS_SERVER_HPP
