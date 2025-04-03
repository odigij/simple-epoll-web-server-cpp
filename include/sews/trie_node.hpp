#ifndef SEWS_TRIE_NODE_HPP
#define SEWS_TRIE_NODE_HPP

#include <cstdint>
#include <functional>
#include "request.hpp"

namespace sews
{
	enum NodeFlags : uint8_t
	{
		NONE = 0,
		VALID = 1 << 0,
		DYNAMIC = 1 << 1,
		HAS_CHILDREN = 1 << 2,
	};

	struct Trie
	{
		using Handler =
			std::function<std::string(const Request &, const std::unordered_map<std::string, std::string> &)>;
		std::unordered_map<std::string, Trie *> children;
		std::unordered_map<std::string, Handler> methods;
		std::string mime_type;
		uint8_t flags{0};
		~Trie()
		{
			for (auto &childPair : this->children)
			{
				delete childPair.second;
			}
		}
	};
} // namespace sews

#endif // !SEWS_TRIE_NODE_HPP
