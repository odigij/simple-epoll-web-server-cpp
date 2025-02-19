#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
// TODO Send parameters by arguments to terminal(?)
int main(int argc, char *argv[]) {
  try {
    int serverFd, serverEpollFd;
    struct sockaddr_in serverAddr;
    // Server socket initialization
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
      // TODO Define error types to be informative
      throw new std::runtime_error("");
    }
    // Server address configuration
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    // Bind server socket to address
    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
      throw new std::runtime_error("");
    }
    if (listen(serverFd, 5) != 0) {
      throw new std::runtime_error("");
    }
    while (serverFd != 0) {
      // TODO Handle clients with epoll_ctl
    }
    return 0;
  } catch (const std::runtime_error error) {
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
