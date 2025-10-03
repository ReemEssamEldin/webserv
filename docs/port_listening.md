# Port Listening

## Overview

This document explains how Webserv handles listening on ports and accepting client connections. The server uses non-blocking sockets and the `poll()` system call to efficiently manage incoming connections.

## Current Implementation

### Basic Setup (Phase 1)

Currently, the server implements a simple single-port listener that:
1. Creates a TCP socket
2. Binds to port 8080 on all network interfaces
3. Listens for incoming connections
4. Uses `poll()` to wait for clients
5. Accepts one connection and closes

### Step-by-Step Process

#### 1. Socket Creation

```cpp
int serverFd = socket(AF_INET, SOCK_STREAM, 0);
```

Creates a TCP socket using:
- `AF_INET`: IPv4 addressing
- `SOCK_STREAM`: TCP protocol (reliable, connection-oriented)
- `0`: Default protocol for the socket type

#### 2. Socket Configuration

**Non-blocking Mode**

The socket is set to non-blocking mode using our platform-specific utility:

```cpp
setNonBlocking(serverFd);
```

This ensures the server never blocks on I/O operations, which is essential for handling multiple clients later.

**Socket Reuse**

```cpp
int opt = 1;
setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));
```

The `SO_REUSEADDR` option allows:
- Immediate restart of the server without "address already in use" errors
- Binding to the same port even if connections are in `TIME_WAIT` state
- Essential for development and testing

#### 3. Binding to a Port

```cpp
struct sockaddr_in serverAddr;
serverAddr.sin_family = AF_INET;
serverAddr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
serverAddr.sin_port = htons(8080);         // Port 8080 in network byte order

bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
```

Key points:
- `INADDR_ANY` (0.0.0.0) means the server accepts connections on any network interface
- `htons()` converts port number to network byte order (big-endian)
- The server is accessible via localhost, LAN IP, or any other interface

#### 4. Listening for Connections

```cpp
listen(serverFd, 10);
```

Marks the socket as passive (ready to accept connections) with a backlog of 10. The backlog determines how many pending connections can be queued before new ones are rejected.

#### 5. Waiting with poll()

```cpp
struct pollfd pollFd;
pollFd.fd = serverFd;
pollFd.events = POLLIN;  // Wait for incoming data/connections

poll(&pollFd, 1, -1);    // Block until a connection arrives
```

Why `poll()`?
- **Non-blocking compatibility**: Works with non-blocking sockets
- **Event-driven**: Only wakes up when something happens
- **Efficient**: No busy-waiting or constant checking
- **Scalable**: Can monitor multiple file descriptors (future expansion)

Parameters:
- `&pollFd`: Array of file descriptors to monitor
- `1`: Number of file descriptors (just one for now)
- `-1`: Timeout in milliseconds (-1 = wait indefinitely)

#### 6. Accepting Connections

```cpp
struct sockaddr_in clientAddr;
socklen_t clientAddrLen = sizeof(clientAddr);

int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
```

After `poll()` indicates a connection is ready:
- `accept()` creates a new socket for the client
- Returns immediately (no blocking) since connection is already pending
- `clientAddr` receives information about the connected client

## Testing

### Start the Server

```bash
./webserv
```

Expected output:
```
Webserv starting...
Server listening on port 8080
Waiting for connection...
```

### Connect with Telnet

In another terminal:

```bash
telnet localhost 8080
```

Server output:
```
Client connected!
Message sent to client
Connection closed. Server shutting down.
```

Client receives:
```
Hello from Webserv!
```

## Future Enhancements

The following features will be implemented in later phases:

### Multiple Ports

The server will need to:
- Read port configurations from a config file
- Create multiple listening sockets
- Monitor all ports simultaneously with `poll()`

Example future config:
```
server {
    listen 8080;
    listen 8081;
    listen 127.0.0.1:9000;
}
```

### Multiple Clients

Instead of accepting one connection and closing:
- Keep the server running in a loop
- Accept multiple clients
- Add client sockets to the `poll()` array
- Handle concurrent requests from different clients

### Persistent Connections

- Keep client connections alive for multiple requests
- Implement proper HTTP keep-alive behavior
- Track connection timeouts
- Gracefully close idle connections

## Common Issues

### "Address already in use"

If the server crashes or is force-killed, you might see:
```
Error: bind failed: Address already in use
```

**Solution**: Wait 30-60 seconds for the OS to release the port, or use:
```bash
lsof -ti:8080 | xargs kill -9
```

This is mitigated by `SO_REUSEADDR` in most cases.

### "Resource temporarily unavailable"

This error occurs when calling `accept()` on a non-blocking socket without waiting for a connection first. Always use `poll()` before `accept()` with non-blocking sockets.

### Permission Denied (Port < 1024)

Ports below 1024 require root privileges. Use ports ≥ 1024 for development, or run with `sudo` (not recommended for this project).

## Platform Differences

Both Linux and macOS handle socket listening similarly:
- Both support `INADDR_ANY` for all interfaces
- Both use `poll()` in the same way
- Both require `SO_REUSEADDR` for quick restart
- Port behavior is identical on both platforms

The main difference is in setting non-blocking mode (see `cross_platform.md`).
