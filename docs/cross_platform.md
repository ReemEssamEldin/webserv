# Cross-Platform Compatibility Guide

## Overview

Webserv is designed to compile and run on both **Linux** and **macOS** while maintaining strict C++98 compliance. This document outlines the platform-specific considerations and how they are handled.

## Platform Detection

The code uses preprocessor macros to detect the operating system at compile time:

```cpp
#ifdef __APPLE__
    // macOS-specific code
#else
    // Linux-specific code
#endif
```

## Non-Blocking Socket Configuration

### Challenge

Setting sockets to non-blocking mode differs between macOS and Linux:

- **macOS**: Requires `fcntl()` with `F_SETFL` and `O_NONBLOCK` flags
- **Linux**: Can use `setsockopt()` with `SO_NONBLOCK` option

According to 42 project constraints, `fcntl()` is only allowed on macOS and only with specific flags (`F_SETFL`, `O_NONBLOCK`, `FD_CLOEXEC`).

### Solution

We created a centralized utility function `setNonBlocking()` in `socket_utils.cpp`:

```cpp
int setNonBlocking(int fd)
{
#ifdef __APPLE__
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return -1;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        return -1;
    return 0;
#else
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_NONBLOCK,
                   (const void*)&opt, sizeof(opt)) < 0)
        return -1;
    return 0;
#endif
}
```

### Usage

```cpp
#include "socket_utils.hpp"

int serverFd = socket(AF_INET, SOCK_STREAM, 0);
if (setNonBlocking(serverFd) < 0) {
    // Handle error
}
```

## Other Platform Considerations

### Socket Options

The `setsockopt()` function has slightly different signatures:
- **macOS**: Expects `const void*` for option value
- **Linux**: Expects `const char*` for option value

We use explicit casting to `(const void*)` for compatibility:

```cpp
int opt = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));
```

### POSIX Compliance

All system calls used in the project are POSIX-compliant and available on both platforms:
- `socket()`, `bind()`, `listen()`, `accept()`, `send()`, `recv()`
- `poll()` for I/O multiplexing
- `close()` for cleanup

## Testing on Multiple Platforms

### macOS
```bash
make re
./webserv
```

### Linux
```bash
make re
./webserv
```

Both should compile without warnings using:
```bash
c++ -Wall -Wextra -Werror -std=c++98
```

## Future Considerations

As the project grows, any platform-specific code should be:
1. **Centralized** in utility functions
2. **Documented** in this guide
3. **Tested** on both platforms
4. **Minimized** - prefer POSIX-compliant solutions when possible
