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

#include <getopt.h>
#include <iostream>
#include <fmt/format.h>
#include "sews/args.hpp"

namespace sews
{
	std::tuple<int, int, int, int, int> handleArgs(int argument_count, char *argument_vector[])
	{
		int port = 443, maximum_request = 128, epoll_event_size = 64, timeout = 500, flags = 1, option;
		std::string terminal_message;
		static struct option long_options[] = {
			{"port", optional_argument, nullptr, 'p'},
			{"max-request", optional_argument, nullptr, 'm'},
			{"epoll-count", optional_argument, nullptr, 'e'},
			{"timeout", optional_argument, nullptr, 't'},
			{"flags", optional_argument, nullptr, 'f'},
			{"help", no_argument, 0, 'h'},
			{0, no_argument, 0, 0},
		};
		while ((option = getopt_long(argument_count, argument_vector, "p:m:e:t:f:h:", long_options, nullptr)) != -1)
		{
			switch (option)
			{
				case 'p':
					port = optarg ? std::stoi(optarg) : port;
					terminal_message =
						port > 0 && port < 65535
							? ""
							: fmt::format(
								  "󰅙 Invalid port value; {}. Port must be 0 < val < 65535, default value is 443.",
								  port);
					break;
				case 'm':
					maximum_request = optarg ? std::stoi(optarg) : maximum_request;
					terminal_message = maximum_request >= 0
										   ? ""
										   : fmt::format("󰅙 Invalid value; {}. Maximum request must be equal or "
														 "greater than zero, default value is 128.",
														 maximum_request);
					break;
				case 'e':
					epoll_event_size = optarg ? std::stoi(optarg) : epoll_event_size;
					terminal_message = epoll_event_size > 0 ? ""
															: fmt::format("󰅙 Invalid value; {}. Epoll count must be "
																		  "greater than zero, default values is 64.",
																		  epoll_event_size);
					break;
				case 't':
					timeout = optarg ? std::stoi(optarg) : timeout;
					terminal_message = timeout >= 0 ? ""
													: fmt::format("󰅙 Invalid value; {}. Timeout must be equal or "
																  "greater than zero, default values is 500.",
																  timeout);
					break;
				case 'f':
					flags = optarg ? std::stoi(optarg) : flags;
					terminal_message = flags >= 0 ? ""
												  : fmt::format("󰅙 Invalid value; {}. Flags must be equal or "
																"greater than zero, default values is 1.",
																flags);
					break;
				case 'h':
					std::cout << "󰞷 sews -p <value>\n"
								 "󰞷 sews -m <value>\n"
								 "󰞷 sews -e <value>\n"
								 "󰞷 sews -t <value>\n"
								 "󰞷 sews --port <value>\n"
								 "󰞷 sews --max-request <value>\n"
								 "󰞷 sews --epoll-count <value>\n"
								 "󰞷 sews --timeout <value>\n"
								 "󰞷 sews --flags <value>\tset it 0 to not use TLS or set it 1 to use TLS\n"
								 "󰞷 sews --help\n";
					exit(EXIT_SUCCESS);
				default:
					terminal_message = "invalid argument, use --help for usage";
					break;
			}
			if (terminal_message != "")
			{
				std::cerr << terminal_message << "\n";
				exit(EXIT_FAILURE);
			}
		}
		return std::make_tuple(port, maximum_request, epoll_event_size, timeout, flags);
	}
} // namespace sews
