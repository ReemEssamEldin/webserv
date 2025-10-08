# HTTP Request Handling

This document explains how Webserv handles HTTP requests, from receiving raw data to sending back responses.

## Architecture Overview

The request handling system uses a modular class-based architecture with clear separation of concerns:

```
Client Request → Server → HttpRequest (parse) → FileReader (read file) → HttpResponse (build) → Client
```

## Core Classes

### HttpRequest

**Purpose**: Parse raw HTTP request strings into structured, accessible data.

**Location**: `include/http_request.hpp`, `src/http_request.cpp`

**Responsibilities**:
- Parse HTTP request line (method, path, version)
- Extract and store headers as key-value pairs
- Handle request body (for POST requests)
- Validate request format

**Usage Example**:
```cpp
HttpRequest request;
if (request.parse(rawRequestString)) {
    std::string method = request.getMethod();      // "GET"
    std::string path = request.getPath();          // "/index.html"
    std::string version = request.getHttpVersion(); // "HTTP/1.1"
}
```

**Implementation Details**:
- Uses `std::istringstream` for line-by-line parsing
- Handles both `\n` and `\r\n` line endings
- Stores headers in `std::map<std::string, std::string>`
- Empty line separates headers from body

### FileReader

**Purpose**: Read files from the filesystem within a configured root directory.

**Location**: `include/file_reader.hpp`, `src/file_reader.cpp`

**Responsibilities**:
- Check if requested files exist
- Read file contents into memory
- Determine appropriate Content-Type headers
- Maintain security by operating within root directory

**Usage Example**:
```cpp
FileReader reader("www");
if (reader.fileExists("/index.html")) {
    std::string content = reader.readFile("/index.html");
    std::string type = reader.getContentType("/index.html");
}
```

**Supported Content Types**:
- HTML/HTM → `text/html`
- CSS → `text/css`
- JS → `application/javascript`
- JSON → `application/json`
- JPEG/JPG → `image/jpeg`
- PNG → `image/png`
- GIF → `image/gif`
- TXT → `text/plain`
- Default → `application/octet-stream`

### HttpResponse

**Purpose**: Build properly formatted HTTP/1.0 responses.

**Location**: `include/http_response.hpp`, `src/http_response.cpp`

**Responsibilities**:
- Format status line with code and message
- Manage response headers
- Assemble complete HTTP response string

**Usage Example**:
```cpp
HttpResponse response = HttpResponse::createOkResponse(
    fileContent,
    "text/html"
);
std::string responseString = response.build();
```

**Factory Methods**:
- `createOkResponse(content, contentType)`: Creates 200 OK response
- `createNotFoundResponse(errorPage)`: Creates 404 response with custom or default error page

**Response Format**:
```
HTTP/1.0 200 OK
Content-Type: text/html
Content-Length: 1234

<html>...</html>
```

### Server

**Purpose**: Orchestrate the entire request/response cycle.

**Location**: `include/server.hpp`, `src/server.cpp`

**Responsibilities**:
- Initialize and manage server socket
- Accept client connections
- Receive and send data
- Coordinate HttpRequest, FileReader, and HttpResponse
- Handle errors gracefully

**Key Methods**:
- `start()`: Initialize socket, bind, and listen
- `run()`: Main event loop using `poll()`
- `handleClient(clientFd)`: Process individual client request
- `processRequest(request)`: Business logic for generating responses
- `stop()`: Clean up resources

## Request Flow

### 1. Client Connects
```cpp
// Server accepts connection
clientFd = accept(serverFd, ...);
```

### 2. Server Receives Request
```cpp
// Read data from socket
recv(clientFd, buffer, sizeof(buffer), 0);
```

### 3. Parse Request
```cpp
HttpRequest request;
request.parse(rawRequest);
```

### 4. Resolve File Path
```cpp
// Map "/" to "/index.html"
std::string filePath = resolveFilePath(request.getPath());
```

### 5. Check File Existence
```cpp
if (fileReader.fileExists(filePath)) {
    // File found - proceed
} else {
    // Return 404 response
}
```

### 6. Read File Content
```cpp
std::string content = fileReader.readFile(filePath);
std::string contentType = fileReader.getContentType(filePath);
```

### 7. Build Response
```cpp
HttpResponse response = HttpResponse::createOkResponse(
    content,
    contentType
);
```

### 8. Send Response
```cpp
std::string responseStr = response.build();
send(clientFd, responseStr.c_str(), responseStr.length(), 0);
```

### 9. Close Connection
```cpp
close(clientFd);
```

## Path Resolution

The server maps request paths to filesystem paths:

| Request Path | Filesystem Path |
|--------------|-----------------|
| `/` | `www/index.html` |
| `/hello.html` | `www/hello.html` |
| `/about.html` | `www/about.html` |
| `/css/style.css` | `www/css/style.css` |

## Error Handling

### 404 Not Found
When a requested file doesn't exist:
1. Check for custom error page at `www/errors/404.html`
2. If custom page exists, serve it
3. Otherwise, return default HTML error message

### 400 Bad Request
Returned when:
- HTTP method is not GET (POST/DELETE not yet supported)
- Request parsing fails

### 500 Internal Server Error
Reserved for server-side errors (not currently used)

## HTTP/1.0 Compliance

The server implements a minimal subset of HTTP/1.0:
- ✅ GET method
- ✅ Status codes (200, 404, 400)
- ✅ Content-Type and Content-Length headers
- ✅ Request line parsing
- ✅ Header parsing
- ❌ Persistent connections (each request closes connection)
- ❌ POST/DELETE methods (planned)
- ❌ Chunked transfer encoding

## Non-Blocking I/O

All socket operations use non-blocking I/O with `poll()`:
- Server socket set to non-blocking mode
- `poll()` waits for incoming connections
- Single connection handled at a time (will be expanded for multiple clients)

## Future Enhancements

Planned improvements:
1. **Multiple Clients**: Handle concurrent connections using poll with multiple file descriptors
2. **POST Support**: Accept and process request bodies
3. **DELETE Support**: Remove files from server
4. **Configuration**: Read settings from config file
5. **CGI**: Execute dynamic content scripts
6. **Request Body Limits**: Enforce maximum sizes
7. **Keep-Alive**: Support persistent connections
