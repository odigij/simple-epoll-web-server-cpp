#ifndef SEWS_INFRASTRUCTURE_HTTP_ROUTER_TRIE_HPP
#define SEWS_INFRASTRUCTURE_HTTP_ROUTER_TRIE_HPP

#include "sews/core/interface/logger.hpp"
#include "sews/core/interface/message_handler.hpp"
#include "sews/core/interface/router.hpp"
#include <unordered_map>
#include <vector>

namespace sews::format::http
{
	struct TrieRouter : public interface::Router
	{
		TrieRouter(interface::Logger *logger);
		~TrieRouter(void) override;
		void add(std::string_view method, std::string_view path, interface::MessageHandler *handler) override;
		void remove(std::string_view method, std::string_view path) override;
		interface::MessageHandler *match(const interface::Message &message) const override;

	  private:
		struct Node
		{
			std::unordered_map<std::string, std::unique_ptr<Node>> children;
			std::unordered_map<std::string, interface::MessageHandler *> handlers;
		};
		std::unique_ptr<Node> root;
		interface::Logger *logger;
		void split(std::vector<std::string> &segments, const std::string &path) const;
	};
} // namespace sews::format::http

#endif // !SEWS_INFRASTRUCTURE_HTTP_ROUTER_TRIE_HPP
