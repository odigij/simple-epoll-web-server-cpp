# SEWS Project Layout

This document describes the purpose and structure of the SEWS (Simple Epoll Web Server) source tree.

SEWS is a modular, event-driven C++ framework designed around clear architectural separation between interfaces, implementations, and platform-specific bindings.

---

## include/

Contains all public headers. Organized into three main layers:

### 1. `core/` – Abstract Interfaces
- **Defines**: Polymorphic interfaces, domain contracts, value types.
- **Does not contain**: Concrete behavior, protocol knowledge, syscalls.

#### Key folders:
- `connection/` – Core abstractions for managing connections (channels, events, loops).
- `message/` – Protocol-agnostic message routing, dispatching, and encoding/decoding.
- `runtime/` – High-level orchestrators like abstract dispatchers.
- `telemetry/` – Interfaces and types for diagnostics and metrics.

### 2. `architecture/` – Protocol-Specific Behavior
- **Defines**: HTTP-specific implementations, format bindings, message handling logic.
- **Does not contain**: Platform or OS-level integration.

#### Key folders:
- `message/http/` – Contains codec (parser/serializer), dispatch (router), and transport (request/response) logic for HTTP.
- `runtime/http/` – Dispatcher orchestration for HTTP protocol.
- `telemetry/diagnostic/` – Logger implementations like `BasicLogger`, `NullLogger`.

### 3. `infrastructure/` – Platform-Bound Integration
- **Defines**: Syscall wrappers, epoll event loop, signal handling, atomic stop control.
- **Contains**: Anything that touches the operating system directly.

#### Key folders:
- `bootstrap/` – Signal and socket setup utilities.
- `connection/epoll/` – Event loop and acceptor based on Linux epoll.
- `control/` – Stop flag or low-level runtime controls.

---

## src/

Mirrors the `include/` structure, containing source files for all implementations.

- `architecture/` – Concrete implementations of protocol logic and components.
- `infrastructure/` – System interaction code such as epoll, signal handling.
- Structure mirrors `include/` for 1:1 header-source mapping.

---

## test/

Contains unit and integration tests for SEWS components.

- `test_handler.hpp` – HTTP test handler used for dispatch testing.
- `test_sews.cpp` – Main test runner (may use Catch2, GTest, etc.)

---

## core Patterns

### Message Dispatch Pipeline

```text
Incoming Bytes
   ↓
Decoder (parse raw → Message)
   ↓
Router (match Message → Handler)
   ↓
Handler (process Message → Response)
   ↓
Encoder (serialize Response → bytes)
   ↓
Channel (write bytes to peer)
```

All steps are pluggable. This allows support for additional protocols like WebSocket, GraphQL, or custom transports.

---

## Future Extensions

- Add `architecture/message/ws/` for WebSocket support
- Add `architecture/runtime/ws/ws_dispatcher.hpp`
- Introduce middleware adapter pattern in dispatcher layer
- Potential CMake modularization per domain

---

## Directory Summary

| Layer             | Purpose                                  |
|------------------|------------------------------------------|
| `core/`          | Abstract interfaces, POD types           |
| `architecture/`  | Protocol-specific logic and orchestration|
| `infrastructure/`| Platform-specific system interaction     |
| `src/`           | Implementation source files              |
| `test/`          | Unit and integration tests               |

---

## Naming Conventions

- `transport/` = runtime-facing data carriers and interfaces
- `codec/` = encoding and decoding logic (decoder ≈ parser, encoder ≈ serializer)
- `dispatch/` = routers and handlers
- `runtime/orchestrator/` = high-level controllers like `Dispatcher`

---
