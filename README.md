# Simple Epoll Web Server

![Screenshot From 2025-02-25 00-53-39](https://github.com/user-attachments/assets/23e38192-6902-4f2a-99ab-5e65e57b0ade)

# Description

This project is an **experimental** effort to understand the mechanics of a web server framework that is usually abstracted from developers. It is a **web server built from scratch in C++**, utilizing **epoll** for asynchronous, non-blocking I/O. It is **Linux-specific**, as it relies on **POSIX** system calls and the `sys/epoll.h` header.

# Features

- **Linux-based** – Uses epoll and POSIX APIs.

- **Single-threaded** – Efficiently handles multiple clients without extra threads.

- **Command-line options** – Supports terminal parameters (`--port`, `--max-request`, `--epoll-count`, `--help`).

- **Non-blocking mode** – Uses epoll for scalable, event-driven I/O.

- **Handles multiple clients** – Supports concurrent client connections.

- **Serves HTML & CSS** – Can deliver static web content.

- **Customizable router** – Define controllers to manage different request paths.

# Requirements

- **Linux OS** (or WSL if on Windows)

- **C++17 or later**

- **GCC or Clang compiler**

- **CMake**

# Example Usage

## Build and Run Instructions

### Prerequisites

Ensure you have CMake and the required dependencies installed before proceeding.

### Steps to Build and Run

1. **Download or Clone the Repository:**
   ```bash
   git clone <repository_url>
   ```
   *(Replace `<repository_url>` with the actual repository link.)*

2. **Navigate to the Project Directory:**
   ```bash
   cd <repository_name>
   ```

3. **Create and Enter the Build Directory:**
   ```bash
   mkdir build && cd build
   ```

4. **Run CMake to Generate Build Files:**
   ```bash
   cmake ..
   ```

5. **Compile the Project:**
   ```bash
   make
   ```

6. **Run the Executable:**
   ```bash
   ./sews
   ```
### Advanced Usage

By default, the server comes with a predefined router and static content handling. To customize request handling, follow these steps:

1. **Create a file named entry.cpp in the app directory.**

2. **Include the app_entry.hpp header:**
   ```cpp
   #include "../include/app_entry.hpp"
   ```
3. **Define the sews::initializeApp function in entry.cpp to customize the router:**
   ```cpp
   void sews::initializeApp(sews::Router& router) {
    // Define your custom routes and handlers here
    }
   ```
This allows you to extend and modify the request handling logic according to your application’s needs.

# License

This project is licensed under the **MIT License**.
