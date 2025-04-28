# SEWS — Modular Event-Driven Server Framework (C++)

![Status: Experimental](https://img.shields.io/badge/status-experimental-orange) ![License: MIT](https://img.shields.io/badge/license-MIT-green) ![Platform: Linux](https://img.shields.io/badge/platform-Linux-blue) ![C++14](https://img.shields.io/badge/C++-14%2B-blue)

---

**SEWS** began as a tightly coupled experiment but has **evolved** into a **modular, extensible server framework skeleton**, built in **modern C++** for clarity, control, and minimalism.

SEWS provides a clean, runtime-separated architecture ready for multiple protocols, live metrics, and future extensibility — while intentionally keeping external dependencies minimal.

---

## Project Status

> **SEWS is under active development.**  
> It is **experimental** and **not production-ready**.
>
> Core modules (HTTP/1.1, metrics exporter, router) are functional, but advanced features (multi-threading, SSL/TLS support, dynamic backend loading) are planned.

---

## Current Modules

| Module | Purpose |
|:-------|:--------|
| **Socket Loop (Epoll-based)** | Non-blocking, event-driven I/O using `epoll`. |
| **Dispatcher** | Drives event polling and delegates message handling. |
| **Connection Manager** | Tracks and manages active client connections. |
| **Router (Trie-based)** | Efficient HTTP/1.1 method-path matching. |
| **Request Parser & Response Serializer (HTTP/1.1)** | Parses incoming requests and serializes responses. |
| **Metrics Manager** | Tracks live server metrics, exports via HTTP `/metrics`. |
| **Logger** | Basic modular logging framework (with null logger support). |

---

## Current Technical Features

| Feature | Description |
|:--------|:------------|
| **Single-threaded Epoll I/O** | Predictable, event-driven execution model. |
| **Runtime Metrics Exporter** | Live counters for uptime, active connections, request & response totals. |
| **Modular Architecture** | Clean separation between core, infrastructure, and runtime layers. |
| **Guided by SOLID Principles** | Modular, focused interfaces and components. |
| **Extensible Dispatcher Design** | Architecturally supports new backends (e.g., WebSocket, raw TCP) easily. |
| **Minimal External Dependencies** | No external web frameworks, pure C++14+. (Note: The CMakeLists.txt currently includes some legacy package checks that may be cleaned up in future versions.)|

---

## Requirements

- Linux OS
- GCC 14.2.1+ or Clang 19.1.7+
- CMake 3.10+

---

## Build & Run

```bash
# Clone the repository
$ git clone https://github.com/odigij/simple-epoll-web-server-cpp.git
$ cd simple-epoll-web-server-cpp

# Build
$ mkdir build && cd build
$ cmake ..
$ make

# Run
$ ./sews
```

---

## Disclaimer

This project is for educational, experimental, and prototyping purposes.
It is **not hardened** for real-world production use.

Use at your own discretion, and contributions are welcome.

---

## License

MIT License. See [LICENSE](./LICENSE) for details.
