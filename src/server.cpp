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
#include <iostream>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <response.hpp>
#include <server.hpp>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace sews {
	Server::Server(Router& router) : _router(router) {
	}

	Server::~Server() {
		std::cout << "󰩹 Cleaning server connections..\n";
		for (auto connection : this->_connections) {
			if (this->_flags & 1) {
				SSL_shutdown(connection->ssl);
				SSL_free(connection->ssl);
			}
			epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, connection->file_descriptor,
					  nullptr);
			close(connection->file_descriptor);
		}
		std::cout << "󰒏 Shutting down the server.\n";
		close(this->_epoll_file_descriptor);
		close(this->_file_descriptor);
	}

	void Server::start(int port, int backlog, int timeout, int flags, int epoll_pool_size) {
		this->_flags = flags, this->_createSocket(port), this->_initSocket(backlog),
		this->_timeout = timeout, this->_epoll_pool.resize(epoll_pool_size);
		std::ostringstream server_info;
		server_info << "󰣖 Settings: ";
		if (this->_flags & 1) {
			server_info << "󰒃 https";
			this->_setUpTls();
		} else {
			server_info << "http";
		}
		server_info << "://127.0.0.1:" << port << " - 󰌴 backlog: " << backlog
					<< " - 󱎫 timeout: " << timeout << "MS - 󰈻 flags: " << flags
					<< "(bitmap) - 󰘆 pool-size: " << epoll_pool_size << "\n\n";
		std::cout << server_info.str();
	}

	void Server::update() {
		const int active_event_count =
			epoll_wait(this->_epoll_file_descriptor, this->_epoll_pool.data(),
					   this->_epoll_pool.size(), this->_timeout);
		for (int index(0); index < active_event_count; index++) {
			this->_handleEvents(this->_epoll_pool.at(index));
		}
	}

	void Server::_createSocket(int port) {
		this->_file_descriptor = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
		if (this->_file_descriptor == -1) {
			throw std::runtime_error("󰅙 Server creation failed.\n");
		}
		struct sockaddr_in address = {.sin_family = AF_INET, .sin_addr = {.s_addr = INADDR_ANY}};
		address.sin_port = htons(port);
		int opt = 1;
		if (setsockopt(this->_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			throw std::runtime_error("󰅙 setsockopt.\n");
		}
		if (bind(this->_file_descriptor, (struct sockaddr*)&address, sizeof(address)) == -1) {
			close(this->_file_descriptor);
			throw std::runtime_error("󰅙 Server binding failed.\n");
		}
		std::cout << "󰙴 Server socket has been created successfully.\n";
	}

	void Server::_initSocket(int backlog) {
		if (listen(this->_file_descriptor, backlog) != 0) {
			close(this->_file_descriptor);
			throw std::runtime_error("󰅙 Server listen failed.\n");
		}
		this->_epoll_file_descriptor = epoll_create(1);
		if (this->_epoll_file_descriptor < 0) {
			throw std::runtime_error(strerror(errno));
		}
		struct epoll_event epollEvent = {EPOLLIN, {.fd = this->_file_descriptor}};
		if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD, this->_file_descriptor,
					  &epollEvent) != 0) {
			throw std::runtime_error("󰅙  Server epoll register failed.\n");
		}
		std::cout << "󰗠 Server socket has initialized.\n";
	}

	void Server::_handleEvents(epoll_event& poll_event) {
		if (poll_event.data.fd == this->_file_descriptor) { // Server events
			std::cout << "󰛴 Server event: ";
			if (poll_event.events & (EPOLLHUP | EPOLLERR)) {
				throw std::runtime_error("󰅙 Server socket closed unexpectedly.\n");
			} else if (poll_event.events & EPOLLIN) {
				std::cout << "󱇬 New connection.\n";
				struct sockaddr_in client_address;
				socklen_t client_address_size = sizeof(client_address);
				Server::Connection* connection = new Server::Connection();
				connection->file_descriptor =
					accept(this->_file_descriptor, (struct sockaddr*)&client_address,
						   &client_address_size);
				if (connection->file_descriptor < 0) {
					std::cerr << "󱘹 Client connection failed.\n";
					delete connection;
					return;
				}

				int socket_flags = fcntl(connection->file_descriptor, F_GETFL, 0);
				if (socket_flags == -1 ||
					fcntl(connection->file_descriptor, F_SETFL, socket_flags | O_NONBLOCK) == -1) {
					std::cerr << "󰅙 Failed to set non-blocking mode.\n";
					close(connection->file_descriptor);
					delete connection;
					return;
				}

				int ret = 0;
				if (this->_flags & 1) { // HTTPS Mode
					connection->ssl = SSL_new(this->_ssl_ctx);
					if (!connection->ssl) {
						std::cerr << "󰅙 SSL_new() failed.\n";
						close(connection->file_descriptor);
						delete connection;
						return;
					}
					SSL_set_fd(connection->ssl, connection->file_descriptor);
					SSL_set_accept_state(connection->ssl);
					ret = SSL_accept(connection->ssl);
				} else {
					connection->ssl = nullptr;
				}

				epoll_event client_epoll_event;
				client_epoll_event.events = EPOLLIN;
				client_epoll_event.data.ptr = connection;

				if (this->_flags & 1 && ret <= 0) {
					int err = SSL_get_error(connection->ssl, ret);
					if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
						client_epoll_event.events =
							(err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
					} else {
						std::cerr << "󰅙 SSL handshake failed with error: " << err << "\n";
						ERR_print_errors_fp(stderr);

						std::string msg = Response::text(
							"You are trying to access via HTTP, please use HTTPS in your URL.");
						send(connection->file_descriptor, msg.data(), msg.size(), 0);

						SSL_shutdown(connection->ssl);
						SSL_free(connection->ssl);
						close(connection->file_descriptor);
						delete connection;
						return;
					}
				}

				if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD,
							  connection->file_descriptor, &client_epoll_event) != 0) {
					std::cerr << "󰅙 Client epoll register failed.\n";
					close(connection->file_descriptor);
					if (this->_flags & 1) {
						SSL_shutdown(connection->ssl);
						SSL_free(connection->ssl);
					}
					delete connection;
					return;
				}
				this->_connections.insert(connection);
			}
		} else { // Client events
			Server::Connection* connection = static_cast<Server::Connection*>(poll_event.data.ptr);
			if (poll_event.events & (EPOLLHUP | EPOLLERR)) {
				epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, connection->file_descriptor,
						  nullptr);
				if (this->_flags & 1) {
					SSL_shutdown(connection->ssl);
					SSL_free(connection->ssl);
				}
				close(connection->file_descriptor);
				this->_connections.erase(connection);
				delete connection;
			} else if (poll_event.events & EPOLLIN) {
				std::string response = this->_handleSocketData(poll_event);
				if (response.empty()) {
					return;
				}
				if (this->_flags & 1) {
					int bytes_written =
						SSL_write(connection->ssl, response.c_str(), response.size());
					if (bytes_written <= 0) {
						int err = SSL_get_error(connection->ssl, bytes_written);
						if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
							epoll_event client_epoll_event;
							client_epoll_event.data.ptr = connection;
							client_epoll_event.events =
								(err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
							epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_MOD,
									  connection->file_descriptor, &client_epoll_event);
							return;
						}
						std::cerr << "󰅙 Failed to send SSL data.\n";
						ERR_print_errors_fp(stderr);
					}
					SSL_shutdown(connection->ssl);
					SSL_free(connection->ssl);
				} else {
					if (send(connection->file_descriptor, response.c_str(), response.size(), 0) ==
						-1) {
						std::cerr << "󰅙 Failed to send HTML.\n";
					}
				}
				epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, poll_event.data.fd, nullptr);
				close(connection->file_descriptor);
				this->_connections.erase(connection);
				delete connection;
			}
		}
	}

	void Server::_setUpTls() {
		std::cout << "󱌢 Setting up TLS..\n";
		static bool initialized = false;
		if (!initialized) {
			SSL_library_init();
			SSL_load_error_strings();
			OpenSSL_add_all_algorithms();
			initialized = true;
		}
		this->_ssl_ctx = SSL_CTX_new(TLS_server_method());
		if (!this->_ssl_ctx) {
			std::cerr << "󰅙 SSL_CTX_new() failed\n";
			ERR_print_errors_fp(stderr);
			exit(EXIT_FAILURE);
		}

		SSL_CTX_set_cipher_list(this->_ssl_ctx, "HIGH:!aNULL:!MD5");
		SSL_CTX_set_options(this->_ssl_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 |
												SSL_OP_NO_TLSv1_1);
		SSL_CTX_set_session_cache_mode(this->_ssl_ctx, SSL_SESS_CACHE_SERVER);
		SSL_CTX_set_options(this->_ssl_ctx, SSL_OP_NO_TICKET);
		SSL_CTX_set_info_callback(this->_ssl_ctx, [](const SSL* ssl, int where, int ret) {
			if (where & SSL_CB_HANDSHAKE_START) {
				std::cout << "󰒊 SSL handshake started.\n";
			}
			if (where & SSL_CB_HANDSHAKE_DONE) {
				std::cout << "󱅣 SSL handshake completed successfully.\n";
			}
			if (where & SSL_CB_ALERT) {
				std::cerr << "󱈸 SSL ALERT: " << SSL_alert_desc_string_long(ret) << "\n";
			}
		});

		char cwd[ 1024 ];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			std::string certPath = std::string(cwd) + "/server.crt";
			std::string keyPath = std::string(cwd) + "/server.key";

			if (SSL_CTX_use_certificate_file(this->_ssl_ctx, certPath.c_str(), SSL_FILETYPE_PEM) <=
				0) {
				std::cerr << "󰅙 Failed to load SSL certificate from: " << certPath << "\n";
				ERR_print_errors_fp(stderr);
				return;
			}

			if (SSL_CTX_use_PrivateKey_file(this->_ssl_ctx, keyPath.c_str(), SSL_FILETYPE_PEM) <=
				0) {
				std::cerr << "󰅙 Failed to load SSL private key from: " << keyPath << "\n";
				ERR_print_errors_fp(stderr);
				return;
			}

			if (!SSL_CTX_check_private_key(this->_ssl_ctx)) {
				std::cerr << "󰅙 Private key does not match the certificate\n";
				ERR_print_errors_fp(stderr);
				return;
			}
		}

		std::cout << "󱌢 Setup completed.\n";
	}

	std::string Server::_handleSocketData(epoll_event& poll_event) {
		Server::Connection* connection = static_cast<Server::Connection*>(poll_event.data.ptr);
		std::vector<char> buffer(1024 * 10);
		ssize_t bytes_read;

		if ((this->_flags & 1) == 1) {
			// TLS Mode (SSL_read)
			bytes_read = SSL_read(connection->ssl, buffer.data(), buffer.size() - 1);
			if (bytes_read <= 0) {
				int ssl_error = SSL_get_error(connection->ssl, bytes_read);
				switch (ssl_error) {
				case SSL_ERROR_ZERO_RETURN:
					std::cout << "󰅙 SSL zero return.\n";
					return "Connection closed.";
				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE:
					std::cout << "󰑖 󰒄 SSL wants more data, retrying..\n";
					return "";
				case SSL_ERROR_SYSCALL:
					std::cout << "󰅙 SSL system call error.\n";
					return "SSL system call error.";
				default:
					std::cout << "󰅙 Server SSL error.\n";
					return "Server SSL error.";
				}
			}
		} else {
			// Plain HTTP Mode
			bytes_read = read(connection->file_descriptor, buffer.data(), buffer.size() - 1);
		}
		std::string response("");
		if (bytes_read > 0) {
			buffer[ bytes_read ] = '\0';
			std::cout << "󰇚 " << buffer.data() << "\n";
			response = this->_router.handleRequest(std::string(buffer.data(), bytes_read));
		} else if (bytes_read == 0) {
			std::cout << "󰅙 No socket reading.\n";
			response = "No socket reading.";
		} else {
			std::cout << "󰅙 Server socket reading error.\n";
			response = "Server socket readin error.";
		}

		return response;
	}
} // namespace sews
