# Simple Epoll Web Server (SEWS)

## 📌 Description

**SEWS** is a lightweight, experimental web server built in **C++** for learning and exploring web frameworks. It leverages **`sys/epoll`** for non-blocking, event-driven networking and is designed to be simple to use via **SSH on a remote server**. SEWS is **strictly single-threaded**, focusing on efficient I/O handling without multi-threading complexity.

It supports **HTTPS via OpenSSL** and serves **static files**, automatically generating **trie-based nodes** for files inside **`assets/public/static`** to optimize lookups. 

**Routing is fully customizable** via the `sews::initializeApp` function, allowing developers to define routes with support for **multiple paths and parameterized URLs** (manual parsing required for multiple parameters).

While not as feature-rich as mainstream web frameworks, **SEWS provides a minimalistic and educational approach** to server development, making it ideal for experimenting with event-driven architectures and low-level networking.

---

## ✨ **Technical Features**
| Feature | Description |
|---------|------------|
| **Epoll-based I/O** | Uses `epoll` for efficient event-driven networking, enabling non-blocking I/O operations. |
| **Minimalist, Single-threaded Design** | SEWS is strictly single-threaded, leveraging `sys/epoll` for event-driven concurrency without multi-threading. |
| **Command-Line Configuration** | Supports server configuration via CLI arguments at startup, allowing flexible initial setup. SEWS uses **JetBrains Mono font** and **Material Design Icons for logs**. |
| **Static Content Serving** | Automatically detects and serves static files, generating **trie-based nodes** for assets in `assets/public/static`. |
| **OpenSSL Integration** | Supports **HTTPS connections**, enabling secure communication. |
| **Custom Routing System** | Allows defining routes via `sews::initializeApp`, supporting multiple paths and parameterized URLs (manual parsing required for multiple parameters). |

---

## ⚙️ **Command-Line Options**
SEWS supports various command-line options for configuring server behavior at startup.

| Option | Description | Example |
|--------|-------------|---------|
| `-p`   | Set the server’s listening port. | `./sews -p 8080` |
| `-m`   | Define the maximum number of pending connections in the backlog queue before new requests are refused. | `./sews -m 128` |
| `-e`   | Set the number of events that can be processed per epoll cycle. | `./sews -e 32` |
| `-t`   | Specify the timeout (in milliseconds) for epoll waiting. | `./sews -t 500` |
| `-f`   | Define additional configuration flags for advanced server behavior. | `./sews -f 0` |
| `--help` | Show all available CLI options and their descriptions. | `./sews --help` |

---

## 🔧 **Build and Run Instructions**

### **📌 Requirements**
- **Linux OS** (or WSL if on Windows)
- **C++17 or later**
- **GCC or Clang compiler**
- **CMake**
- **OpenSSL**

### **📌 Steps to Build and Run**

1. **Clone the Repository:**
   ```bash
   git clone <repository_url>
