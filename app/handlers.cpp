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
#include <iostream>
#include "include/handlers.hpp"
#include "nlohmann/json_fwd.hpp"
#include "external/nlohmann/json.hpp"

namespace app
{

	const std::string handleIndex(const sews::Request &request,
								  const std::unordered_map<std::string, std::string> &params)
	{
		std::ifstream file("../assets/public/index.html", std::ios::binary);
		std::ostringstream contentStream;
		contentStream << file.rdbuf();
		return contentStream.str();
	}

	std::unordered_map<std::string, std::string> parseQueryString(const std::string &query)
	{
		std::unordered_map<std::string, std::string> result;
		std::stringstream ss(query);
		std::string pair;

		while (std::getline(ss, pair, '&'))
		{
			size_t eq = pair.find('=');
			if (eq != std::string::npos)
			{
				std::string key = pair.substr(0, eq);
				std::string value = pair.substr(eq + 1);
				result[key] = value;
			}
		}
		return result;
	}

	const std::string apiTest(const sews::Request &request, const std::unordered_map<std::string, std::string> &params)
	{
		nlohmann::json jsonResponse;
		jsonResponse["id"] = params.count("id") ? params.at("id") : nullptr;
		jsonResponse["type"] = params.count("type") ? params.at("type") : nullptr;
		return jsonResponse.dump(4);
	}

} // namespace app
