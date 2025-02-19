#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
// TODO Implement SIGINT
int main(int argc, char *argv[]) {
  try {
    long port = 8080;
    // FIXME Need better way to parse & handle.
    // "./" is not good enough for usability.
    // Getting user parameters through terminal.
    // May include max N request parameter too.
    if (std::strcmp(argv[0], "./sews")) {
      if (std::strcmp(argv[1], "-p") == 0) {
        char *end;
        port = std::strtol(argv[2], &end, 10);
        delete end;
        end = nullptr;
        if (*end != '\0' || port < 0 || port > 65535) {
          throw std::runtime_error("err_port_val");
        }
      }
    }
    int serverFd, serverEpollFd;
    struct sockaddr_in serverAddr;
    // Server socket initialization
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
      throw std::runtime_error("err_list_init");
    }
    // Server address configuration
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    // Bind server socket to address
    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
      throw std::runtime_error("err_list_bind");
    }
    // Server socket listens
    if (listen(serverFd, 5) != 0) {
      throw std::runtime_error("err_list_listn");
    }
    printf("Server listens: 127.0.0.1:%ld\n", port);
    while (serverFd != 0) {
      // TODO Handle clients with epoll_ctl
    }
    exit(EXIT_SUCCESS);
  } catch (const std::runtime_error error) {
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
