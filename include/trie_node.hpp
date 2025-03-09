#ifndef SEWS_TRIE_NODE_HPP
#define SEWS_TRIE_NODE_HPP

#include "request.hpp"

#include <functional>
#include <string>
#include <unordered_map>

namespace sews {
	struct Trie {
		~Trie() {
			for (auto& childPair : children) {
				delete childPair.second;
			}
		}
		using Handler = std::function<std::string(const Request&)>;
		std::unordered_map<std::string, Trie*> children;
		std::unordered_map<std::string, Handler> methods;
	};
} // namespace sews

#endif // !SEWS_TRIE_NODE_HPP
