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
#include "include/handlers.hpp"
#include "nlohmann/json_fwd.hpp"
#include "external/nlohmann/json.hpp"

namespace app
{
	const std::string handleIndex(const sews::Request &request,
								  const std::unordered_map<std::string, std::string> &params)
	{
		std::ifstream file("../assets/public/index.html", std::ios::binary);
		std::ostringstream content_stream;
		content_stream << file.rdbuf();
		return content_stream.str();
	}

	const std::string apiTest(const sews::Request &request, const std::unordered_map<std::string, std::string> &params)
	{
		auto query_params = request.query_params;
		nlohmann::json json_response;
		json_response["id"] = params.count("id") ? params.at("id") : nullptr;
		json_response["type"] = params.count("type") ? params.at("type") : nullptr;
		for (auto pair : query_params)
		{
			json_response["queries"][pair.first] = pair.second;
		}
		return json_response.dump(4);
	}
} // namespace app
