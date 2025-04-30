#ifndef SEWS_ARCHITECTURE_MESSAGE_HTTP_DISPATCH_TRIE_ROUTER_HPP
#define SEWS_ARCHITECTURE_MESSAGE_HTTP_DISPATCH_TRIE_ROUTER_HPP

#include "core/telemetry/diagnostic/transport/logger.hpp"
#include "core/message/dispatch/handler.hpp"
#include "core/message/dispatch/router.hpp"
#include <unordered_map>
#include <vector>

namespace sews::architecture::message::http::dispatch
{
	struct TrieRouter : public core::message::dispatch::Router
	{
		TrieRouter(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger);
		~TrieRouter(void) override;
		void add(std::string_view method, std::string_view path,
				 core::message::dispatch::MessageHandler *handler) override;
		void remove(std::string_view method, std::string_view path) override;
		core::message::dispatch::MessageHandler *match(const core::message::transport::Message &message) const override;

	  private:
		struct Node
		{
			std::unordered_map<std::string, std::unique_ptr<Node>> children;
			std::unordered_map<std::string, core::message::dispatch::MessageHandler *> handlers;
		};
		std::unique_ptr<Node> root;
		std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger;
		void split(std::vector<std::string> &segments, const std::string &path) const;
	};
} // namespace sews::architecture::message::http::dispatch

#endif // !SEWS_ARCHITECTURE_MESSAGE_HTTP_DISPATCH_TRIE_ROUTER_HPP
