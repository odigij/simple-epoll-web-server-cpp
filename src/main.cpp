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

#include <iostream>
#include <sews.hpp>

volatile sig_atomic_t sews::SignalHandler::_flags = 0;

int main(int argc, char* argv[]) {
	try {
		sews::SignalHandler::init();
		sews::Router router;
		sews::Server server(router);
		sews::initializeApp(router);
		auto [ port, maximumRequest, epollEventSize, timeout, flags ] =
			sews::handleArgs(argc, argv);
		server.start(port, maximumRequest, timeout, flags, epollEventSize);
		while (sews::SignalHandler::getSignal() == 0) {
			try {
				server.update();
			} catch (const std::exception& ex) {
				std::cerr << "Error handling event: " << ex.what() << std::endl;
			}
		}
		exit(EXIT_SUCCESS);
	} catch (const std::exception& err) {
		std::cerr << err.what();
		exit(EXIT_FAILURE);
	}
}
