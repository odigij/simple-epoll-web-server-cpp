#ifndef SEWS_HPP
#define SEWS_HPP

#include <csignal>
#include <netinet/in.h>
#include <set>
#include <string>
#include <sys/epoll.h>
#include <tuple>

namespace sews {

void handleSigint(int sig);
std::tuple<int, int, int> handleArgs(int argumentCount, char *argumentVector[]);
std::tuple<int, sockaddr_in, int> initServer(int port, int maximumRequest);
void handleServerEvents(struct epoll_event &activeEpollEvent, int serverFileDescriptor,
                        int serverEpollFileDescriptor, std::set<int> &clientFileDescriptors);
std::string handleRequest(epoll_event &epollEvent);
void handleClientEvents(struct epoll_event &activeEpollEvent, int serverEpollFileDescriptor,
                        std::set<int> &clientFileDescriptors);
void thread(int argumentCount, char *argumentVector[]);

} // namespace sews

#endif // !SEWS_HPP
