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

#include <set>
#include <sys/epoll.h>
#include <openssl/ssl.h>
#include <chrono>
#include "router.hpp"

namespace sews
{
	class Server
	{
	  public:
		Server(Router &);
		Server(Server &&) = default;
		Server(const Server &) = default;
		Server &operator=(Server &&) = delete;
		Server &operator=(const Server &) = delete;
		~Server();
		void start(int port, int backlog, int timeout, int flags, int epoll_pool_size);
		void update();

	  private:
		int _flags, _file_descriptor, _epoll_file_descriptor, _timeout, _timer_fd;
		size_t _max_connection_count{1024}, _max_request_size{1204 * 8};
		enum ConnState : uint8_t
		{
			HANDSHAKE,
			READING,
			PROCESSING,
			WRITING,
			CLOSED
		};
		struct Connection
		{
			int file_descriptor{-1}, write_offset{0}, requests_handled{0};
			SSL *ssl{nullptr};
			std::string read_buffer, write_buffer;
			ConnState state;
			bool keep_alive{false};
			std::chrono::steady_clock::time_point handshake_start_time, last_active;
		};
		std::set<Server::Connection *> _connections;
		std::vector<epoll_event> _epoll_pool;
		SSL_CTX *_ssl_ctx;
		Router &_router;
		void _createSocket(int port);
		void _initSocket(int backlog);
		void _handleEvents(epoll_event &);
		void _handleClientEvents(epoll_event &);
		void _setUpTls();
		void _updateEpoll(Server::Connection *, uint32_t events);
		void _resumeTlsHandshake(Connection *, epoll_event &event);
		void _writeToClient(Connection *, epoll_event &event);
		void _readFromClient(Connection *, epoll_event &event);
		void _processRequest(Connection *);
		void _clear(Server::Connection *);
	};
} // namespace sews
#endif // !SEWS_SERVER_HPP
