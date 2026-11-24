#include <stdio.h>
#include <portaudio.h>

#include "config.h"
#include "capture.h"

// Audio input stream
static PaStream *capture_stream = NULL;


// Initialize microphone input
int capture_init(void)
{
    PaError err;

    err = Pa_Initialize();

    if (err != paNoError)
    {
        fprintf(stderr, "PortAudio Init Error: %s\n",
                Pa_GetErrorText(err));
        return -1;
    }

    // Open microphone stream
    err = Pa_OpenDefaultStream(
        &capture_stream,
        CHANNELS,
        0,
        paInt16,
        SAMPLE_RATE,
        FRAME_SIZE,
        NULL,
        NULL
    );

    if (err != paNoError)
    {
        fprintf(stderr, "Capture Open Error: %s\n",
                Pa_GetErrorText(err));
        Pa_Terminate();
        return -1;
    }

    // Start microphone stream
    err = Pa_StartStream(capture_stream);

    if (err != paNoError)
    {
        fprintf(stderr, "Capture Start Error: %s\n",
                Pa_GetErrorText(err));

        Pa_CloseStream(capture_stream);
        capture_stream = NULL;
        Pa_Terminate();

        return -1;
    }

    printf("[CAPTURE] Microphone initialized.\n");

    return 0;
}


// Read one audio frame
int capture_read(int16_t *buffer)
{
    PaError err;

    if (capture_stream == NULL)
        return -1;

    err = Pa_ReadStream(
        capture_stream,
        buffer,
        FRAME_SIZE
    );

    // Handle input overflow
    if (err == paInputOverflowed)
    {
        return 1;
    }

    if (err != paNoError)
    {
        fprintf(stderr, "Capture Read Error: %s\n",
                Pa_GetErrorText(err));
        return -1;
    }

    return 0;
}


// Release microphone resources
void capture_close(void)
{
    if (capture_stream != NULL)
    {
        Pa_StopStream(capture_stream);
        Pa_CloseStream(capture_stream);

        capture_stream = NULL;

        Pa_Terminate();

        printf("[CAPTURE] Microphone closed.\n");
    }
}
