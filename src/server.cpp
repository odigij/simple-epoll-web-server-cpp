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
#include <sstream>
#include <unistd.h>
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
		for (auto connection : this->_connections)
		{
			this->_clear(*connection);
		}
		sews::logger::log(sews::logger::Mode::INFO, " Shutting down the server.");
		close(this->_epoll_file_descriptor);
		close(this->_file_descriptor);
	}

	void Server::start(int port, int backlog, int timeout, int flags, int epoll_pool_size)
	{
		this->_flags = flags, this->_createSocket(port), this->_initSocket(backlog), this->_timeout = timeout,
		this->_epoll_pool.resize(epoll_pool_size);
		std::ostringstream server_info;
		server_info << "  Settings: ";
		if (this->_flags & 1)
		{
			this->_setUpTls();
		}
		sews::logger::log(sews::logger::Mode::INFO,
						  fmt::format("  Settings: {}://127.0.0.1:{} - 󰌴 backlog: {} - 󱎫 timeout: {}MS - 󰈻 "
									  "flags: {} (bitmap) - 󰘆 pool-size: {}",
									  this->_flags & 1 ? "https" : "http", port, backlog, timeout, flags,
									  epoll_pool_size));
	}

	void Server::update()
	{
		const int active_event_count = epoll_wait(this->_epoll_file_descriptor, this->_epoll_pool.data(),
												  this->_epoll_pool.size(), this->_timeout);
		for (int index(0); index < active_event_count; index++)
		{
			epoll_event &poll_event = this->_epoll_pool.at(index);
			if (this->_file_descriptor == poll_event.data.fd)
			{
				this->_handleEvents(poll_event);
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
			throw std::runtime_error("󰅙 Server creation failed.\n");
		}
		struct sockaddr_in address = {.sin_family = AF_INET, .sin_addr = {.s_addr = INADDR_ANY}};
		address.sin_port = htons(port);
		int opt = 1;
		if (setsockopt(this->_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			throw std::runtime_error("󰅙 setsockopt.\n");
		}
		if (bind(this->_file_descriptor, (struct sockaddr *)&address, sizeof(address)) == -1)
		{
			close(this->_file_descriptor);
			throw std::runtime_error("󰅙 Server binding failed.\n");
		}
		sews::logger::log(sews::logger::INFO, "󰙴 Server socket has been created successfully.");
	}

	void Server::_initSocket(int backlog)
	{
		if (listen(this->_file_descriptor, backlog) != 0)
		{
			close(this->_file_descriptor);
			throw std::runtime_error("󰅙 Server listen failed.\n");
		}
		this->_epoll_file_descriptor = epoll_create(1);
		if (this->_epoll_file_descriptor < 0)
		{
			throw std::runtime_error(strerror(errno));
		}
		struct epoll_event epollEvent = {EPOLLIN, {.fd = this->_file_descriptor}};
		if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD, this->_file_descriptor, &epollEvent) != 0)
		{
			throw std::runtime_error("󰅙  Server epoll register failed.\n");
		}
		sews::logger::log(sews::logger::INFO, "󰗠 Server socket has initialized.");
	}

	void Server::_handleEvents(epoll_event &poll_event)
	{
		if (poll_event.events & (EPOLLHUP | EPOLLERR))
		{
			throw std::runtime_error("󰅙 Server socket closed unexpectedly.\n");
		}
		else if (poll_event.events & EPOLLIN)
		{
			sews::logger::log(sews::logger::INFO, "󰛴 Server event: New connection󱇬");
			struct sockaddr_in client_address;
			socklen_t client_address_size = sizeof(client_address);
			Server::Connection *connection = new Server::Connection();
			connection->file_descriptor =
				accept(this->_file_descriptor, (struct sockaddr *)&client_address, &client_address_size);
			if (connection->file_descriptor < 0)
			{
				sews::logger::log(sews::logger::ERROR, "󱘹 Client connection failed.");
				delete connection;
				return;
			}

			int socket_flags = fcntl(connection->file_descriptor, F_GETFL, 0);
			if (socket_flags == -1 || fcntl(connection->file_descriptor, F_SETFL, socket_flags | O_NONBLOCK) == -1)
			{
				sews::logger::log(sews::logger::ERROR, "󰅙 Failed to set non-blocking mode.");
				close(connection->file_descriptor);
				delete connection;
				return;
			}

			int ret = 0;
			if (this->_flags & 1)
			{ // HTTPS Mode
				connection->ssl = SSL_new(this->_ssl_ctx);
				if (!connection->ssl)
				{
					sews::logger::log(sews::logger::ERROR, "󰅙 SSL_new() failed.");
					close(connection->file_descriptor);
					delete connection;
					return;
				}
				SSL_set_fd(connection->ssl, connection->file_descriptor);
				SSL_set_accept_state(connection->ssl);
				ret = SSL_accept(connection->ssl);
			}
			else
			{
				connection->ssl = nullptr;
			}

			epoll_event client_epoll_event;
			client_epoll_event.events = EPOLLIN;
			client_epoll_event.data.ptr = connection;

			if (this->_flags & 1 && ret <= 0)
			{
				int err = SSL_get_error(connection->ssl, ret);
				if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
				{
					client_epoll_event.events = (err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
				}
				else
				{
					sews::logger::log(sews::logger::ERROR,
									  fmt::format("󰅙 SSL handshake failed with error: {}", err));
					ERR_print_errors_fp(stderr);

					std::string msg =
						Response::text("You are trying to access via HTTP, please use HTTPS in your URL.");
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
				sews::logger::log(sews::logger::ERROR, "󰅙 Client epoll register failed.");
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

	void Server::_handleClientEvents(epoll_event &poll_event)
	{
		Server::Connection *connection = static_cast<Server::Connection *>(poll_event.data.ptr);
		if (poll_event.events & (EPOLLHUP | EPOLLERR))
		{
			this->_clear(*connection);
		}
		else if (poll_event.events & EPOLLIN)
		{
			this->_readClientSocket(poll_event);
			if (connection->buffer.empty())
			{
				return;
			}
			if (!connection->response_generated)
			{
				connection->buffer = this->_router.handleRequest(connection->buffer.data());
				connection->response_generated = true;
				connection->offset = 0;
			}
			poll_event.events = EPOLLOUT;
			epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_MOD, connection->file_descriptor, &poll_event);
		}
		else if (poll_event.events & EPOLLOUT)
		{
			if (connection->buffer.empty())
			{
				return;
			}
			char *ptr = connection->buffer.data() + connection->offset;
			int remaining = connection->buffer.size() - connection->offset;
			int bytes_written{0};
			if ((this->_flags & 1) == 0)
			{ // Plain HTTP
				bytes_written = send(connection->file_descriptor, ptr, remaining, 0);
				if (bytes_written <= 0)
				{
					sews::logger::log(sews::logger::ERROR, "󰅙 Failed to send HTML.");
				}
			}
			else
			{ // HTTPS (TLS)
				bytes_written = SSL_write(connection->ssl, ptr, remaining);
				if (bytes_written <= 0)
				{
					int err = SSL_get_error(connection->ssl, bytes_written);
					if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
					{
						poll_event.events = (err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
						epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_MOD, connection->file_descriptor,
								  &poll_event);
						return;
					}
					sews::logger::log(sews::logger::ERROR, "󰅙 Failed to send SSL data.");
					ERR_print_errors_fp(stderr);
				}
			}

			if (bytes_written > 0)
			{
				connection->offset += bytes_written;
				if (connection->offset < connection->buffer.size())
				{
					poll_event.events = EPOLLOUT;
					epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_MOD, connection->file_descriptor, &poll_event);
				}
				else
				{
					connection->buffer.clear();
					connection->offset = 0;
					this->_clear(*connection);
				}
			}
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
			sews::logger::log(sews::logger::ERROR, "󰅙 SSL_CTX_new() failed.");
			ERR_print_errors_fp(stderr);
			exit(EXIT_FAILURE);
		}
		SSL_CTX_set_cipher_list(this->_ssl_ctx, "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256");
		SSL_CTX_set_options(this->_ssl_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
		SSL_CTX_set_session_cache_mode(this->_ssl_ctx, SSL_SESS_CACHE_SERVER);
		SSL_CTX_set_options(this->_ssl_ctx, SSL_OP_NO_TICKET);
		SSL_CTX_set_info_callback(this->_ssl_ctx, [](const SSL *ssl, int where, int ret) {
			if (where & SSL_CB_HANDSHAKE_START)
			{
				sews::logger::log(sews::logger::INFO, "󰒊 SSL handshake started.");
			}
			if (where & SSL_CB_HANDSHAKE_DONE)
			{
				sews::logger::log(sews::logger::INFO, "󱅣 SSL handshake completed successfully.");
			}
			if (where & SSL_CB_ALERT)
			{
				sews::logger::log(sews::logger::ERROR,
								  fmt::format("󱈸 SSL ALERT: {}", SSL_alert_desc_string_long(ret)));
			}
		});

		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			std::string certPath = std::string(cwd) + "/server.crt";
			std::string keyPath = std::string(cwd) + "/server.key";

			if (SSL_CTX_use_certificate_file(this->_ssl_ctx, certPath.c_str(), SSL_FILETYPE_PEM) <= 0)
			{
				sews::logger::log(sews::logger::ERROR,
								  fmt::format("󰅙 Failed to load SSL certificate from: {}", certPath));
				ERR_print_errors_fp(stderr);
				return;
			}

			if (SSL_CTX_use_PrivateKey_file(this->_ssl_ctx, keyPath.c_str(), SSL_FILETYPE_PEM) <= 0)
			{
				sews::logger::log(sews::logger::ERROR,
								  fmt::format("󰅙 Failed to load SSL private key from: {}", keyPath));
				ERR_print_errors_fp(stderr);
				return;
			}

			if (!SSL_CTX_check_private_key(this->_ssl_ctx))
			{
				sews::logger::log(sews::logger::ERROR, "󰅙 Private key does not match the certificate.");
				ERR_print_errors_fp(stderr);
				return;
			}
		}
		sews::logger::log(sews::logger::INFO, "󱌢 Setup completed.");
	}

	void Server::_clear(Server::Connection &connection)
	{
		if (this->_flags & 1)
		{
			SSL_shutdown(connection.ssl);
			SSL_free(connection.ssl);
		}
		epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, connection.file_descriptor, nullptr);
		close(connection.file_descriptor);
		this->_connections.erase(&connection);
		delete &connection;
	}

	void Server::_readClientSocket(epoll_event &poll_event)
	{
		Server::Connection *connection = static_cast<Server::Connection *>(poll_event.data.ptr);
		std::vector<char> buffer(1024);
		ssize_t bytes_read = 0;
		if ((this->_flags & 1) == 0)
		{
			bytes_read = read(connection->file_descriptor, buffer.data(), buffer.size() - 1);
		}
		else
		{
			bytes_read = SSL_read(connection->ssl, buffer.data(), buffer.size() - 1);
		}
		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			sews::logger::log(sews::logger::INFO, fmt::format("Received: {}", buffer.data()));
			connection->buffer = buffer.data();
		}
		else if (bytes_read == 0)
		{
			sews::logger::log(sews::logger::INFO, "Connection closed by peer.");
			this->_clear(*connection);
			return;
		}
		else
		{
			if (this->_flags & 1)
			{
				int ssl_error = SSL_get_error(connection->ssl, bytes_read);
				if (ssl_error == SSL_ERROR_ZERO_RETURN)
				{
					sews::logger::log(sews::logger::INFO, "󰅙 SSL zero return.");
				}
				else if (ssl_error == SSL_ERROR_WANT_READ)
				{
					sews::logger::log(sews::logger::INFO, "󰑖 SSL wants more data, retrying..");
					return;
				}
				else if (ssl_error == SSL_ERROR_WANT_WRITE)
				{
					sews::logger::log(sews::logger::INFO, "󰑖 SSL wants more data, retrying..");
					return;
				}
				else if (ssl_error == SSL_ERROR_SYSCALL)
				{
					sews::logger::log(sews::logger::INFO, "󰅙 SSL system call error.");
				}
			}
			else
			{
				sews::logger::log(sews::logger::INFO, "󰅙 no socket reading.");
			}
			this->_clear(*connection);
		}
	}

} // namespace sews
