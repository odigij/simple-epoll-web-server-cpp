#include "architecture/message/http/transport/request.hpp"
#include <sstream>

std::string requestSerializer(sews::architecture::message::http::transport::Request &request)
{
	std::ostringstream oss;
	oss << request.method << " " << request.path << " " << request.version << "\r\n";
	for (auto header : request.headers)
	{
		oss << header.first << ": " << header.second << "\r\n";
	}
	oss << "\r\n" << request.body;
	return oss.str();
}

int main()
{
	// TODO:
	// - [] Write tests.
	return 0;
}
