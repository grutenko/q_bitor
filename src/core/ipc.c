#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#include "ipc.h"

int ipc_open_unix(char *path)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd == -1) return -1;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if( bind(fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1 ) return -1;
    if( listen(fd, 1) == -1 ) return -1;

    return fd;
}