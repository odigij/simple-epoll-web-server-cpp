#include "include/sews.hpp"
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <sys/epoll.h>

namespace sews {
volatile sig_atomic_t stop;

void handleSigint(int sig) {
  std::cout << "SIGINT received" << '\n';
  stop = 1;
}

std::tuple<int, int, int> handleArgs(int argumentCount, char *argumentVector[]) {
  int port = 8080, maximumRequest = 3, epollCount = 32, option;
  std::string message;
  static struct option longOptions[] = {
      {"port", optional_argument, nullptr, 'p'},
      {"max-request", optional_argument, nullptr, 'm'},
      {"epoll-count", optional_argument, nullptr, 'e'},
      {"help", no_argument, 0, 'h'},
      {0, no_argument, 0, 0},
  };
  while ((option = getopt_long(argumentCount, argumentVector, "p:m:e:h:", longOptions, nullptr)) !=
         -1) {
    switch (option) {
    case 'p':
      port = optarg ? std::stoi(optarg) : port;
      message = port > 0 && port < 65535
                    ? ""
                    : "Invalid port value, 0 < val < 65535, default value is 8080.";
      break;
    case 'm':
      maximumRequest = optarg ? std::stoi(optarg) : maximumRequest;
      message = maximumRequest >= 0
                    ? ""
                    : "Maximum request must be equal or greater than zero, default value is 3.";
      break;
    case 'e':
      epollCount = optarg ? std::stoi(optarg) : epollCount;
      message =
          epollCount > 0 ? "" : "Epoll count must be greater than zero, default values is 32.";
      break;
    case 'h':
      std::cout << "sews -p <value> -m <value> -e <value>\nsews -h\nsews --port <value> "
                   "--max-request <value> --epoll-count <value>\nsew --help\n";
      exit(EXIT_SUCCESS);
    default:
      message = "invalid argument, use --help for usage";
      break;
    }
    if (message != "") {
      std::cerr << message << ".\n";
      exit(EXIT_FAILURE);
    }
  }
  std::cout << "SEWS listens http://127.0.0.1:" << port << " | max-request: " << maximumRequest
            << ", epoll-count: " << epollCount << '\n';
  return std::make_tuple(port, maximumRequest, epollCount);
}

std::tuple<int, sockaddr_in, int> initServer(int port, int maximumRequest) {
  int fileDescriptor, epollFileDescriptor, opt = 1;
  struct sockaddr_in address;
  fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (fileDescriptor == -1) {
    throw std::runtime_error("FATAL: Server creation failed.\n");
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  if (setsockopt(fileDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    throw std::runtime_error("FATAL: setsockopt.\n");
  }
  if (bind(fileDescriptor, (struct sockaddr *)&address, sizeof(address)) != 0) {
    close(fileDescriptor);
    throw std::runtime_error("FATAL: Server binding failed.\n");
  }
  if (listen(fileDescriptor, maximumRequest) != 0) {
    close(fileDescriptor);
    throw std::runtime_error("FATAL: Server listen failed.\n");
  }
  epollFileDescriptor = epoll_create(1);
  if (epollFileDescriptor < 0) {
    throw std::runtime_error(strerror(errno));
  }
  struct epoll_event tempEvent;
  tempEvent.events = EPOLLIN;
  tempEvent.data.fd = fileDescriptor;
  if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, fileDescriptor, &tempEvent) != 0) {
    throw std::runtime_error("FATAL: Server epoll register failed.\n");
  }
  return std::make_tuple(fileDescriptor, address, epollFileDescriptor);
}

void handleServerEvents(struct epoll_event &activeEpollEvent, int serverFileDescriptor,
                        int serverEpollFileDescriptor, std::set<int> &clientFileDescriptors) {
  if (activeEpollEvent.events & EPOLLIN) {
    struct sockaddr_in newClientAddress;
    socklen_t newClientAddressSize = sizeof(newClientAddress);
    const int newClientFileDescriptor =
        accept(serverFileDescriptor, (struct sockaddr *)&newClientAddress, &newClientAddressSize);
    if (newClientFileDescriptor < 0) {
      std::cerr << "LOG: Client connection failed.\n";
      return;
    }
    int flags = fcntl(newClientFileDescriptor, F_GETFL, 0);
    if (flags == -1) {
      std::cerr << "LOG: Failed to get file flags.\n";
      close(newClientFileDescriptor);
      return;
    }
    if (fcntl(newClientFileDescriptor, F_SETFL, flags | O_NONBLOCK) == -1) {
      std::cerr << "LOG: Failed to set non-blocking mode.\n";
      close(newClientFileDescriptor);
      return;
    }
    epoll_event newClientEpollEvent;
    newClientEpollEvent.events = EPOLLIN;
    newClientEpollEvent.data.fd = newClientFileDescriptor;
    if (epoll_ctl(serverEpollFileDescriptor, EPOLL_CTL_ADD, newClientFileDescriptor,
                  &newClientEpollEvent) != 0) {
      close(newClientFileDescriptor);
      std::cerr << "LOG: Client epoll register failed.\n";
      return;
    }
    clientFileDescriptors.insert(newClientFileDescriptor);
  }
  if (activeEpollEvent.data.fd == serverFileDescriptor &&
      activeEpollEvent.events & (EPOLLHUP | EPOLLERR)) {
    throw std::runtime_error("FATAL: Server socket closed unexpectedly.\n");
  }
}

std::string handleRequest(epoll_event &epollEvent) {
  char buffer[1024 * 2];
  ssize_t bytesRead = read(epollEvent.data.fd, buffer, sizeof(buffer) - 1);
  std::ostringstream responseStream;
  std::istringstream requestStream(buffer);
  std::string method, path, httpVersion, filePath, htmlContent, contentType = "text/html";
  requestStream >> method >> path >> httpVersion;
  if (bytesRead > 0) {
    buffer[bytesRead] = '\0';
    filePath =
        path == "/" || path.empty() ? "../assets/pages/index.html" : "../assets/pages" + path;
    if (filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == ".css") {
      filePath = "../assets/styles/" + path;
      contentType = "text/css";
    }
    if (method == "GET") {
      std::ifstream file(filePath, std::ios::binary);
      if (file) {
        htmlContent.insert(htmlContent.end(), std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>());
        responseStream << "HTTP/1.1 200 OK\r\n"
                       << "Content-Type: " << contentType << "\r\n"
                       << "Content-Length: " << htmlContent.size() << "\r\n"
                       << "\r\n"
                       << htmlContent;
      } else {
        htmlContent.insert(htmlContent.end(), std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>());
        responseStream << "HTTP/1.1 404 Not Found\r\n"
                       << "Content-Type: " << contentType << "\r\n"
                       << "Content-Length: " << htmlContent.size() << "\r\n"
                       << "\r\n"
                       << htmlContent;
      }
    } else {
      std::ifstream file("../assests/pages/405.html", std::ios::binary);
      htmlContent.insert(htmlContent.end(), std::istreambuf_iterator<char>(file),
                         std::istreambuf_iterator<char>());
      responseStream << "HTTP/1.1 405 Method Not Allowed\r\n"
                     << "Content-Type: " << contentType << "\r\n"
                     << "Content-Length: " << htmlContent.size() << "\r\n"
                     << "\r\n"
                     << htmlContent;
    }
  } else if (bytesRead == 0) {
    // Unimplemented scope
  } else {
    // Unimplemented scope
  }
  return responseStream.str();
}

void handleClientEvents(struct epoll_event &activeEpollEvent, int serverEpollFileDescriptor,
                        std::set<int> &clientFileDescriptors) {
  if (activeEpollEvent.events & (EPOLLHUP | EPOLLERR)) {
    if (epoll_ctl(serverEpollFileDescriptor, EPOLL_CTL_DEL, activeEpollEvent.data.fd, nullptr) ==
        -1) {
      std::cerr << "LOG: Failed to remove client from epoll registry.\n";
    }
    close(activeEpollEvent.data.fd);
    clientFileDescriptors.erase(activeEpollEvent.data.fd);
  } else if (activeEpollEvent.events & EPOLLIN) {
    std::string httpResponse = handleRequest(activeEpollEvent);
    if (send(activeEpollEvent.data.fd, httpResponse.c_str(), httpResponse.size(), 0) == -1) {
      std::cerr << "LOG: Failed to send HTML.\n";
    } else {
      // Unimplemented scope
    }
    epoll_ctl(serverEpollFileDescriptor, EPOLL_CTL_DEL, activeEpollEvent.data.fd, nullptr);
    close(activeEpollEvent.data.fd);
    clientFileDescriptors.erase(activeEpollEvent.data.fd);
  }
}

void thread(int argumentCount, char *argumentVector[]) {
  try {
    signal(SIGINT, handleSigint);
    auto [port, maximumRequest, epollEventSize] = handleArgs(argumentCount, argumentVector);
    auto [serverFileDescriptor, serverAddr, serverEpollFileDescriptor] =
        initServer(port, maximumRequest);
    struct epoll_event activeEpollEvents[epollEventSize];
    std::set<int> clientFileDescriptors;
    while (!sews::stop) {
      const int activeEventCount =
          epoll_wait(serverEpollFileDescriptor, activeEpollEvents, epollEventSize, 3000);
      for (int index(0); index < activeEventCount; index++) {
        epoll_event &currentEvent = activeEpollEvents[index];
        if (currentEvent.data.fd == serverFileDescriptor) {
          handleServerEvents(currentEvent, serverFileDescriptor, serverEpollFileDescriptor,
                             clientFileDescriptors);
        } else {
          handleClientEvents(currentEvent, serverEpollFileDescriptor, clientFileDescriptors);
        }
      }
    }
    for (auto activeClientFileDescriptor : clientFileDescriptors) {
      close(activeClientFileDescriptor);
    }
    close(serverEpollFileDescriptor);
    close(serverFileDescriptor);
    exit(EXIT_SUCCESS);
  } catch (const std::exception &err) {
    std::cerr << err.what();
    exit(EXIT_FAILURE);
  }
}

} // namespace sews
