#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "config.h"
#include "ipc.h"


// Create a pipe
int ipc_create(int pipefd[2])
{
    if (pipe(pipefd) == -1)
    {
        perror("Pipe creation error");
        return -1;
    }

    return 0;
}


// Write one audio frame to the pipe
int ipc_write(int fd, const int16_t *buffer)
{
    const char *data = (const char *)buffer;

    int total = 0;
    ssize_t result;

    while (total < FRAME_BYTES)
    {
        result = write(
            fd,
            data + total,
            FRAME_BYTES - total
        );

        if (result == -1)
        {
            if (errno == EINTR)
                continue;

            perror("IPC Write Error");
            return -1;
        }

        if (result == 0)
            return -1;

        total += result;
    }

    return 0;
}


// Read one audio frame from the pipe
int ipc_read(int fd, int16_t *buffer)
{
    char *data = (char *)buffer;

    int total = 0;
    ssize_t result;

    while (total < FRAME_BYTES)
    {
        result = read(
            fd,
            data + total,
            FRAME_BYTES - total
        );

        if (result == -1)
        {
            if (errno == EINTR)
                continue;

            perror("IPC Read Error");
            return -1;
        }

        if (result == 0)
        {
            if (total == 0)
                return 1;

            fprintf(stderr, "Incomplete audio frame\n");
            return -1;
        }

        total += result;
    }

    return 0;
}


// Close a pipe descriptor
void ipc_close(int fd)
{
    close(fd);
}
