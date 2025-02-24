# Simple Epoll Web Server

## Description

This project is an **experimental** effort to understand the mechanics of a web server framework that is usually abstracted from developers. It is a **web server built from scratch in C++**, utilizing **epoll** for asynchronous, non-blocking I/O.

It is **Linux-specific**, as it relies on **POSIX** system calls and the `sys/epoll.h` header. Windows users would need to rewrite those sections or use `sys/poll.h` as an alternative.

## Features

- **Linux-based** – Uses epoll and POSIX APIs.
- **Single-threaded** – Efficiently handles multiple clients without extra threads.
- **Command-line options** – Supports terminal parameters (`--port`, `--max-request`, `--epoll-count`, `--help`).
- **Non-blocking mode** – Uses epoll for scalable, event-driven I/O.
- **Handles multiple clients** – Supports concurrent client connections.
- **Serves HTML & CSS** – Can deliver static web content.

## Requirements

- **Linux OS** (or WSL if on Windows)
- **C++17 or later**
- **GCC or Clang compiler**
- **Make** (optional, for easy compilation)

## Short Guide

- mkdir build
- cd ./build
- mkdir pages
- cd ./pages
- touch index.html
- cd ..
- cmake ..
- make
- ./sews

## License

This project is licensed under the **MIT License**.
