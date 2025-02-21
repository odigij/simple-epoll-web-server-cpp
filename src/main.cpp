#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <getopt.h>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>

volatile sig_atomic_t stop = 0;
void handleSigint(int sig) {
  stop = 1;
  printf("SIGINT received.\n");
}

int main(int argc, char *argv[]) {
  try {
    // Signal handler registration.
    signal(SIGINT, handleSigint);
    long port = 8080;
    int maxReq = 3;
    // Setting parameters for terminal.
    static struct option longOptions[] = {{"port", required_argument, 0, 'p'},
                                          {"maximum-request", required_argument, 0, 'r'},
                                          {"help", no_argument, 0, '?'},
                                          {0, 0, 0, 0}};

    // Getting & handling user parameters through terminal.
    int opt;
    while ((opt = getopt_long(argc, argv, "p:r:?", longOptions, nullptr)) != -1) {
      switch (opt) {
      case 'p':
        port = std::strtol(optarg, nullptr, 10);
        if (port < 1 || port > 65535) {
          throw std::runtime_error("err_port_val");
        }
        break;
      case 'r':
        maxReq = std::stoi(optarg);
        if (maxReq < 0) {
          throw std::runtime_error("err_maxreq_val");
        }
        break;
      case '?':
        printf("sews -p <PORT> -r <MAX-REQUEST>, default port & max-req: 8080, 3\n");
        exit(EXIT_SUCCESS);
      default:
        printf("Unknown argument. Use --help for usage.\n");
        exit(EXIT_FAILURE);
      }
    }
    // Server socket initialization.
    int serverFd, serverEpollFd;
    struct sockaddr_in serverAddr;
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
      throw std::runtime_error("err_list_init");
    }
    // Server address configuration.
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    // Bind server socket to address.
    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
      throw std::runtime_error("err_list_bind");
    }
    // Server socket listens.
    if (listen(serverFd, maxReq) != 0) {
      throw std::runtime_error("err_list_listn");
    }
    printf("Server listens: 127.0.0.1:%ld\n", port);
    while (!stop) {
      // TODO Handle clients:
      //  - Define a fixed count concurrent epoll events.
      //  - Use epoll_ctl to get event count.
      //  - Integrate OpenSSL.
    }
    close(serverFd);
    exit(EXIT_SUCCESS);
  } catch (const std::runtime_error error) {
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
