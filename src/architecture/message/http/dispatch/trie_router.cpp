#include "architecture/message/http/dispatch/trie_router.hpp"
#include "architecture/message/http/transport/request.hpp"

#include <sstream>
#include <string>

namespace sews::architecture::message::http::dispatch
{
	TrieRouter::TrieRouter(std::shared_ptr<core::telemetry::diagnostic::transport::Logger> logger)
		: logger(logger), root(std::make_unique<Node>())
	{
		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO, "\033[36mTrie Router:\033[0m Initialized.");
	}

	TrieRouter::~TrieRouter(void)
	{
		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO, "\033[36mTrie Router:\033[0m Terminated.");
	}

	void TrieRouter::add(std::string_view method, std::string_view path, core::message::dispatch::Handler *handler)
	{
		// WARN:
		// - Do not store or manipulate std::string_view (method, path) directly, use safe copies.
		// NOTE:
		// - Router does not owns handlers but nodes, thats why handler is raw pointer here.
		std::string safeCopyMethod{method}, safeCopyPath{path};
		std::vector<std::string> segments{};
		split(segments, safeCopyPath);
		Node *currentNode{root.get()};
		for (std::string &segment : segments)
		{
			if (currentNode->children.find(segment) == currentNode->children.end())
			{
				currentNode->children[segment] = std::make_unique<Node>();
				// TODO:
				// - [] Need to parse query params.
			}
			currentNode = currentNode->children[segment].get();
		}
		currentNode->handlers[safeCopyMethod] = handler;
		logger->log(core::telemetry::diagnostic::logger::type::Log::INFO,
					"\033[36mTrie Router:\033[0m Registering\033[33m -> " + safeCopyMethod + ' ' + safeCopyPath);
	}

	void TrieRouter::remove(std::string_view method, std::string_view path)
	{
		// WARN:
		// - Do not store or manipulate std::string_view (method, path) directly, use safe copies.
		// NOTE:
		// - This function removes handler from the node, not the node's itself.
		// TODO:
		// - [] Need to implement a lazy node clean up for optimization.
		std::string safeCopyMethod{method}, safeCopyPath{path};
		std::vector<std::string> segments{};
		split(segments, safeCopyPath);
		Node *currentNode{root.get()};
		for (std::string &segment : segments)
		{
			if (currentNode->children.find(segment) == currentNode->children.end())
			{
				logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
							"\033[36mTrie Router:\033[0m Node does not exists to remove, cancelling ->\033[31m " +
								safeCopyMethod + ' ' + safeCopyPath + " -> " + segment);
				return;
			}
			currentNode = currentNode->children[segment].get();
		}
		currentNode->handlers.erase(safeCopyMethod);
	}

	core::message::dispatch::Handler *TrieRouter::match(const core::message::transport::Message &message) const
	{
		const http::transport::Request *request{dynamic_cast<const http::transport::Request *>(&message)};
		if (!request)
		{
			logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
						"\033[36mTrie Router:\033[0m Failed to cast message as request, returning nullptr.");
			return nullptr;
		}

		std::vector<std::string> segments{};
		split(segments, request->path);
		Node *currentNode{root.get()};

		for (std::string &segment : segments)
		{
			if (currentNode->children.find(segment) == currentNode->children.end())
			{
				logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
							"\033[36mTrie Router:\033[0m Node does not exists; \033[33m" + request->method + ' ' +
								request->path + "\033[0m. Returning nullptr.");
				return nullptr;
			}
			currentNode = currentNode->children[segment].get();
		}
		// WARN: Do not return currentNode->handlers[request->method] directly, if in any case method is nullptr then
		// operator[] would register it.
		auto it = currentNode->handlers.find(request->method);
		if (it == currentNode->handlers.end())
		{
			logger->log(core::telemetry::diagnostic::logger::type::Log::WARNING,
						"\033[36mTrie Router:\033[0m Node handler does not exists, returning nullptr.");
			return nullptr;
		}
		return it->second;
	}

	void TrieRouter::split(std::vector<std::string> &segments, const std::string &path) const
	{
		// TODO:
		// - [] Optimize it.
		std::stringstream ss(path);
		std::string segment{};
		while (std::getline(ss, segment, '/'))
		{
			if (!segment.empty())
			{
				segments.push_back(segment);
			}
		}
	}
} // namespace sews::architecture::message::http::dispatch
