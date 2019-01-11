#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>


// Play gong audio on separate thread.
void play_gong_audio_and_exit()
{
    const char *gong = "../audio/ding.wav";

    int audio_status = play_wav_audio(gong);
    if (audio_status == 1) {
        printf("Error: Could not play audio.\n");
        _exit(1);
    } else {
        _exit(0);
    }
}


int play_wav_audio(const char *filename)
{
    // With aplay command and null termination this allows for filenames up to
    // length 235.
    char command[256];
    int status;

    // Prepare and execute syscall command.
    sprintf(command, "aplay -c 1 -q -t wav %s", filename);
    status = system(command);
     
    return status;
}


void begin_session(const char *session_type, const int minutes)
{
    printf("%s time started.\n", session_type);

    unsigned int time_remaining = (minutes * 60) + 1;
    do {
        unsigned int time_slept = sleep(1);
        time_remaining -= 1;
        printf("\r%02dm %02ds", time_remaining / 60, time_remaining % 60);
        fflush(stdout);
        //printf("\r%d", time_remaining);
    } while (time_remaining);
    printf("\n");

    printf("%s time ended.\n", session_type);

    pid_t p = fork();
    if (p < 0) {
        // Fork has failed.
        printf("Fork failure\n");
        exit(1);
    } else if (p == 0) {
        play_gong_audio_and_exit();
    } else {
        // Ignore child exit code -- we don't care too much if the sound plays
        // successfully or not. This prevents it becoming a zombie process,
        // but is a "quick and dirty" approach.
        signal(SIGCHLD, SIG_IGN);
    }
}


int main(int argc, char **argv)
{
    int work_length, break_length;
    if (argc < 3 || sscanf (argv[2], "%i", &break_length) != 1) {
        // No break length argument specified or argument is not an int.
        // Use default value.
        break_length = 5;
    }
    if (argc < 2 || sscanf (argv[1], "%i", &work_length) != 1) {
        // No work length argument specified or argument is not an int.
        // Use default value.
        work_length = 25;
    }

    while (1) {
        begin_session("Work", work_length);
        begin_session("Break", break_length);
    }

    return 0;
}