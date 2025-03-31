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

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

namespace sews
{
	class Response
	{
	  public:
		static std::string text(const std::string &data, int status = 200);
		static std::string json(const std::string &jsonData, int status = 200);
		static std::string html(const std::string &htmlContent, int status = 200);
		static std::string fileResponse(const std::string &content, const std::string &path);
		static std::string custom(const std::string &content, const std::string &mimeType, int status);
		static std::string notFound();
		static std::string notAllowed();
		static std::string getMimeType(const std::string &path);
	};
} // namespace sews
#endif
