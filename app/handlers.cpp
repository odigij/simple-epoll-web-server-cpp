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
