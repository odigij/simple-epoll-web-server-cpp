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

#include <sstream>
#include "sews/request.hpp"

namespace sews
{
	Request::Request(const std::string &rawRequest)
	{
		this->raw = rawRequest;
		std::istringstream stream(this->raw);
		stream >> method >> path >> http_version;

		size_t query_start_position = path.find('?');
		if (query_start_position != std::string::npos)
		{
			this->query_string = path.substr(query_start_position + 1);
			path = path.substr(0, query_start_position);
			this->query_params = this->parseQueryString(this->query_string);
		}

		std::string header_line;
		while (std::getline(stream, header_line) && header_line != "\r")
		{
			if (!header_line.empty() && header_line.back() == '\r')
				header_line.pop_back();

			size_t pos = header_line.find(":");
			if (pos != std::string::npos)
			{
				std::string key = header_line.substr(0, pos);
				std::string value = header_line.substr(pos + 1);
				value.erase(0, value.find_first_not_of(" \t"));
				value.erase(value.find_last_not_of(" \t") + 1);
				headers[key] = value;
			}
		}

		std::stringstream body_stream;
		body_stream << stream.rdbuf();
		body = body_stream.str();
	}

	std::unordered_map<std::string, std::string> sews::Request::parseQueryString(const std::string &query)
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

} // namespace sews
