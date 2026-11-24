#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <stdint.h>
#include "config.h"

//Playback function declarations
int playback_init(void);
int playback_write(const int16_t *buffer);
void playback_close(void);

#endif
