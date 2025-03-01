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

#include "../include/args.hpp"

#include <getopt.h>
#include <iostream>
#include <string>

namespace sews {
    std::tuple<int, int, int> handleArgs(int argumentCount, char* argumentVector[]) {
        int port = 8080, maximumRequest = 3, epollCount = 32, option;
        std::string message;
        static struct option longOptions[] = {
            {"port", optional_argument, nullptr, 'p'},
            {"max-request", optional_argument, nullptr, 'm'},
            {"epoll-count", optional_argument, nullptr, 'e'},
            {"help", no_argument, 0, 'h'},
            {0, no_argument, 0, 0},
        };
        while ((option = getopt_long(argumentCount, argumentVector, "p:m:e:h:", longOptions,
                                     nullptr)) != -1) {
            switch (option) {
            case 'p':
                port = optarg ? std::stoi(optarg) : port;
                message = port > 0 && port < 65535
                              ? ""
                              : "Invalid port value, 0 < val < 65535, default value is 8080.";
                break;
            case 'm':
                maximumRequest = optarg ? std::stoi(optarg) : maximumRequest;
                message = maximumRequest >= 0 ? ""
                                              : "Maximum request must be equal or "
                                                "greater than zero, default value is 3.";
                break;
            case 'e':
                epollCount = optarg ? std::stoi(optarg) : epollCount;
                message = epollCount > 0
                              ? ""
                              : "Epoll count must be greater than zero, default values is 32.";
                break;
            case 'h':
                std::cout << "sews -p <value> -m <value> -e <value>\nsews -h\nsews "
                             "--port <value> "
                             "--max-request <value> --epoll-count <value>\nsew --help\n";
                exit(EXIT_SUCCESS);
            default:
                message = "invalid argument, use --help for usage";
                break;
            }
            if (message != "") {
                std::cerr << message << ".\n";
                exit(EXIT_FAILURE);
            }
        }
        std::cout << "SEWS listens http://127.0.0.1:" << port
                  << " | max-request: " << maximumRequest << ", epoll-count: " << epollCount
                  << '\n';
        return std::make_tuple(port, maximumRequest, epollCount);
    }
} // namespace sews
