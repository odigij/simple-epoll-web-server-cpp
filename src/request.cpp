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

#include <request.hpp>
#include <sstream>

namespace sews {
	Request::Request(const std::string& rawRequest) {
		this->raw = rawRequest;
		std::istringstream stream(this->raw);
		stream >> method >> path >> http_version;
		std::string headerLine;
		while (std::getline(stream, headerLine) && headerLine != "\r") {
			if (headerLine.back() == '\r') {
				headerLine.pop_back();
			}
			size_t pos = headerLine.find(":");
			if (pos != std::string::npos) {
				std::string key = headerLine.substr(0, pos);
				std::string value = headerLine.substr(pos + 1);

				value.erase(0, value.find_first_not_of(" \t"));
				value.erase(value.find_last_not_of(" \t") + 1);
				headers[ key ] = value;
			}
		}
		std::stringstream bodyStream;
		bodyStream << stream.rdbuf();
		body = bodyStream.str();
	}
} // namespace sews
