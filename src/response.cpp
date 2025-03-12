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
#include <response.hpp>
#include <sstream>

namespace sews {
	std::string Response::ok(const std::string& body, const std::string& contentType) {
		return "HTTP/1.1 200 OK\r\nContent-Type: " + contentType +
			   "\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
	}

	std::string Response::notFound(bool isHtml) {
		std::ostringstream responseStream;
		if (isHtml) {
			std::string content;
			std::ifstream file("../assets/public/pages/404.html", std::ios::binary);
			if (file) {
				content.insert(content.end(), std::istreambuf_iterator<char>(file),
							   std::istreambuf_iterator<char>());
				responseStream << "HTTP/1.1 404 Not Found\r\n"
							   << "Content-Type: text/html" << "\r\n"
							   << "Content-Length: " << content.size() << "\r\n\r\n"
							   << content;
			}
		} else {
			responseStream << "HTTP/1.1 404 Not Found\r\n"
						   << "Content-Type: text/plain\r\n"
						   << "Content-Length: 9\r\n\r\n"
						   << "Not Found";
		}
		return responseStream.str();
	}
} // namespace sews
