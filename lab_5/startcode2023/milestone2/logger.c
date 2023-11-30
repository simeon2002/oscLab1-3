#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
//#include <errno.h>
#define BUFFER_SIZE 1000
#define LOG_FILE "gateway.log"
// status 1: file opening error
// status 2: fork failure
// status 3: writing to log file failure
// status 4: file failed to close
// status 5: pipe creation failure
// status 6: writing to log file failure
// status 7: error occurred during reading from pipe

int fd[2];
int i = 0;
FILE *fp;

/*global variables (for file opening + pipe)*/
int end_log_process() {
    close(fd[1]);
    fclose(fp);
    int status;
    wait(&status);
    if (WIFEXITED(status)) { // checking the exit status of child to determine type of error via status code.
        int exit_status = WEXITSTATUS(status);
        if (exit_status != 0) {
            fprintf(stderr, "Child process exit with status %d", exit_status);
        }
    } else perror("Error occurred during wait"); // --> indicates error occurrence during waiting time
    // or if child process is still running, i.e. it became a zombie process !!
    return 0;
}

int create_log_process() {
    // log file opening
    fp = fopen(LOG_FILE, "a");
    if (fp == NULL) {
        perror("Error occurred during log file opening");
        exit(1);
    }
    // creation of communication and child process
    if (pipe(fd) == -1) {
        perror("Error occurred during pipe creation");
        exit(5);
    };
    int pid = fork();
    // error handle
    if (pid == -1) {
        perror("fork failed");
        close(fd[0]);
        close(fd[1]);
        exit(2);
    } else if (pid == 0) {
        // opening log file.
//        setbuf(fp, NULL); // Disable buffering
        close(fd[1]); // closing write_end.
        char received_message[BUFFER_SIZE];
        size_t message_length;
        while(read(fd[0], &message_length, sizeof(size_t)) > 0) {

            if ((read(fd[0], received_message, message_length)) < 0) {
                perror("An error occurred during reading from pipe");
                fclose(fp);
                close(fd[0]);
                exit(7);
            }
//            printf("child process reads: %s \n", received_message);

            // timestamp creation.
            time_t timer;
            time(&timer);
            struct tm *timestamp_info;
            timestamp_info = localtime(&timer);
            char timestamp[25];
            strftime(timestamp, sizeof(timestamp), "%a %b %d %X %Y", timestamp_info);

            // writing message to log file.
            if (fprintf(fp, "%d - %s - %s\n", i++, timestamp, received_message) < 0) {
                perror("Error occurred during writing to log file");
                fclose(fp);
                close(fd[0]);
                exit(6); // error during writing to file
            }
            fflush(fp); // helps writing to log file without data to be buffered first.
        }
        fclose(fp);
        close(fd[0]);
        exit(0); // success status.
    } else {
        close(fd[0]);
    }
    return 0;
}

int write_to_log_process(char *msg) {
    size_t msg_length = strlen(msg) + 1;
    write(fd[1], &msg_length, sizeof(size_t)); // writing the length to be written.
    if (write(fd[1], msg, strlen(msg) + 1) == -1) {  // writing the message based on the length to be written.
        perror("Error occurred during writing to pipe");
        end_log_process();
        exit(3);
    }
    return 0;
}



// if using close() in else statement of parent process:
/*
 *In this case, the read end of the pipe is closed in the parent process immediately after the fork.
 * This means the parent process will not read from the pipe, and if the child process writes to the pipe,
 * the parent won't be waiting for it.
 * */

// question:
/* can I do:
 *      if (pid == 0) {
        close(fd[0]);
        exit(0);
    } in de end log process instead of insde of the child process in the create log process?
    */

// question: why do I need to send the messange boundary first to be able to receive all of the message
// if f.e. parent process continues and data is inserted three times, I only get it once. so.. why do I need to use
// these message boundaries when writing to specifically check for this? does the data that is not read yet, get lost
// when reading the first bytes or something??