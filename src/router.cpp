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

#include "../include/router.hpp"

namespace sews {
    Router::Router() {
    }
    Router::~Router() {
    }
    void Router::addRoute(const std::string method, std::string path, HandlerFunc handler) {
        this->routes[ method + ' ' + path ] = handler;
    }
    std::string Router::handleRequest(const std::string& method, const std::string& path,
                                      const std::string& body) {
        const std::string key = method + ' ' + path;
        if (this->routes.find(key) != routes.end()) {
            return this->routes[ key ](body);
        }
        // TODO:
        // - [] Make it return a definable class as template.
        return "";
    }
} // namespace sews
