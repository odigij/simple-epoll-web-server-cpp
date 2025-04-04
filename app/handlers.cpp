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

#include "include/handlers.hpp"
#include "external/nlohmann/json.hpp"
#include "external/inja/inja.hpp"

namespace app
{
	static inja::Environment env("../assets/public/");

	const std::string handleIndex(const sews::Request &request,
								  const std::unordered_map<std::string, std::string> &params)
	{
		nlohmann::json data;
		data["title"] = "SEWS - Intro";
		data["content"] = env.render_file("intro.html", data);
		return env.render_file("layout.html", data);
	}

	const std::string handleRouteVisualization(const sews::Request &request,
											   const std::unordered_map<std::string, std::string> &params)
	{
		nlohmann::json data;
		data["title"] = "SEWS - Routes";
		data["content"] = env.render_file("routes.html", data);
		return env.render_file("layout.html", data);
	}

	const std::string handleDynamicRouteTest(const sews::Request &request,
											 const std::unordered_map<std::string, std::string> &params)
	{
		nlohmann::json data;
		data["title"] = "SEWS - Dynamic Route Test";
		data["content"] = env.render_file("dynamic-route.html", data);
		return env.render_file("layout.html", data);
	}

	const std::string handleTestDynamicRoute(const sews::Request &request,
											 const std::unordered_map<std::string, std::string> &params)
	{
		auto query_params = request.query_params;
		nlohmann::json data;
		data["response"]["id"] = params.count("id") ? params.at("id") : nullptr;
		data["response"]["type"] = params.count("type") ? params.at("type") : nullptr;
		for (auto pair : query_params)
		{
			data["response"]["queries"][pair.first] = pair.second;
		}
		return data.dump(4);
	}
} // namespace app
