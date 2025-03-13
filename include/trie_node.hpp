#ifndef SEWS_TRIE_NODE_HPP
#define SEWS_TRIE_NODE_HPP

#include <functional>
#include <request.hpp>

namespace sews {
	struct Trie {
		using Handler = std::function<std::string(
			const Request&, const std::unordered_map<std::string, std::string>&)>;
		std::unordered_map<std::string, Trie*> children;
		std::unordered_map<std::string, Handler> methods;
		std::string mime_type;

		~Trie() {
			for (auto& childPair : this->children) {
				delete childPair.second;
			}
		}
	};
} // namespace sews

#endif // !SEWS_TRIE_NODE_HPP
