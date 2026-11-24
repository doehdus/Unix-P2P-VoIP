#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "config.h"
#include "capture.h"
#include "playback.h"
#include "ipc.h"

#define TEST_SECONDS 10


// Capture child process
int run_capture(int fd)
{
    int16_t buffer[FRAME_SIZE];
    int overflow = 0;
    int result;
    int status = 0;

    printf("[CAPTURE] PID: %d\n", (int)getpid());

    if (capture_init() == -1)
        return 1;

    // Capture audio frames for 10 seconds
    for (int i = 0;
         i < SAMPLE_RATE * TEST_SECONDS / FRAME_SIZE;
         i++)
    {
        result = capture_read(buffer);

        if (result == -1)
        {
            status = 1;
            break;
        }

        if (result == 1)
            overflow++;

        if (ipc_write(fd, buffer) == -1)
        {
            status = 1;
            break;
        }
    }

    capture_close();

    printf("[CAPTURE] Overflow: %d\n", overflow);

    return status;
}


// Playback child process
int run_playback(int fd)
{
    int16_t buffer[FRAME_SIZE];
    int underflow = 0;
    int result;
    int status = 0;

    printf("[PLAYBACK] PID: %d\n", (int)getpid());

    if (playback_init() == -1)
        return 1;

    // Receive and play audio frames
    while (1)
    {
        result = ipc_read(fd, buffer);

        // End of pipe
        if (result == 1)
            break;

        if (result == -1)
        {
            status = 1;
            break;
        }

        result = playback_write(buffer);

        if (result == -1)
        {
            status = 1;
            break;
        }

        if (result == 1)
            underflow++;
    }

    playback_close();

    printf("[PLAYBACK] Underflow: %d\n", underflow);

    return status;
}


// Main process
int main(void)
{
    int pipefd[2];
    pid_t capture_pid;
    pid_t playback_pid;
    int capture_status;
    int playback_status;

    // Disable stdout buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    // Handle broken pipe as a write error
    signal(SIGPIPE, SIG_IGN);

    printf("=== UNIX Voice Communication ===\n");
    printf("[MAIN] PID: %d\n", (int)getpid());

    // Create IPC pipe
    if (ipc_create(pipefd) == -1)
        return 1;

    // Create capture process
    capture_pid = fork();

    if (capture_pid == -1)
    {
        perror("Capture fork error");

        ipc_close(pipefd[0]);
        ipc_close(pipefd[1]);

        return 1;
    }

    if (capture_pid == 0)
    {
        ipc_close(pipefd[0]);

        int result = run_capture(pipefd[1]);

        ipc_close(pipefd[1]);

        exit(result);
    }

    // Create playback process
    playback_pid = fork();

    if (playback_pid == -1)
    {
        perror("Playback fork error");

        ipc_close(pipefd[0]);
        ipc_close(pipefd[1]);

        kill(capture_pid, SIGTERM);
        waitpid(capture_pid, NULL, 0);

        return 1;
    }

    if (playback_pid == 0)
    {
        ipc_close(pipefd[1]);

        int result = run_playback(pipefd[0]);

        ipc_close(pipefd[0]);

        exit(result);
    }

    // Close unused pipe descriptors
    ipc_close(pipefd[0]);
    ipc_close(pipefd[1]);

    printf("[MAIN] Capture PID: %d\n",
           (int)capture_pid);

    printf("[MAIN] Playback PID: %d\n",
           (int)playback_pid);

    // Wait for child processes
    waitpid(capture_pid, &capture_status, 0);
    waitpid(playback_pid, &playback_status, 0);

    if (!WIFEXITED(capture_status) ||
        WEXITSTATUS(capture_status) != 0 ||
        !WIFEXITED(playback_status) ||
        WEXITSTATUS(playback_status) != 0)
    {
        printf("[MAIN] Audio test failed.\n");
        return 1;
    }

    printf("[MAIN] All processes terminated.\n");
    printf("[MAIN] Audio test completed.\n");

    return 0;
}
