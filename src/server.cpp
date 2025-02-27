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

#include "../include/sews.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace sews {
    ServerManager::ServerManager(int port, int requestNum, int eventSize) {
        this->config.port = port;
        this->config.requestNum = requestNum;
        this->config.eventSize = eventSize;
    }
    ServerManager::~ServerManager() {
        for (auto activeClientFileDescriptor : this->clientFileDescriptors) {
            close(activeClientFileDescriptor);
        }
        close(this->config.epollFileDescriptor);
        close(this->config.fileDescriptor);
    }
    void ServerManager::initialize() {
        int opt = 1;
        struct sockaddr_in address;
        this->config.fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (this->config.fileDescriptor == -1) {
            throw std::runtime_error("FATAL: Server creation failed.\n");
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(this->config.port);
        if (setsockopt(this->config.fileDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
            0) {
            throw std::runtime_error("FATAL: setsockopt.\n");
        }
        if (bind(this->config.fileDescriptor, (struct sockaddr*)&address, sizeof(address)) != 0) {
            close(this->config.fileDescriptor);
            throw std::runtime_error("FATAL: Server binding failed.\n");
        }
    };
    void ServerManager::run() {
        if (listen(this->config.fileDescriptor, this->config.requestNum) != 0) {
            close(this->config.fileDescriptor);
            throw std::runtime_error("FATAL: Server listen failed.\n");
        }
        this->config.epollFileDescriptor = epoll_create(1);
        if (this->config.epollFileDescriptor < 0) {
            throw std::runtime_error(strerror(errno));
        }
        struct epoll_event epollEvent;
        epollEvent.events = EPOLLIN;
        epollEvent.data.fd = this->config.fileDescriptor;
        std::cout << this->config.eventSize << ',' << this->config.epollFileDescriptor;
        if (epoll_ctl(this->config.epollFileDescriptor, EPOLL_CTL_ADD, this->config.fileDescriptor,
                      &epollEvent) != 0) {
            throw std::runtime_error("FATAL: Server epoll register failed.\n");
        }
        while (sews::SignalHandler::getSignal() == 0) {
            const int activeEventCount =
                epoll_wait(this->config.epollFileDescriptor, &*activeEpollEvents,
                           this->config.eventSize, 3000);
            for (int index(0); index < activeEventCount; index++) {
                epoll_event& currentEvent = activeEpollEvents[ index ];
                this->handleEvents(currentEvent, this->config.fileDescriptor,
                                   this->config.epollFileDescriptor, clientFileDescriptors);
            }
        }
    }
    void ServerManager::handleEvents(struct epoll_event& activeEpollEvent, int serverFileDescriptor,
                                     int serverEpollFileDescriptor,
                                     std::set<int>& clientFileDescriptors) {
        if (activeEpollEvent.data.fd == serverFileDescriptor) { // Server events
            if (activeEpollEvent.events & (EPOLLHUP | EPOLLERR)) {
                throw std::runtime_error("FATAL: Server socket closed unexpectedly.\n");
            } else if (activeEpollEvent.events & EPOLLIN) {
                struct sockaddr_in newClientAddress;
                socklen_t newClientAddressSize = sizeof(newClientAddress);
                const int newClientFileDescriptor =
                    accept(serverFileDescriptor, (struct sockaddr*)&newClientAddress,
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
                if (epoll_ctl(serverEpollFileDescriptor, EPOLL_CTL_ADD, newClientFileDescriptor,
                              &newClientEpollEvent) != 0) {
                    close(newClientFileDescriptor);
                    std::cerr << "LOG: Client epoll register failed.\n";
                    return;
                }
                clientFileDescriptors.insert(newClientFileDescriptor);
            }
        } else { // Client events
            if (activeEpollEvent.events & (EPOLLHUP | EPOLLERR)) {
                if (epoll_ctl(serverEpollFileDescriptor, EPOLL_CTL_DEL, activeEpollEvent.data.fd,
                              nullptr) == -1) {
                    std::cerr << "LOG: Failed to remove client from epoll registry.\n";
                }
                close(activeEpollEvent.data.fd);
                clientFileDescriptors.erase(activeEpollEvent.data.fd);
            } else if (activeEpollEvent.events & EPOLLIN) {
                std::string httpResponse = this->handleRequest(activeEpollEvent);
                if (send(activeEpollEvent.data.fd, httpResponse.c_str(), httpResponse.size(), 0) ==
                    -1) {
                    std::cerr << "LOG: Failed to send HTML.\n";
                } else {
                    // Unimplemented scope
                }
                epoll_ctl(serverEpollFileDescriptor, EPOLL_CTL_DEL, activeEpollEvent.data.fd,
                          nullptr);
                close(activeEpollEvent.data.fd);
                clientFileDescriptors.erase(activeEpollEvent.data.fd);
            }
        }
    };
    std::string ServerManager::handleRequest(epoll_event& epollEvent) {
        char buffer[ 1024 * 2 ];
        ssize_t bytesRead = read(epollEvent.data.fd, buffer, sizeof(buffer) - 1);
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
    };
} // namespace sews
