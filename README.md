# Simple Epoll Web Server (SEWS)

## Description

**SEWS** is a lightweight, experimental web server written in **C++**. It’s built for learning and exploration, not production. It uses **`sys/epoll`** for non-blocking, event-driven I/O and is intentionally **single-threaded** to keep complexity down.

It includes **HTTPS support** via OpenSSL and serves **static files** by generating **trie-based lookup nodes** for anything in `assets/public/static`. Routing is configured via `sews::initializeApp`, supporting **multiple path aliases** and **basic path parameter matching** (you must handle parameter parsing yourself).

SEWS is a low-level HTTP server skeleton, not a framework. It provides basic parsing of incoming HTTP requests through the sews::Request class, including method, path, headers, query parameters, and body. There is no middleware system, thread pool, or high-level routing abstraction — all behavior is defined manually and explicitly.

---

## Technical Features

| Feature                        | Description                                                                |
| ------------------------------ | -------------------------------------------------------------------------- |
| **Epoll-based I/O**            | Uses `epoll` for efficient event-driven, non-blocking networking.          |
| **Single-threaded Design**     | All operations run on one thread; concurrency is handled by epoll.         |
| **Command-Line Configuration** | Startup behavior is controlled via CLI flags.                              |
| **Static Content Serving**     | Serves files in `assets/public/static` via trie-optimized path resolution. |
| **OpenSSL Integration**        | HTTPS supported using OpenSSL; HTTP can be enabled with a flag.            |
| **Custom Routing System**      | Routes are defined manually with basic support for parameters.             |

---

## Command-Line Options

| Option   | Description                                               | Example          |
| -------- | --------------------------------------------------------- | ---------------- |
| `-p`     | Set the server port.                                      | `./sews -p 8080` |
| `-m`     | Max pending connections in the backlog queue.             | `./sews -m 128`  |
| `-e`     | Max number of epoll events per cycle.                     | `./sews -e 32`   |
| `-t`     | Timeout (ms) for epoll wait.                              | `./sews -t 500`  |
| `-f`     | Flag to control extra behavior (e.g. TLS enable/disable). | `./sews -f 0`    |
| `--help` | Show all available CLI options.                           | `./sews --help`  |

---

## Build and Run Instructions

### Requirements

- Linux OS (or WSL)
- C++17-compatible compiler (GCC/Clang)
- CMake
- OpenSSL
- FMT

### Instructions

```bash
# Clone and build
git clone https://github.com/odigij/simple-epoll-web-server-cpp.git
cd simple-epoll-web-server-cpp
mkdir build && cd build
cmake ..
make

# Optional: generate self-signed certificate
openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365 -nodes

# Run the server (TLS enabled)
./sews -p 8080 -f 1 >> sews.log 2>&1 &
disown

# Or alternatively:
nohup ./sews >> sews.log 2>&1 &
```
