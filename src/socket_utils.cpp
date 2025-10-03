#include "socket_utils.hpp"
#include <sys/socket.h>
#include <cerrno>

#ifdef __APPLE__
    #include <fcntl.h>
#endif

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
