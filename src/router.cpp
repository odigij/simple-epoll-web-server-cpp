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

#include <filesystem>
#include <fstream>
#include "nlohmann/json.hpp"
#include "sews/response.hpp"
#include "sews/trie_node.hpp"
#include "sews/router.hpp"

namespace sews
{
	Router::Router()
	{
		this->_root = new Trie();
		this->_registerStatics();
	}

	Router::~Router()
	{
		delete this->_root;
		this->_root = nullptr;
	}

	void Router::_split(std::vector<std::string> &parts, std::string &route)
	{
		std::stringstream ss(route);
		std::string segment;
		while (std::getline(ss, segment, '/'))
		{
			if (!segment.empty())
			{
				parts.push_back(segment);
			}
		}
	}

	void Router::addRoute(std::string method, std::vector<std::string> routes, Trie::Handler function,
						  std::string mime_type)
	{
		for (std::string route : routes)
		{
			std::vector<std::string> parts;
			_split(parts, route);
			Trie *node = _root;

			for (const std::string &part : parts)
			{

				if (node->children.find(part) == node->children.end())
				{
					node->children[part] = new Trie();

					if (!part.empty() && part[0] == ':')
					{
						node->children[part]->flags |= NodeFlags::DYNAMIC;
					}
				}

				node = node->children[part];
			}

			node->methods[method] = function;
			node->mime_type = mime_type;
			node->flags |= NodeFlags::VALID;
		}
	}

	std::string Router::handleRequest(const std::string &raw_request)
	{
		Request request(raw_request);
		Trie *node = this->_root;
		std::vector<std::string> parts;
		this->_split(parts, request.path);
		std::unordered_map<std::string, std::string> params;

		for (size_t i = 0; i < parts.size(); ++i)
		{
			const std::string &part = parts[i];

			if (node->children.find(part) != node->children.end())
			{
				node = node->children[part];
			}
			else
			{
				for (auto &childPair : node->children)
				{
					const std::string &key = childPair.first;
					if (!key.empty() && key[0] == ':')
					{
						std::string paramName = key.substr(1);
						params[paramName] = part;
						node = childPair.second;
						break;
					}
				}
				if (!(node->flags & NodeFlags::DYNAMIC))
				{
					return serveStaticErrorPage(404);
				}
			}
		}

		if (!(node->flags & NodeFlags::VALID))
		{
			return serveStaticErrorPage(404);
		}

		if (node->methods.find(request.method) != node->methods.end())
		{
			std::string responseBody = node->methods[request.method](request, params);
			std::string detectedMimeType = node->mime_type.empty() ? "text/plain" : node->mime_type;
			return Response::custom(responseBody, detectedMimeType, 200);
		}

		return serveStaticErrorPage(405);
	}

	std::string Router::serveStaticErrorPage(int statusCode)
	{
		std::string errorPath = "/" + std::to_string(statusCode) + ".html";
		Trie *errorNode = this->_root;
		std::vector<std::string> parts;
		this->_split(parts, errorPath);
		for (const std::string &part : parts)
		{
			if (errorNode->children.find(part) != errorNode->children.end())
			{
				errorNode = errorNode->children[part];
			}
			else
			{
				errorNode = nullptr;
				break;
			}
		}
		if (errorNode && errorNode->methods.find("GET") != errorNode->methods.end())
		{
			return Response::custom(errorNode->methods["GET"](Request("GET " + errorPath), {}), "text/html",
									statusCode);
		}
		nlohmann::json json;
		json["status-code"] = statusCode;
		return Response::custom(json.dump(), "application/json", statusCode);
	}

	const std::string Router::handleStaticFile(const sews::Request &request,
											   const std::unordered_map<std::string, std::string> &params)
	{
		std::string file_path = "../assets/public/static" + request.path;
		std::ifstream file(file_path, std::ios::binary);
		if (!file)
		{
			return serveStaticErrorPage(404);
		}
		std::ostringstream contentStream;
		contentStream << file.rdbuf();
		file.close();
		std::string mime_type = Response::getMimeType(request.path);
		if (mime_type == "text/html")
		{
			mime_type += "; charset=utf-8";
		}
		return contentStream.str();
	}

	void Router::_registerStatics()
	{
		const std::string base_dir = "../assets/public/static";
		for (const auto &entry : std::filesystem::recursive_directory_iterator(base_dir))
		{
			if (std::filesystem::is_regular_file(entry))
			{
				std::filesystem::path relative_path = std::filesystem::relative(entry.path(), base_dir);
				this->addRoute("GET", {relative_path.c_str()},
							   std::bind(&Router::handleStaticFile, this, std::placeholders::_1, std::placeholders::_2),
							   Response::getMimeType(relative_path.string()));
			}
		}
	}

	void Router::collectRoutes(Trie *node, std::string currentPath, std::vector<nlohmann::json> &routes)
	{
		for (const auto &[segment, child] : node->children)
		{
			std::string fullPath = currentPath + "/" + segment;

			if (child->flags & NodeFlags::VALID)
			{
				for (const auto &methodPair : child->methods)
				{
					nlohmann::json routeEntry;
					routeEntry["method"] = methodPair.first;
					routeEntry["path"] = fullPath;
					routeEntry["mime"] = child->mime_type;
					routeEntry["dynamic"] = bool(child->flags & NodeFlags::DYNAMIC);
					routes.push_back(routeEntry);
				}
			}

			collectRoutes(child, fullPath, routes);
		}
	}

	std::string Router::debugRouteHandler(const Request &, const std::unordered_map<std::string, std::string> &)
	{
		std::vector<nlohmann::json> routes;
		collectRoutes(this->_root, "", routes);

		nlohmann::json output;
		output["routes"] = routes;
		return output.dump();
	}

} // namespace sews
