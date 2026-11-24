#ifndef CAPTURE_H
#define CAPTURE_H

#include <stdint.h>
#include "config.h"

// Capture function declarations
int capture_init(void);
int capture_read(int16_t *buffer);
void capture_close(void);

#endif
