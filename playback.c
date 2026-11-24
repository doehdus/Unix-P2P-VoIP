#include <stdio.h>
#include <portaudio.h>

#include "config.h"
#include "playback.h"

// Audio output stream
static PaStream *playback_stream = NULL;


// Initialize audio output
int playback_init(void)
{
    PaError err;

    err = Pa_Initialize();

    if (err != paNoError)
    {
        fprintf(stderr, "PortAudio Init Error: %s\n",
                Pa_GetErrorText(err));
        return -1;
    }

    // Open audio output stream
    err = Pa_OpenDefaultStream(
        &playback_stream,
        0,
        CHANNELS,
        paInt16,
        SAMPLE_RATE,
        FRAME_SIZE,
        NULL,
        NULL
    );

    if (err != paNoError)
    {
        fprintf(stderr, "Playback Open Error: %s\n",
                Pa_GetErrorText(err));

        Pa_Terminate();
        return -1;
    }

    // Start audio output stream
    err = Pa_StartStream(playback_stream);

    if (err != paNoError)
    {
        fprintf(stderr, "Playback Start Error: %s\n",
                Pa_GetErrorText(err));

        Pa_CloseStream(playback_stream);
        playback_stream = NULL;
        Pa_Terminate();

        return -1;
    }

    printf("[PLAYBACK] Audio output initialized.\n");

    return 0;
}


// Play one audio frame
int playback_write(const int16_t *buffer)
{
    PaError err;

    if (playback_stream == NULL)
        return -1;

    err = Pa_WriteStream(
        playback_stream,
        buffer,
        FRAME_SIZE
    );

    // Handle output underflow
    if (err == paOutputUnderflowed)
    {
        return 1;
    }

    if (err != paNoError)
    {
        fprintf(stderr, "Playback Write Error: %s\n",
                Pa_GetErrorText(err));

        return -1;
    }

    return 0;
}


// Release audio output resources
void playback_close(void)
{
    if (playback_stream != NULL)
    {
        Pa_StopStream(playback_stream);
        Pa_CloseStream(playback_stream);

        playback_stream = NULL;

        Pa_Terminate();

        printf("[PLAYBACK] Audio output closed.\n");
    }
}
