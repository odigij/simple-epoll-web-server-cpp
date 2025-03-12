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

#include <response.hpp>
#include <router.hpp>
#include <sstream>

namespace sews {
	Router::Router() {
		this->root = new Trie();
	}

	Router::~Router() {
		delete this->root;
		this->root = nullptr;
	}

	void Router::split(std::vector<std::string>& parts, std::string& route) {
		std::stringstream ss(route);
		std::string segment;
		while (std::getline(ss, segment, '/')) {
			if (!segment.empty()) {
				parts.push_back(segment);
			}
		}
	}

	void Router::addRoute(std::string method, std::vector<std::string> routes,
						  Trie::Handler function) {
		for (std::string route : routes) {
			std::vector<std::string> parts;
			split(parts, route);
			Trie* node = root;
			for (const std::string& part : parts) {
				if (node->children.find(part) == node->children.end()) {
					node->children[ part ] = new Trie();
				}
				node = node->children[ part ];
			}
			node->methods[ method ] = function;
		}
	}

	// FIXME: Returns not found for both undefined route or function.
	std::string Router::handleRequest(const std::string& raw_request) {
		Request request(raw_request);
		Trie* node = root;
		std::vector<std::string> parts;
		split(parts, request.path);

		std::unordered_map<std::string, std::string> params;

		for (const std::string& part : parts) {
			if (node->children.find(part) != node->children.end()) {
				node = node->children[ part ];
			} else {
				bool foundDynamic = false;
				for (auto& childPair : node->children) {
					const std::string& key = childPair.first;
					if (!key.empty() && key[ 0 ] == ':') {
						std::string paramName = key.substr(1);
						params[ paramName ] = part;
						node = childPair.second;
						foundDynamic = true;
						break;
					}
				}
				if (!foundDynamic) {
					return Response::notFound(true);
				}
			}
		}

		if (node->methods.find(request.method) != node->methods.end()) {
			return node->methods[ request.method ](request, params);
		} else {
			return Response::notFound();
		}
	}

} // namespace sews
