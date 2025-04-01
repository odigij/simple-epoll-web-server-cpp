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

#include <algorithm>
#include <sstream>
#include <unordered_map>
#include "sews/response.hpp"

namespace sews
{
	std::string Response::text(const std::string &data, int status)
	{
		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 " << status << " OK\r\n"
					   << "Content-Type: text/plain\r\n"
					   << "Content-Length: " << data.size() << "\r\n"
					   << "Connection: close\r\n"
					   << "\r\n"
					   << data;
		return responseStream.str();
	}

	std::string Response::notFound()
	{
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

	std::string Response::notAllowed()
	{
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

	std::string Response::getMimeType(const std::string &path)
	{
		static const std::unordered_map<std::string, std::string> mimeTypes = {
			{".html", "text/html"},		   {".css", "text/css"},	   {".js", "application/javascript"},
			{".json", "application/json"}, {".png", "image/png"},	   {".jpg", "image/jpeg"},
			{".jpeg", "image/jpeg"},	   {".gif", "image/gif"},	   {".ico", "image/x-icon"},
			{".svg", "image/svg+xml"},	   {".txt", "text/plain"},	   {".woff", "font/woff"},
			{".woff2", "font/woff2"},	   {".ttf", "font/ttf"},	   {".eot", "application/vnd.ms-fontobject"},
			{".mp3", "audio/mpeg"},		   {".mp4", "video/mp4"},	   {".xml", "application/xml"},
			{".pdf", "application/pdf"},   {".zip", "application/zip"}};

		size_t dotPos = path.find_last_of(".");
		if (dotPos != std::string::npos && dotPos != path.length() - 1)
		{
			std::string ext = path.substr(dotPos);
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			auto it = mimeTypes.find(ext);
			if (it != mimeTypes.end())
			{
				return it->second;
			}
		}

		return "application/octet-stream"; // Default for unknown file types
	}

	std::string Response::custom(const std::string &content, const std::string &mimeType, int status)
	{
		std::ostringstream responseStream;
		responseStream << "HTTP/1.1 " << status << " OK\r\n"
					   << "Content-Type: " << mimeType << "\r\n"
					   << "Content-Length: " << content.size() << "\r\n"
					   << "Connection: close\r\n"
					   /*<< "Connection: keep-alive\r\n"*/
					   /*<< "Keep-Alive: timeout=1, max=100\r\n"*/
					   << "\r\n"
					   << content;
		return responseStream.str();
	}
} // namespace sews
