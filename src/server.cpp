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

#include "../include/server.hpp"

#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace sews {
	Server::Server(Router& router) : _router(router) {
	}
	Server::~Server() {
		for (auto connection : this->_connections) {
			SSL_shutdown(connection->ssl);
			SSL_free(connection->ssl);
			epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, connection->file_descriptor,
					  nullptr);
			close(connection->file_descriptor);
		}
		close(this->_epoll_file_descriptor);
		close(this->_file_descriptor);
	}
	void Server::start(int port, int backlog) {
		this->_createSocket(port);
		this->_initSocket(backlog);
		this->_setUpTls();
		std::cout << "SEWS listens https://127.0.0.1:" << port << " | max-request: " << backlog
				  << '\n';
	}
	void Server::update(int event_poll_size) {
		struct epoll_event events[ event_poll_size ];
		// TODO
		// Get timeout through user.
		const int active_event_count =
			epoll_wait(this->_epoll_file_descriptor, &*events, event_poll_size, 3000);
		for (int index(0); index < active_event_count; index++) {
			this->_handleEvents(events[ index ]);
		}
	}
	void Server::_createSocket(int port) {
		this->_file_descriptor = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
		if (this->_file_descriptor == -1) {
			throw std::runtime_error("FATAL: Server creation failed.\n");
		}
		struct sockaddr_in address = {.sin_family = AF_INET, .sin_addr = {.s_addr = INADDR_ANY}};
		address.sin_port = htons(port);
		int opt = 1;
		if (setsockopt(this->_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			throw std::runtime_error("FATAL: setsockopt.\n");
		}
		if (bind(this->_file_descriptor, (struct sockaddr*)&address, sizeof(address)) == -1) {
			close(this->_file_descriptor);
			throw std::runtime_error("FATAL: Server binding failed.\n");
		}
	}
	void Server::_initSocket(int backlog) {
		if (listen(this->_file_descriptor, backlog) != 0) {
			close(this->_file_descriptor);
			throw std::runtime_error("FATAL: Server listen failed.\n");
		}
		this->_epoll_file_descriptor = epoll_create(1);
		if (this->_epoll_file_descriptor < 0) {
			throw std::runtime_error(strerror(errno));
		}
		struct epoll_event epollEvent = {EPOLLIN, {.fd = this->_file_descriptor}};
		if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD, this->_file_descriptor,
					  &epollEvent) != 0) {
			throw std::runtime_error("FATAL: Server epoll register failed.\n");
		}
	}
	void Server::_handleEvents(epoll_event& poll_event) {
		if (poll_event.data.fd == this->_file_descriptor) { // Server events
			if (poll_event.events & (EPOLLHUP | EPOLLERR)) {
				throw std::runtime_error("FATAL: Server socket closed unexpectedly.\n");
			} else if (poll_event.events & EPOLLIN) {
				struct sockaddr_in client_address;
				socklen_t client_address_size = sizeof(client_address);
				const int client_file_descriptor =
					accept(this->_file_descriptor, (struct sockaddr*)&client_address,
						   &client_address_size);
				if (client_file_descriptor < 0) {
					std::cerr << "LOG: Client connection failed.\n";
					return;
				}
				int flags = fcntl(client_file_descriptor, F_GETFL, 0);
				if (flags == -1 ||
					fcntl(client_file_descriptor, F_SETFL, flags | O_NONBLOCK) == -1) {
					std::cerr << "LOG: Failed to set non-blocking mode.\n";
					close(client_file_descriptor);
					return;
				}
				Server::Connection* connection = new Server::Connection();
				connection->file_descriptor = client_file_descriptor;
				connection->ssl = SSL_new(this->_ssl_ctx);
				if (!connection->ssl) {
					std::cerr << "LOG: SSL_new() failed.\n";
					close(client_file_descriptor);
					delete connection;
					return;
				}
				SSL_set_fd(connection->ssl, client_file_descriptor);
				int op, ret = SSL_accept(connection->ssl);
				epoll_event client_epoll_event;
				client_epoll_event.data.ptr = connection;
				if (ret <= 0) {
					int err = SSL_get_error(connection->ssl, ret);
					if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
						client_epoll_event.events =
							(err == SSL_ERROR_WANT_READ) ? EPOLLIN : EPOLLOUT;
						op = EPOLL_CTL_ADD;
					} else {
						std::cerr << "LOG: SSL handshake failed.\n";
						ERR_print_errors_fp(stderr);
						SSL_shutdown(connection->ssl);
						SSL_free(connection->ssl);
						close(connection->file_descriptor);
						delete connection;
						return;
					}
				}
				if (epoll_ctl(this->_epoll_file_descriptor, op, client_file_descriptor,
							  &client_epoll_event) != 0) {
					std::cerr << "LOG: Client epoll register failed.\n";
					close(client_file_descriptor);
					SSL_shutdown(connection->ssl);
					SSL_free(connection->ssl);
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
				SSL_shutdown(connection->ssl);
				SSL_free(connection->ssl);
				epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, connection->file_descriptor,
						  nullptr);
				close(connection->file_descriptor);
				this->_connections.erase(connection);
			} else if (poll_event.events & EPOLLIN) {
				std::string response = this->_handleSocketData(poll_event);
				if (response.empty()) {
					return;
				}
				int bytes_written = SSL_write(connection->ssl, response.c_str(), response.size());
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
					std::cerr << "LOG: Failed to send SSL data.\n";
					ERR_print_errors_fp(stderr);
				}
				SSL_shutdown(connection->ssl);
				SSL_free(connection->ssl);
				epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, poll_event.data.fd, nullptr);
				close(connection->file_descriptor);
				this->_connections.erase(connection);
			}
		}
	}
	void Server::_setUpTls() {
		SSL_library_init();
		SSL_load_error_strings();
		OpenSSL_add_all_algorithms();

		this->_ssl_ctx = SSL_CTX_new(TLS_server_method());
		if (!this->_ssl_ctx) {
			std::cerr << "FATAL: SSL_CTX_new() failed\n";
			exit(EXIT_FAILURE);
		}
		char cwd[ 1024 ];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			std::string certPath = std::string(cwd) + "/server.crt";
			std::string keyPath = std::string(cwd) + "/server.key";
			if (SSL_CTX_use_certificate_file(this->_ssl_ctx, certPath.c_str(), SSL_FILETYPE_PEM) <=
					0 ||
				SSL_CTX_use_PrivateKey_file(this->_ssl_ctx, keyPath.c_str(), SSL_FILETYPE_PEM) <=
					0) {
				std::cerr << "FATAL: SSL certificate or key failed to load.\n";
				ERR_print_errors_fp(stderr);
				exit(EXIT_FAILURE);
			}
		}
	}
	std::string Server::_handleSocketData(epoll_event& poll_event) {
		// TODO
		// [] Need to implement dynamic size buffer.
		// [] Implement the rest of the control flows.
		Server::Connection* connection = static_cast<Server::Connection*>(poll_event.data.ptr);
		char buffer[ 1024 * 5 ];
		ssize_t bytes_read = SSL_read(connection->ssl, buffer, sizeof(buffer) - 1);
		std::string response;
		if (bytes_read >= 0) {
			buffer[ bytes_read ] = '\0';
			return this->_router.handleRequest(buffer);
		} else if (bytes_read == 0) {
			// Unimplemented scope
		} else {
			// Unimplemented scope
		}
		return response;
	}
} // namespace sews
