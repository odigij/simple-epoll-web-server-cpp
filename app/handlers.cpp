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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace app {
	std::string getContentType(const std::string& path) {
		static const std::unordered_map<std::string, std::string> mimeTypes = {
			{".html", "text/html"}, {".css", "text/css"},	  {".js", "application/javascript"},
			{".png", "image/png"},	{".jpg", "image/jpeg"},	  {".jpeg", "image/jpeg"},
			{".gif", "image/gif"},	{".ico", "image/x-icon"}, {".svg", "image/svg+xml"}};
		size_t dotPos = path.find_last_of(".");
		if (dotPos != std::string::npos) {
			std::string ext = path.substr(dotPos);
			auto it = mimeTypes.find(ext);
			if (it != mimeTypes.end()) {
				return it->second;
			}
		}
		return "application/octet-stream";
	}
	const std::string handleStaticFile(const sews::Request& request) {
		std::string filePath = "../assets" + request.path;
		std::ifstream file(filePath, std::ios::binary);
		std::ostringstream responseStream;
		std::string content;
		if (file) {
			content.insert(content.end(), std::istreambuf_iterator<char>(file),
						   std::istreambuf_iterator<char>());
			responseStream << "HTTP/1.1 200 OK\r\n"
						   << "Content-Type: " << getContentType(request.path) << "\r\n"
						   << "Content-Length: " << content.size() << "\r\n"
						   << "\r\n"
						   << content;
		} else {
			std::ifstream file("../assets/pages/404.html", std::ios::binary);
			content.insert(content.end(), std::istreambuf_iterator<char>(file),
						   std::istreambuf_iterator<char>());
			responseStream << "HTTP/1.1 404 Not Found\r\n"
						   << "Content-Type: text/html\r\n"
						   << "Content-Length: " << content.size() << "\r\n"
						   << "\r\n"
						   << content;
		}
		return responseStream.str();
	}
	const std::string handleIndex(const sews::Request& request) {
		std::string content;
		std::ostringstream responseStream;
		std::ifstream file("../assets/pages/index.html", std::ios::binary);
		if (file) {
			content.insert(content.end(), std::istreambuf_iterator<char>(file),
						   std::istreambuf_iterator<char>());
			responseStream << "HTTP/1.1 200 OK\r\n"
						   << "Content-Type: " << "text/html" << "\r\n"
						   << "Content-Length: " << content.size() << "\r\n"
						   << "\r\n"
						   << content;
		} else {
			content.insert(content.end(), std::istreambuf_iterator<char>(file),
						   std::istreambuf_iterator<char>());
			responseStream << "HTTP/1.1 404 Not Found\r\n"
						   << "Content-Type: " << "text/html" << "\r\n"
						   << "Content-Length: " << content.size() << "\r\n"
						   << "\r\n"
						   << content;
		}
		return responseStream.str();
	}
} // namespace app
