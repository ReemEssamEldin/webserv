# Configuration File

Webserv uses an NGINX-style configuration file to define server behavior, routes, and various settings.

## File Location

Default: `conf/webserv.conf`

You can specify a custom config file:
```bash
./webserv path/to/config.conf
```

## Configuration Syntax

The configuration file uses a simple block-based syntax inspired by NGINX.

### Comments
```nginx
# This is a comment
```

### Server Blocks

Each `server` block defines a separate web server instance:

```nginx
server {
    # Server configuration directives here
}
```

You can have multiple server blocks in one file to run multiple servers on different ports.

## Server Directives

### listen
Specifies the IP address and port to listen on.

**Syntax**: `listen host:port;` or `listen port;`

**Default**: `0.0.0.0:8080`

**Examples**:
```nginx
listen 8080;                    # Listen on all interfaces, port 8080
listen 0.0.0.0:8080;           # Explicit all interfaces
listen 127.0.0.1:8081;         # Listen only on localhost
```

### server_name
Sets the name of the virtual server.

**Syntax**: `server_name name;`

**Default**: `webserv`

**Example**:
```nginx
server_name example.com;
```

### client_max_body_size
Sets the maximum allowed size of client request body.

**Syntax**: `client_max_body_size size;`

**Default**: `1M` (1 megabyte)

**Supported units**: bytes (no suffix), K (kilobytes), M (megabytes)

**Examples**:
```nginx
client_max_body_size 1024;      # 1024 bytes
client_max_body_size 10K;       # 10 kilobytes
client_max_body_size 5M;        # 5 megabytes
```

### error_page
Defines custom error pages for specific HTTP status codes.

**Syntax**: `error_page code path;`

**Example**:
```nginx
error_page 404 /errors/404.html;
error_page 500 /errors/500.html;
```

### root
Sets the root directory for serving files.

**Syntax**: `root path;`

**Default**: `www`

**Example**:
```nginx
root /var/www/html;
```

## Location Blocks

Location blocks define configuration for specific URL paths:

```nginx
location /path {
    # Location-specific directives
}
```

### Location Directives

#### allow_methods
Specifies which HTTP methods are allowed for this location.

**Syntax**: `allow_methods method1 method2 ...;`

**Default**: All methods allowed

**Example**:
```nginx
location /api {
    allow_methods GET POST;
}
```

#### root
Overrides the server root for this location.

**Syntax**: `root path;`

**Example**:
```nginx
location /files {
    root /var/data;
}
```

#### index
Sets the default file to serve when a directory is requested.

**Syntax**: `index filename;`

**Default**: `index.html`

**Example**:
```nginx
location / {
    index home.html;
}
```

#### autoindex
Enables or disables directory listing.

**Syntax**: `autoindex on|off;`

**Default**: `off`

**Example**:
```nginx
location /files {
    autoindex on;    # Show directory listing
}
```

When enabled and no index file exists, the server generates an HTML page listing directory contents.

#### return
Creates an HTTP redirect.

**Syntax**: `return redirect_url;`

**Example**:
```nginx
location /old-page {
    return /new-page.html;    # 301 redirect
}
```

#### upload_path
Specifies where uploaded files should be stored (for POST requests).

**Syntax**: `upload_path directory;`

**Example**:
```nginx
location /upload {
    allow_methods POST DELETE;
    upload_path www/uploads;
}
```

#### cgi_extension
Maps file extensions to CGI interpreters.

**Syntax**: `cgi_extension .extension /path/to/interpreter;`

**Example**:
```nginx
location /cgi-bin {
    cgi_extension .php /usr/bin/php-cgi;
    cgi_extension .py /usr/bin/python3;
}
```

## Complete Example

```nginx
# Main website
server {
    listen 0.0.0.0:8080;
    server_name localhost;

    client_max_body_size 1M;

    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;

    root www;

    # Root location
    location / {
        allow_methods GET POST DELETE;
        index index.html;
        autoindex off;
    }

    # File uploads
    location /upload {
        allow_methods POST DELETE;
        upload_path www/uploads;
        autoindex on;
    }

    # CGI scripts
    location /cgi-bin {
        allow_methods GET POST;
        root www/cgi-bin;
        cgi_extension .php /usr/bin/php-cgi;
        cgi_extension .py /usr/bin/python3;
    }

    # Static files with listing
    location /files {
        allow_methods GET;
        root www/files;
        autoindex on;
    }

    # Redirect example
    location /old-page {
        return /new-page.html;
    }
}

# API server on different port
server {
    listen 0.0.0.0:8081;
    server_name api.localhost;

    client_max_body_size 10M;

    root www/api;

    location / {
        allow_methods GET POST PUT DELETE;
    }
}
```

## Path Resolution

### How Paths Are Resolved

1. Server receives request for `/location/path/file.html`
2. Finds matching location block (longest prefix match)
3. Determines root directory (location root or server root)
4. Strips location path from request path
5. Combines root + remaining path

**Example**:
```nginx
server {
    root www;

    location /files {
        root /var/data;
    }
}
```

- Request: `/files/test.txt`
- Location: `/files` (matched)
- Location root: `/var/data`
- Stripped path: `/test.txt`
- Final path: `/var/data/test.txt`

- Request: `/hello.html`
- Location: `/` (default, matched)
- Server root: `www`
- Final path: `www/hello.html`

## Configuration Features Status

### ✅ Implemented
- Multiple server blocks (interface:port pairs)
- Custom error pages
- Client body size limits
- Route/location configuration
- HTTP method restrictions per route
- HTTP redirects (301)
- Custom root directories per route
- Directory listing (autoindex)
- Default index files per route

### ⏳ Planned
- File upload handling (POST body parsing)
- CGI execution
- Request body size enforcement
- Multiple concurrent clients
