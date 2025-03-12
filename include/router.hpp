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

#include <string>
#include <trie_node.hpp>
#include <vector>

namespace sews {
	class Router {
	  public:
		Router();
		Router(Router&&) = default;
		Router(const Router&) = default;
		Router& operator=(Router&&) = default;
		Router& operator=(const Router&) = default;
		~Router();
		void addRoute(std::string method, std::vector<std::string> routes, Trie::Handler function);
		std::string handleRequest(const std::string& raw_request);

	  private:
		Trie* root;
		void split(std::vector<std::string>& parts, std::string& route);
	};
} // namespace sews

#endif // !SEWS_ROUTER_HPP
