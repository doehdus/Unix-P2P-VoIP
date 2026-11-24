#ifndef IPC_H
#define IPC_H

#include <stdint.h>
#include "config.h"

//IPC function declarations
int ipc_create(int pipefd[2]);
int ipc_write(int fd, const int16_t *buffer);
int ipc_read(int fd, int16_t *buffer);
void ipc_close(int fd);

#endif
