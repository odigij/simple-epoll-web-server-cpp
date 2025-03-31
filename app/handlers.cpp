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
#include <sstream>
#include "include/handlers.hpp"
#include "sews/response.hpp"

namespace app
{

	const std::string handleIndex(const sews::Request &request,
								  const std::unordered_map<std::string, std::string> &params)
	{
		std::ifstream file("../assets/public/index.html", std::ios::binary);
		std::ostringstream contentStream;
		contentStream << file.rdbuf();
		std::string content = contentStream.str();
		return content;
	}

	const std::string apiTest(const sews::Request &request, const std::unordered_map<std::string, std::string> &params)
	{
		auto it = params.find("parameter");
		std::string responseBody;
		if (it != params.end())
		{
			responseBody = "{Params: " + it->second + '}';
		}
		else
		{
			responseBody = "{Params: null}";
		}
		return sews::Response::json(responseBody);
	}

} // namespace app
