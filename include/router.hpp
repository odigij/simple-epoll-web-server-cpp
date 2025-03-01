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

#ifndef SEWS_ROUTER_HPP
#define SEWS_ROUTER_HPP

#include <functional>
#include <string>
#include <unordered_map>

namespace sews {

    class Router {
      public:
        Router();
        Router(Router&&) = default;
        Router(const Router&) = default;
        Router& operator=(Router&&) = default;
        Router& operator=(const Router&) = default;
        ~Router();
        using HandlerFunc = std::function<std::string(const std::string&)>;
        void addRoute(const std::string method, std::string path, HandlerFunc handler);
        std::string handleRequest(const std::string& method, const std::string& path,
                                  const std::string& body);

      private:
        std::unordered_map<std::string, HandlerFunc> routes;
    };

} // namespace sews

#endif // !SEWS_ROUTER_HPP
