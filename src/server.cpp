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
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>

namespace sews {
	Server::Server(Router& router) : router(router) {
	}
	Server::~Server() {
		for (auto active_client_fd : this->_client_file_descriptors) {
			close(active_client_fd);
		}
		close(this->_epoll_file_descriptor);
		close(this->_file_descriptor);
	}
	void Server::start(int port, int backlog) {
		this->_createSocket(port);
		this->_initSocket(backlog);
	}
	void Server::update(int event_poll_size) {
		struct epoll_event events[ event_poll_size ];
		const int active_event_count =
			epoll_wait(this->_epoll_file_descriptor, &*events, event_poll_size, 3000);
		for (int index(0); index < active_event_count; index++) {
			this->_handleEvents(events[ index ]);
		}
	}
	void Server::_createSocket(int port) {
		this->_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
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
															// TODO
															// [] Separate EPOLLHUP & EPOLLERR
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
				if (flags == -1) {
					std::cerr << "LOG: Failed to get file flags.\n";
					close(client_file_descriptor);
					return;
				}
				if (fcntl(client_file_descriptor, F_SETFL, flags | O_NONBLOCK) == -1) {
					std::cerr << "LOG: Failed to set non-blocking mode.\n";
					close(client_file_descriptor);
					return;
				}
				epoll_event client_epoll_event;
				client_epoll_event.events = EPOLLIN;
				client_epoll_event.data.fd = client_file_descriptor;
				if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD, client_file_descriptor,
							  &client_epoll_event) != 0) {
					close(client_file_descriptor);
					std::cerr << "LOG: Client epoll register failed.\n";
					return;
				}
				this->_client_file_descriptors.insert(client_file_descriptor);
			}
		} else { // Client events
			if (poll_event.events & (EPOLLHUP | EPOLLERR)) {
				if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, poll_event.data.fd,
							  nullptr) == -1) {
					std::cerr << "LOG: Failed to remove client from epoll registry.\n";
				}
				close(poll_event.data.fd);
				this->_client_file_descriptors.erase(poll_event.data.fd);
			} else if (poll_event.events & EPOLLIN) {
				std::string response = this->_handleSocketData(poll_event);
				if (send(poll_event.data.fd, response.c_str(), response.size(), 0) == -1) {
					std::cerr << "LOG: Failed to send HTML.\n";
				} else {
					// Unimplemented scope
				}
				epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, poll_event.data.fd, nullptr);
				close(poll_event.data.fd);
				this->_client_file_descriptors.erase(poll_event.data.fd);
			}
		}
	}
	std::string Server::_handleSocketData(epoll_event& poll_event) {
		char buffer[ 1024 * 5 ];
		ssize_t bytes_read = read(poll_event.data.fd, buffer, sizeof(buffer) - 1);
		std::string response;
		// TODO
		// [] Implement the rest of the control flows.
		if (bytes_read > 0) {
			buffer[ bytes_read ] = '\0';
			return this->router.handleRequest(buffer);
		} else if (bytes_read == 0) {
			// Unimplemented scope
		} else {
			// Unimplemented scope
		}
		return response;
	}
} // namespace sews
