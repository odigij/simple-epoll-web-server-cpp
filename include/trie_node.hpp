#ifndef SEWS_TRIE_NODE_HPP
#define SEWS_TRIE_NODE_HPP

#include "request.hpp"

#include <functional>
#include <string>
#include <unordered_map>

namespace sews {
	struct Trie {
		using Handler = std::function<std::string(
			const Request&, const std::unordered_map<std::string, std::string>&)>;
		std::unordered_map<std::string, Trie*> children;
		std::unordered_map<std::string, Handler> methods;

		~Trie() {
			for (auto& childPair : this->children) {
				delete childPair.second;
			}
		}
	};

} // namespace sews

#endif // !SEWS_TRIE_NODE_HPP
