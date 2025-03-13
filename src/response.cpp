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

#include <response.hpp>
#include <sstream>
#include <unordered_map>

namespace sews {
	std::string Response::text(const std::string& data, int status) {
		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 " << status << " OK\r\n"
					   << "Content-Type: text/plain\r\n"
					   << "Content-Length: " << data.size() << "\r\n"
					   << "Connection: close\r\n"
					   << "\r\n"
					   << data;
		return responseStream.str();
	}

	std::string Response::json(const std::string& jsonData, int status) {
		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 " << status << " OK\r\n"
					   << "Content-Type: application/json\r\n"
					   << "Content-Length: " << jsonData.size() << "\r\n"
					   << "Connection: close\r\n"
					   << "\r\n"
					   << jsonData;
		return responseStream.str();
	}

	std::string Response::html(const std::string& htmlContent, int status) {
		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 " << status << " OK\r\n"
					   << "Content-Type: text/html\r\n"
					   << "Content-Length: " << htmlContent.size() << "\r\n"
					   << "Connection: close\r\n"
					   << "\r\n"
					   << htmlContent;
		return responseStream.str();
	}

	std::string Response::notFound() {
		std::string content = "404 Not Found";
		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 404 Not Found\r\n"
					   << "Content-Type: text/plain\r\n"
					   << "Content-Length: " << content.size() << "\r\n"
					   << "Connection: close\r\n"
					   << "\r\n"
					   << content;
		return responseStream.str();
	}

	std::string Response::notAllowed() {
		std::string content = "405 Not Allowed";
		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 405 Not Allowed\r\n"
					   << "Content-Type: text/plain\r\n"
					   << "Content-Length: " << content.size() << "\r\n"
					   << "Connection: close\r\n"
					   << "\r\n"
					   << content;
		return responseStream.str();
	}

	std::string Response::getMimeType(const std::string& path) {
		static const std::unordered_map<std::string, std::string> mimeTypes = {
			{".html", "text/html"},		   {".css", "text/css"},  {".js", "application/javascript"},
			{".json", "application/json"}, {".png", "image/png"}, {".jpg", "image/jpeg"},
			{".jpeg", "image/jpeg"},	   {".gif", "image/gif"}, {".ico", "image/x-icon"},
			{".svg", "image/svg+xml"},	   {".txt", "text/plain"}};

		size_t dotPos = path.find_last_of(".");
		if (dotPos != std::string::npos) {
			std::string ext = path.substr(dotPos);
			auto it = mimeTypes.find(ext);
			if (it != mimeTypes.end()) {
				return it->second;
			}
		}
		return "application/octet-stream"; // Default for unknown types
	}

	std::string Response::fileResponse(const std::string& content, const std::string& path) {
		std::string mimeType = getMimeType(path);

		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 200 OK\r\n"
					   << "Content-Type: " << mimeType << "\r\n"
					   << "Content-Length: " << content.size() << "\r\n"
					   << "Connection: close\r\n"
					   << "\r\n"
					   << content;

		return responseStream.str();
	}

	std::string Response::custom(const std::string& content, const std::string& mimeType,
								 int status) {
		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 " << status << " OK\r\n"
					   << "Content-Type: " << mimeType << "\r\n"
					   << "Content-Length: " << content.size() << "\r\n"
					   << "Connection: close\r\n"
					   << "\r\n"
					   << content;
		return responseStream.str();
	}

} // namespace sews
