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
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>

namespace sews {
    Server::Server() {
    }
    Server::~Server() {
        for (auto activeClientFileDescriptor : this->_client_file_descriptors) {
            close(activeClientFileDescriptor);
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
        const int activeEventCount =
            epoll_wait(this->_epoll_file_descriptor, &*events, event_poll_size, 3000);
        for (int index(0); index < activeEventCount; index++) {
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
        if (bind(this->_file_descriptor, (struct sockaddr*)&address, sizeof(address)) != 0) {
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
    void Server::_handleEvents(epoll_event& event) {
        if (event.data.fd == this->_file_descriptor) { // Server events
            if (event.events & (EPOLLHUP | EPOLLERR)) {
                throw std::runtime_error("FATAL: Server socket closed unexpectedly.\n");
            } else if (event.events & EPOLLIN) {
                struct sockaddr_in newClientAddress;
                socklen_t newClientAddressSize = sizeof(newClientAddress);
                const int newClientFileDescriptor =
                    accept(this->_file_descriptor, (struct sockaddr*)&newClientAddress,
                           &newClientAddressSize);
                if (newClientFileDescriptor < 0) {
                    std::cerr << "LOG: Client connection failed.\n";
                    return;
                }
                int flags = fcntl(newClientFileDescriptor, F_GETFL, 0);
                if (flags == -1) {
                    std::cerr << "LOG: Failed to get file flags.\n";
                    close(newClientFileDescriptor);
                    return;
                }
                if (fcntl(newClientFileDescriptor, F_SETFL, flags | O_NONBLOCK) == -1) {
                    std::cerr << "LOG: Failed to set non-blocking mode.\n";
                    close(newClientFileDescriptor);
                    return;
                }
                epoll_event newClientEpollEvent;
                newClientEpollEvent.events = EPOLLIN;
                newClientEpollEvent.data.fd = newClientFileDescriptor;
                if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_ADD, newClientFileDescriptor,
                              &newClientEpollEvent) != 0) {
                    close(newClientFileDescriptor);
                    std::cerr << "LOG: Client epoll register failed.\n";
                    return;
                }
                this->_client_file_descriptors.insert(newClientFileDescriptor);
            }
        } else { // Client events
            if (event.events & (EPOLLHUP | EPOLLERR)) {
                if (epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, event.data.fd,
                              nullptr) == -1) {
                    std::cerr << "LOG: Failed to remove client from epoll registry.\n";
                }
                close(event.data.fd);
                this->_client_file_descriptors.erase(event.data.fd);
            } else if (event.events & EPOLLIN) {
                std::string httpResponse = this->_handleSocketData(event);
                if (send(event.data.fd, httpResponse.c_str(), httpResponse.size(), 0) == -1) {
                    std::cerr << "LOG: Failed to send HTML.\n";
                } else {
                    // Unimplemented scope
                }
                epoll_ctl(this->_epoll_file_descriptor, EPOLL_CTL_DEL, event.data.fd, nullptr);
                close(event.data.fd);
                this->_client_file_descriptors.erase(event.data.fd);
            }
        }
    }
    std::string Server::_handleSocketData(epoll_event& event) {
        char buffer[ 1024 * 2 ];
        ssize_t bytesRead = read(event.data.fd, buffer, sizeof(buffer) - 1);
        std::ostringstream responseStream;
        std::istringstream requestStream(buffer);
        std::string method, path, httpVersion, filePath, htmlContent, contentType = "text/html";
        requestStream >> method >> path >> httpVersion;
        if (bytesRead > 0) {
            buffer[ bytesRead ] = '\0';
            filePath = path == "/" || path.empty() ? "../assets/pages/index.html"
                                                   : "../assets/pages" + path;
            if (filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == ".css") {
                filePath = "../assets/styles/" + path;
                contentType = "text/css";
            }
            if (method == "GET") {
                std::ifstream file(filePath, std::ios::binary);
                if (file) {
                    htmlContent.insert(htmlContent.end(), std::istreambuf_iterator<char>(file),
                                       std::istreambuf_iterator<char>());
                    responseStream << "HTTP/1.1 200 OK\r\n"
                                   << "Content-Type: " << contentType << "\r\n"
                                   << "Content-Length: " << htmlContent.size() << "\r\n"
                                   << "\r\n"
                                   << htmlContent;
                } else {
                    htmlContent.insert(htmlContent.end(), std::istreambuf_iterator<char>(file),
                                       std::istreambuf_iterator<char>());
                    responseStream << "HTTP/1.1 404 Not Found\r\n"
                                   << "Content-Type: " << contentType << "\r\n"
                                   << "Content-Length: " << htmlContent.size() << "\r\n"
                                   << "\r\n"
                                   << htmlContent;
                }
            } else {
                std::ifstream file("../assets/pages/405.html", std::ios::binary);
                htmlContent.insert(htmlContent.end(), std::istreambuf_iterator<char>(file),
                                   std::istreambuf_iterator<char>());
                responseStream << "HTTP/1.1 405 Method Not Allowed\r\n"
                               << "Content-Type: " << contentType << "\r\n"
                               << "Content-Length: " << htmlContent.size() << "\r\n"
                               << "\r\n"
                               << htmlContent;
            }
        } else if (bytesRead == 0) {
            // Unimplemented scope
        } else {
            // Unimplemented scope
        }
        return responseStream.str();
    }
} // namespace sews
