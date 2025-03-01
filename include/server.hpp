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

#include <set>
#include <string>
#include <sys/epoll.h>

namespace sews {
    class Server {
      public:
        Server();
        Server(Server&&) = default;
        Server(const Server&) = default;
        Server& operator=(Server&&) = default;
        Server& operator=(const Server&) = default;
        ~Server();
        void start(int port, int backlog);
        void update(int event_poll_size);

      private:
        int _flags, _file_descriptor, _epoll_file_descriptor;
        std::set<int> _client_file_descriptors;
        void _createSocket(int port);
        void _initSocket(int backlog);
        void _handleEvents(epoll_event& event);
        std::string _handleSocketData(epoll_event& event);
        Router router;
    };
} // namespace sews

#endif // !SEWS_SERVER_HPP
