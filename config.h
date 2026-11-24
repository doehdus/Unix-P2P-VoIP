#ifndef CONFIG_H
#define CONFIG_H

// Audio configuration
#define SAMPLE_RATE 16000
#define CHANNELS 1
#define FRAME_SIZE 512
#define BIT_DEPTH 16
#define FRAME_BYTES (FRAME_SIZE * CHANNELS * BIT_DEPTH / 8)

// Network configuration
#define DEFAULT_PORT 5000

// Shared memory configuration
#define RING_BUFFER_SIZE 16

// File configuration
#define RECORDING_DIR "recordings"
#define LOG_DIR "logs"

#endif
