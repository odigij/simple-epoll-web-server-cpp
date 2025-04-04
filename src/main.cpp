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

#include "sews/sews.hpp"

volatile sig_atomic_t sews::SignalHandler::_flags{0};

int main(int argc, char *argv[])
{
	try
	{
		sews::SignalHandler::init();
		sews::Router router;
		sews::Server server(router);
		sews::initializeApp(router);
		auto [port, maximum_request, epoll_event_size, time_out, flags] = sews::handleArgs(argc, argv);
		server.start(port, maximum_request, time_out, flags, epoll_event_size);
		while (sews::SignalHandler::getSignal() == 0)
		{
			try
			{
				server.update();
			}
			catch (const std::exception &ex)
			{
				sews::logger::log(sews::logger::Mode::ERROR, fmt::format("Server update exception: {}", ex.what()));
			}
		}
	}
	catch (const std::exception &ex)
	{
		sews::logger::log(sews::logger::Mode::ERROR, fmt::format("Server fatal exception: {}", ex.what()));
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
