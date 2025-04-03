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

#include <fcntl.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "nlohmann/json.hpp"
#include "sews/response.hpp"
#include "sews/server.hpp"
#include "sews/logger.hpp"

namespace sews
{
	Server::Server(Router &router) : _router(router)
	{
	}

	Server::~Server()
	{
		sews::logger::log(sews::logger::Mode::INFO, "  Cleaning server connections..");
		auto connections_copy = this->_connections;
		for (auto connection : connections_copy)
		{
			this->_clear(connection);
		}
		sews::logger::log(sews::logger::Mode::INFO, " Shutting down the server.");
		close(this->_epoll_file_descriptor);
		close(this->_file_descriptor);
	}

	std::string getOpensslErrorString()
	{
		std::stringstream ss;
		unsigned long err;
		while ((err = ERR_get_error()) != 0)
		{
			char buf[256];
			ERR_error_string_n(err, buf, sizeof(buf));
			ss << buf << "\n";
		}
		return ss.str();
	}

	void Server::start(int port, int backlog, int timeout, int flags, int epoll_pool_size)
	{
		this->_flags = flags, this->_createSocket(port), this->_initSocket(backlog), this->_timeout = timeout,
		this->_epoll_pool.resize(epoll_pool_size);
		if (this->_flags & 1)
		{
			this->_setUpTls();
		}
		else
		{
			sews::logger::log(sews::logger::WARN, " Server runs in plain HTTP.");
		}
		sews::logger::log(sews::logger::Mode::INFO,
						  fmt::format("  Settings: {}://127.0.0.1:{} - 󱕱 backlog: {} - 󱎫 timeout: {}ms - 󰈻 "
									  "flags: {} (bitmap) - 󰘆 pool-size: {}",
									  this->_flags & 1 ? " https" : " http", port, backlog, timeout, flags,
									  epoll_pool_size));
	}

	void Server::update()
	{
		const int active_event_count = epoll_wait(this->_epoll_file_descriptor, this->_epoll_pool.data(),
												  this->_epoll_pool.size(), this->_timeout);
		for (int index(0); index < active_event_count; index++)
		{
			epoll_event &poll_event = this->_epoll_pool.at(index);
			if (poll_event.data.fd == this->_file_descriptor)
			{
				this->_handleEvents(poll_event);
				continue;
			}
			if (poll_event.data.fd == this->_timer_fd)
			{
				uint64_t expirations;
				read(this->_timer_fd, &expirations, sizeof(expirations));
				auto now(std::chrono::steady_clock::now());
				auto connections_copy(this->_connections);
				for (auto connection : connections_copy)
				{
					auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - connection->last_active);
					if (elapsed.count() > 30) // Timeout
					{
						sews::logger::log(sews::logger::INFO, "  Closing idle connection.");
						this->_clear(connection);
					}
				}
				continue;
			}
			this->_handleClientEvents(poll_event);
		}
	}

	void Server::_createSocket(int port)
	{
		this->_file_descriptor = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
		if (this->_file_descriptor == -1)
		{
			throw std::runtime_error("󱎘 Server creation failed.\n");
		}
		struct sockaddr_in address = {
			.sin_family = AF_INET,
			.sin_port = htons(port),
			.sin_addr =
				{
					.s_addr = INADDR_ANY,
				},
		};
		int opt = 1;
		if (setsockopt(this->_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			throw std::runtime_error("󱎘 setsockopt.\n");
		}
		if (bind(this->_file_descriptor, (struct sockaddr *)&address, sizeof(address)) == -1)
		{
			close(this->_file_descriptor);
			throw std::runtime_error("󱎘 Server binding failed.\n");
		}
		sews::logger::log(sews::logger::INFO, "󰒋 Server socket has been created successfully.");
	}

	void Server::_initSocket(int backlog)
	{
		if (listen(this->_file_descriptor, backlog) != 0)
		{
			close(this->_file_descriptor);
			throw std::runtime_error("󱎘 Server listen failed.\n");
		}
		this->_epoll_file_descriptor = epoll_create(1);
		if (this->_epoll_file_descriptor < 0)
		{
			throw std::runtime_error(strerror(errno));
		}
		this->_timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
		if (this->_timer_fd == -1)
		{
			throw std::runtime_error("󱎘 Failed to create timerfd");
		}
		// TODO: Configurable timeout
		itimerspec timeout{
			.it_interval =
				{
					.tv_sec = 5,
					.tv_nsec = 0,
				},
			.it_value =
				{
					.tv_sec = 5, // First expiration
					.tv_nsec = 0,
				},
		};
		if (timerfd_settime(this->_timer_fd, 0, &timeout, nullptr) == -1)
		{
			throw std::runtime_error("󱎘 Failed to set timerfd time");
		}
		epoll_event timer_epoll_event = {.events = EPOLLIN, .data = {.fd = this->_timer_fd}};
		if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD, this->_timer_fd, &timer_epoll_event) != 0)
		{
			throw std::runtime_error("󱎘 Failed to add timerfd to epoll");
		}
		struct epoll_event epoll_event = {EPOLLIN, {.fd = this->_file_descriptor}};
		if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD, this->_file_descriptor, &epoll_event) != 0)
		{
			throw std::runtime_error("󱎘 Server epoll register failed.\n");
		}
		sews::logger::log(sews::logger::INFO, " Server socket has initialized.");
	}

	void Server::_handleEvents(epoll_event &poll_event)
	{
		if (poll_event.events & (EPOLLHUP | EPOLLERR))
		{
			throw std::runtime_error("󱎘 Server socket closed unexpectedly.\n");
		}
		else if (poll_event.events & EPOLLIN)
		{
			if (this->_connections.size() >= this->_max_connection_count)
			{
				sews::logger::log(sews::logger::WARN, " Too many open connections, rejecting new client.");
				return;
			}
			sews::logger::log(sews::logger::INFO, " Server event: New client connection.");
			struct sockaddr_in client_address;
			socklen_t client_address_size(sizeof(client_address));
			Server::Connection *connection(new Server::Connection());
			connection->file_descriptor =
				accept(this->_file_descriptor, (struct sockaddr *)&client_address, &client_address_size);
			if (connection->file_descriptor < 0)
			{
				sews::logger::log(sews::logger::ERROR, " Server event: Client connection failed.");
				delete connection;
				return;
			}
			int socket_flags(fcntl(connection->file_descriptor, F_GETFL, 0));
			if (socket_flags == -1 || fcntl(connection->file_descriptor, F_SETFL, socket_flags | O_NONBLOCK) == -1)
			{
				sews::logger::log(sews::logger::ERROR, "󱎘 Failed to set non-blocking mode.");
				close(connection->file_descriptor);
				delete connection;
				return;
			}
			int ret = 0;
			if (this->_flags & 1) // HTTPS Mode
			{
				connection->ssl = SSL_new(this->_ssl_ctx);
				if (!connection->ssl)
				{
					sews::logger::log(sews::logger::ERROR, "󱎘 Initializing a new SSL structure has failed.");
					close(connection->file_descriptor);
					delete connection;
					return;
				}
				SSL_set_fd(connection->ssl, connection->file_descriptor);
				SSL_set_accept_state(connection->ssl);
				ret = SSL_accept(connection->ssl);
			}
			else // HTTP Mode
			{
				connection->ssl = nullptr;
				connection->state = ConnState::READING;
			}
			epoll_event client_epoll_event{.events = EPOLLIN, .data = {.ptr = connection}};
			if (this->_flags & 1 && ret <= 0)
			{
				int err(SSL_get_error(connection->ssl, ret));
				if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
				{
					client_epoll_event.events = (err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
					connection->state = ConnState::HANDSHAKE;
				}
				else
				{
					sews::logger::log(sews::logger::ERROR, fmt::format("󱎘 SSL handshake failed with error: {}\n{}",
																	   err, getOpensslErrorString()));
					nlohmann::json response;
					response["status"] = 505;
					response["message"] = "You are trying to access via HTTP, please use HTTPS in your URI.";
					std::string msg(Response::custom(response.dump(), "application/json", 505));
					send(connection->file_descriptor, msg.data(), msg.size(), 0);
					SSL_shutdown(connection->ssl);
					SSL_free(connection->ssl);
					close(connection->file_descriptor);
					delete connection;
					return;
				}
			}
			if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD, connection->file_descriptor,
						  &client_epoll_event) != 0)
			{
				sews::logger::log(sews::logger::ERROR, "󱎘 Client epoll register failed.");
				if (this->_flags & 1)
				{
					SSL_shutdown(connection->ssl);
					SSL_free(connection->ssl);
				}
				close(connection->file_descriptor);
				delete connection;
				return;
			}
			this->_connections.insert(connection);
		}
	}

	void Server::_handleClientEvents(epoll_event &event)
	{
		Connection *conn = static_cast<Connection *>(event.data.ptr);
		if (event.events & (EPOLLHUP | EPOLLERR))
		{
			this->_clear(conn);
			return;
		}
		switch (conn->state)
		{
			case ConnState::HANDSHAKE:
				{
					auto now(std::chrono::steady_clock::now());
					if (conn->handshake_start_time == std::chrono::steady_clock::time_point{})
					{
						conn->handshake_start_time = now;
					}
					else
					{
						auto elapsed =
							std::chrono::duration_cast<std::chrono::seconds>(now - conn->handshake_start_time);
						// TODO: Configurable hanshake timeout
						if (elapsed.count() > 5) // 5s max for handshake
						{
							sews::logger::log(sews::logger::WARN, " TLS handshake timeout.");
							this->_clear(conn);
							return;
						}
					}
					this->_resumeTlsHandshake(conn, event);
				}
				break;
			case ConnState::READING:
				{
					this->_readFromClient(conn, event);
				}
				break;
			case ConnState::PROCESSING:
				{
					this->_processRequest(conn);
				}
				break;
			case ConnState::WRITING:
				{
					this->_writeToClient(conn, event);
				}
				break;
			case ConnState::CLOSED:
				{
					this->_clear(conn);
				}
				break;
		}
	}

	void Server::_setUpTls()
	{
		sews::logger::log(sews::logger::INFO, "󱌢 Setting up TLS..");
		static bool initialized = false;
		if (!initialized)
		{
			SSL_library_init();
			SSL_load_error_strings();
			OpenSSL_add_all_algorithms();
			initialized = true;
		}
		this->_ssl_ctx = SSL_CTX_new(TLS_server_method());
		if (!this->_ssl_ctx)
		{
			sews::logger::log(sews::logger::ERROR,
							  fmt::format("󱎘 Failed to initialize SSL context: {}", getOpensslErrorString()));
			exit(EXIT_FAILURE);
		}
		// TODO: Configurable SSL
		SSL_CTX_set_cipher_list(this->_ssl_ctx, "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256");
		SSL_CTX_set_options(this->_ssl_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
		SSL_CTX_set_session_cache_mode(this->_ssl_ctx, SSL_SESS_CACHE_SERVER);
		SSL_CTX_set_options(this->_ssl_ctx, SSL_OP_NO_TICKET);
		SSL_CTX_set_info_callback(this->_ssl_ctx, [](const SSL *ssl, int where, int ret) {
			if (where & SSL_CB_HANDSHAKE_START)
			{
				sews::logger::log(sews::logger::INFO, " SSL handshake started.");
			}
			if (where & SSL_CB_HANDSHAKE_DONE)
			{
				sews::logger::log(sews::logger::INFO, " SSL handshake completed successfully.");
			}
			if (where & SSL_CB_ALERT)
			{
				const char *desc = SSL_alert_desc_string_long(ret);
				if ((ret & 0xff) == SSL3_AD_CLOSE_NOTIFY)
				{
					sews::logger::log(sews::logger::INFO, fmt::format("󱎘 TLS close_notify received from client."));
				}
				else
				{
					sews::logger::log(sews::logger::ERROR, fmt::format("󱎘 SSL ALERT: {}", desc));
				}
			}
		});
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			std::string certPath = std::string(cwd) + "/server.crt";
			std::string keyPath = std::string(cwd) + "/server.key";
			if (SSL_CTX_use_certificate_file(this->_ssl_ctx, certPath.c_str(), SSL_FILETYPE_PEM) <= 0)
			{
				sews::logger::log(sews::logger::ERROR, fmt::format("󱎘 Failed to load SSL certificate from: {}\n{}",
																   certPath, getOpensslErrorString()));
				return;
			}
			if (SSL_CTX_use_PrivateKey_file(this->_ssl_ctx, keyPath.c_str(), SSL_FILETYPE_PEM) <= 0)
			{
				sews::logger::log(sews::logger::ERROR, fmt::format("󱎘 Failed to load SSL private key from: {}/n{}",
																   keyPath, getOpensslErrorString()));
				return;
			}
			if (!SSL_CTX_check_private_key(this->_ssl_ctx))
			{
				sews::logger::log(
					sews::logger::ERROR,
					fmt::format("󱎘 Private key does not match the certificate: {}", getOpensslErrorString()));
				return;
			}
		}
		sews::logger::log(sews::logger::INFO, "  Setup completed.");
	}

	void Server::_updateEpoll(Server::Connection *conn, uint32_t events)
	{
		epoll_event ev({.events = events | EPOLLET, .data = {.ptr = conn}});
		if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_MOD, conn->file_descriptor, &ev) != 0)
		{
			sews::logger::log(sews::logger::ERROR, "󱎘 Failed to modify epoll event.");
			this->_clear(conn);
		}
	}

	void Server::_resumeTlsHandshake(Connection *conn, epoll_event &event)
	{
		int ret(SSL_accept(conn->ssl));
		if (ret == 1)
		{
			conn->state = ConnState::READING;
			this->_updateEpoll(conn, EPOLLIN);
			return;
		}
		int err = SSL_get_error(conn->ssl, ret);
		if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
		{
			uint32_t wait_event = (err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
			this->_updateEpoll(conn, wait_event);
			return;
		}
		sews::logger::log(sews::logger::ERROR, fmt::format("󱎘 SSL handshake failed: {}", getOpensslErrorString()));
		this->_clear(conn);
	}

	void Server::_writeToClient(Connection *connection, epoll_event &event)
	{
		size_t remaining(connection->write_buffer.size() - connection->write_offset), bytes_written(0);
		const char *data(connection->write_buffer.data() + connection->write_offset);
		connection->last_active = std::chrono::steady_clock::now();
		if (!connection->ssl)
		{
			bytes_written = send(connection->file_descriptor, data, remaining, 0);
		}
		else
		{
			bytes_written = SSL_write(connection->ssl, data, remaining);
			int err = SSL_get_error(connection->ssl, bytes_written);
			if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
			{
				uint32_t wait_event = (err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
				this->_updateEpoll(connection, wait_event);
				return;
			}
			else if (bytes_written <= 0)
			{
				sews::logger::log(sews::logger::ERROR,
								  fmt::format("󱎘 SSL write failed: {}", getOpensslErrorString()));
				this->_clear(connection);
				return;
			}
		}
		if (bytes_written <= 0)
		{
			sews::logger::log(sews::logger::ERROR, "󱎘 Failed to write to client.");
			this->_clear(connection);
			return;
		}
		connection->write_offset += bytes_written;
		if (connection->write_offset < connection->write_buffer.size())
		{
			this->_updateEpoll(connection, EPOLLOUT);
		}
		else
		{
			connection->write_buffer.clear();
			connection->write_offset = 0;
			if (connection->keep_alive && connection->requests_handled < 100)
			{
				connection->state = ConnState::READING;
				this->_updateEpoll(connection, EPOLLIN);
			}
			else
			{
				connection->state = ConnState::CLOSED;
				this->_clear(connection);
			}
		}
	}

	void Server::_readFromClient(Connection *connection, epoll_event &event)
	{
		if (connection->read_buffer.size() > this->_max_request_size)
		{
			sews::logger::log(sews::logger::WARN, "󱎘 Request too large, closing connection.");
			this->_clear(connection);
			return;
		}
		char buf[4096];
		ssize_t bytes_read(0);
		connection->last_active = std::chrono::steady_clock::now();
		if (!connection->ssl) // Plain HTTP
		{
			bytes_read = recv(connection->file_descriptor, buf, sizeof(buf), 0);
		}
		else // TLS
		{
			bytes_read = SSL_read(connection->ssl, buf, sizeof(buf));
			int err = SSL_get_error(connection->ssl, bytes_read);

			if (err == SSL_ERROR_ZERO_RETURN)
			{
				this->_clear(connection);
				return;
			}
			else if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
			{
				uint32_t wait_event = (err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
				this->_updateEpoll(connection, wait_event);
				return;
			}
			else if (bytes_read <= 0)
			{
				sews::logger::log(sews::logger::ERROR,
								  fmt::format("󱎘 SSL read failed: {}", getOpensslErrorString()));
				this->_clear(connection);
				return;
			}
		}
		if (bytes_read <= 0)
		{
			sews::logger::log(sews::logger::INFO, " Client closed connection.");
			this->_clear(connection);
			return;
		}
		connection->read_buffer.append(buf, bytes_read);
		connection->state = ConnState::PROCESSING;
		this->_handleClientEvents(event);
	}

	void Server::_processRequest(Connection *conn)
	{
		sews::Request request(conn->read_buffer);
		sews::logger::log(sews::logger::INFO, fmt::format("Request: {}", request.raw));
		auto it(request.headers.find("Connection"));
		if (it != request.headers.end())
		{
			std::string connection_type = it->second;
			std::transform(connection_type.begin(), connection_type.end(), connection_type.begin(), ::tolower);
			conn->keep_alive = (connection_type == "keep-alive");
		}
		else if (request.http_version == "HTTP/1.1")
		{
			conn->keep_alive = true;
		}
		else
		{
			conn->keep_alive = false;
		}
		conn->write_buffer = this->_router.handleRequest(request.raw);
		conn->write_offset = 0;
		conn->read_buffer.clear();
		conn->requests_handled++;
		conn->state = ConnState::WRITING;
		this->_updateEpoll(conn, EPOLLOUT);
	}

	void Server::_clear(Server::Connection *connection)
	{
		if ((this->_flags & 1) && connection->ssl)
		{
			SSL_shutdown(connection->ssl);
			SSL_free(connection->ssl);
		}
		epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, connection->file_descriptor, nullptr);
		close(connection->file_descriptor);
		this->_connections.erase(connection);
		delete connection;
	}
} // namespace sews
