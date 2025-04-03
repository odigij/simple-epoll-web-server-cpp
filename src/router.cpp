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

	void Router::_split(std::vector<std::string> &parts, const std::string &route)
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
		for (const std::string &route : routes)
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
		for (std::string &part : parts)
		{
			if (node->children.find(part) != node->children.end())
			{
				node = node->children[part];
			}
			else
			{
				for (const std::pair<const std::basic_string<char>, sews::Trie *> &child_pair : node->children)
				{
					const std::string &key = child_pair.first;
					if (!key.empty() && key[0] == ':')
					{
						std::string param_name = key.substr(1);
						params[param_name] = part;
						node = child_pair.second;
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
			std::string response_body = node->methods[request.method](request, params);
			std::string detected_mime_type = node->mime_type.empty() ? "text/plain" : node->mime_type;
			return Response::custom(response_body, detected_mime_type, 200);
		}
		return serveStaticErrorPage(405);
	}

	std::string Router::serveStaticErrorPage(int statusCode)
	{
		std::string error_path = "/" + std::to_string(statusCode) + ".html";
		Trie *error_node = this->_root;
		std::vector<std::string> parts;
		this->_split(parts, error_path);
		for (const std::string &part : parts)
		{
			if (error_node->children.find(part) != error_node->children.end())
			{
				error_node = error_node->children[part];
			}
			else
			{
				error_node = nullptr;
				break;
			}
		}
		if (error_node && error_node->methods.find("GET") != error_node->methods.end())
		{
			return Response::custom(error_node->methods["GET"](Request("GET " + error_path), {}), "text/html",
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
		std::ostringstream os;
		os << file.rdbuf();
		file.close();
		std::string mime_type = Response::getMimeType(request.path);
		if (mime_type == "text/html")
		{
			mime_type += "; charset=utf-8";
		}
		return os.str();
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

	void Router::collectRoutes(Trie *node, const std::string currentPath, std::vector<nlohmann::json> &routes)
	{
		for (const auto &[segment, child] : node->children)
		{
			std::string full_path = currentPath + "/" + segment;
			if (child->flags & NodeFlags::VALID)
			{
				for (const auto &method_pair : child->methods)
				{
					nlohmann::json route_entry;
					route_entry["method"] = method_pair.first;
					route_entry["path"] = full_path;
					route_entry["mime"] = child->mime_type;
					route_entry["dynamic"] = bool(child->flags & NodeFlags::DYNAMIC);
					routes.push_back(route_entry);
				}
			}
			collectRoutes(child, full_path, routes);
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
