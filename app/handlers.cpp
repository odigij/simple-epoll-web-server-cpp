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

#include "handlers.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace app {
	const std::string handleIndex(const sews::Request& request) {
		std::string file_path = "../assets/pages/index.html", html_content,
					content_type = "text/html";
		std::ostringstream responseStream;
		std::ifstream file(file_path, std::ios::binary);
		if (file) {
			html_content.insert(html_content.end(), std::istreambuf_iterator<char>(file),
								std::istreambuf_iterator<char>());
			responseStream << "HTTP/1.1 200 OK\r\n"
						   << "Content-Type: " << content_type << "\r\n"
						   << "Content-Length: " << html_content.size() << "\r\n"
						   << "\r\n"
						   << html_content;
		} else {
			html_content.insert(html_content.end(), std::istreambuf_iterator<char>(file),
								std::istreambuf_iterator<char>());
			responseStream << "HTTP/1.1 404 Not Found\r\n"
						   << "Content-Type: " << content_type << "\r\n"
						   << "Content-Length: " << html_content.size() << "\r\n"
						   << "\r\n"
						   << html_content;
		}
		return responseStream.str();
	}

	const std::string handleStyle(const sews::Request& request) {
		std::string file_path = "../assets/styles/stylesheet.css", html_content,
					content_type = "text/css";
		std::ostringstream responseStream;
		std::ifstream file(file_path, std::ios::binary);
		if (file) {
			html_content.insert(html_content.end(), std::istreambuf_iterator<char>(file),
								std::istreambuf_iterator<char>());
			responseStream << "HTTP/1.1 200 OK\r\n"
						   << "Content-Type: " << content_type << "\r\n"
						   << "Content-Length: " << html_content.size() << "\r\n"
						   << "\r\n"
						   << html_content;
		} else {
			html_content.insert(html_content.end(), std::istreambuf_iterator<char>(file),
								std::istreambuf_iterator<char>());
			responseStream << "HTTP/1.1 404 Not Found\r\n"
						   << "Content-Type: " << content_type << "\r\n"
						   << "Content-Length: " << html_content.size() << "\r\n"
						   << "\r\n"
						   << html_content;
		}
		return responseStream.str();
	}
} // namespace app
