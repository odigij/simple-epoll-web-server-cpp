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

#ifndef SEWS_HPP
#define SEWS_HPP

#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <getopt.h>
#include <netinet/in.h>
#include <set>
#include <string>
#include <sys/epoll.h>

namespace sews {
    std::tuple<int, int, int> handleArgs(int argumentCount, char* argumentVector[]);
    class Router {
      public:
        Router();
        Router(Router&&) = default;
        Router(const Router&) = default;
        Router& operator=(Router&&) = default;
        Router& operator=(const Router&) = default;
        ~Router();
        using HandlerFunc = std::function<std::string(const std::string&)>;
        void addRoute(const std::string& path, HandlerFunc handler);

      private:
        std::unordered_map<std::string, HandlerFunc> routes;
    };
    class SignalHandler {
      public:
        static void init();
        static void handle(int signal);
        static int getSignal();

      private:
        static volatile sig_atomic_t flags;
    };
    class ServerManager {
      public:
        ServerManager(int port, int requestNum, int eventSize);
        ServerManager(ServerManager&&) = default;
        ServerManager(const ServerManager&) = default;
        ServerManager& operator=(ServerManager&&) = default;
        ServerManager& operator=(const ServerManager&) = default;
        ~ServerManager();
        void initialize();
        void run();

      private:
        struct {
            int port;
            int requestNum;
            int fileDescriptor;
            int epollFileDescriptor;
            int eventSize;
        } config;
        std::set<int> clientFileDescriptors;
        // std::vector<Router> routers;
        struct epoll_event activeEpollEvents[];
        void handleEvents(struct epoll_event& activeEpollEvent, int serverFileDescriptor,
                          int serverEpollFileDescriptor, std::set<int>& clientFileDescriptors);
        std::string handleRequest(epoll_event& epollEvent);
    };
} // namespace sews

#endif // !SEWS_HPP
