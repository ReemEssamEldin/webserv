# Webserv Documentation

## Overview

Webserv is a minimal HTTP server written in C++98 as part of the 42 curriculum. This server implements non-blocking I/O using `poll()` and can handle multiple client connections while serving static files and executing CGI scripts.

## Current Status

The project is in its **initial setup phase**. So far, we have:

- ✅ Basic project structure
- ✅ Makefile with C++98 compilation
- ✅ Simple socket server that accepts one connection

## Project Structure

```
webserv/
├── src/           # Source files (.cpp)
├── include/       # Header files (.hpp)
├── conf/          # Configuration files
├── obj/           # Compiled object files (generated)
├── docs/          # Documentation
├── changelog/     # Daily change logs
├── Makefile       # Build configuration
└── webserv        # Compiled executable (generated)
```

## Building the Project

### Requirements
- C++ compiler with C++98 support
- Make
- POSIX-compliant system (Linux, macOS)

### Compilation
```bash
make        # Build the project
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Rebuild from scratch
```

## Running the Server

### Basic Usage
```bash
./webserv
```

The server will:
1. Print "Webserv starting..."
2. Listen on port 8080
3. Accept one connection
4. Send "Hello from Webserv!" to the client
5. Close and exit

### Testing with Telnet
```bash
telnet localhost 8080
```

You should see "Hello from Webserv!" printed in your terminal.

## Next Steps

The following features need to be implemented:

1. **Configuration Parser** - Read server configuration from file
2. **Poll-based Event Loop** - Handle multiple clients simultaneously
3. **HTTP Request Parser** - Parse incoming HTTP requests
4. **HTTP Response Builder** - Generate proper HTTP responses
5. **Static File Serving** - Serve files from configured directories
6. **CGI Support** - Execute CGI scripts and return output
7. **Error Handling** - Proper error pages and status codes

## Technical Details

### Allowed System Calls
- Socket operations: `socket`, `bind`, `listen`, `accept`, `send`, `recv`
- I/O multiplexing: `poll`, `select`, `kqueue`, `epoll`
- File operations: `open`, `read`, `write`, `close`, `stat`
- Process control: `fork`, `execve`, `pipe`, `dup`, `dup2` (for CGI)
- Directory operations: `opendir`, `readdir`, `closedir`
- Socket options: `fcntl`, `setsockopt`

### Code Standards
- **Language**: C++98 strict
- **Indentation**: 4 spaces (no tabs)
- **Line length**: 80-100 characters
- **Function length**: < 25 lines
- **Naming**:
  - Classes: `PascalCase`
  - Functions/variables: `camelCase`
  - Constants: `UPPER_SNAKE_CASE`

## References

- HTTP/1.0 specification (RFC 1945)
- HTTP/1.1 specification (RFC 2616) for reference only
- NGINX configuration format (inspiration for config parser)
