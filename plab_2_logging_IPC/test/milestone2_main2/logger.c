#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#define BUFFER_SIZE 100

// status 1: file opening error
// status 2: fork failure
// status 3: writing to log file failure
// status 4: file failed to close
// status 5: pipe creation failure
// status 6: writing to log file failure

/*global variables (for file opening + pipe)*/
FILE *fp;
int fd[2], pid;
int i = 0;

int create_log_process() {
    // log file opening
    fp = fopen("gateway.log", "a");
    if (fp == NULL) {
        perror("Error occurred during log file opening");
        exit(1);
    }
    // creation of communication and child process
    if (pipe(fd) == -1) {
        perror("Error occurred during pipe creation");
        exit(5);
    };
    pid = fork();
    // error handle
    if (pid == -1) {
        perror("fork failed");
        exit(2);
    }
    return 0;
}

int end_log_process() {
    if (fclose(fp) == EOF){
        perror("Error occurred during log file closing");
        //NO EXIT SYSCALL HERE! NOT A CRITICAL ERROR.
    }
    wait(NULL);
    close (fd[0]);
    close(fd[1]);
    return 0;
}


int write_to_log_process(char *msg) {
    if (pid == 0) { // child process
        char received_message[BUFFER_SIZE];
        // reading from pipe
        close(fd[1]);
        printf("%d child process\n", i);
        if (read(fd[0], received_message, BUFFER_SIZE) == -1) {
            perror("Error occurred during reading from pipe");
            close(fd[0]);
            close(fd[1]);
            exit(6);
        };
        close(fd[0]);

        // timestamp creation
        time_t timer;
        time(&timer);
        struct tm *timestamp_info;
        timestamp_info = localtime(&timer);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%a %b %d %X %Y", timestamp_info);

        // writing message to log file.
        if (fprintf(fp, "%d - %s - %s\n", i, timestamp, received_message) < 0 ){
            perror("Error occurred during writing to log file");
            close(fd[0]);
            close(fd[1]);
            fclose(fp);
        };
        exit(0); // successful log
    } else { // parent process
        // Increment the sequence number only for actual log entries
        i++;
        printf("parent process\n");
        // Write the message to the pipe in the parent process
        if (write(fd[1], msg, BUFFER_SIZE) == -1) {  // +1 to include the null terminator
            // opmerking!!! sizeof(msg) don't use, = size of the pointer! use buffer_size...
            // with buffer_size you know for sure the nbytes you want to copy at most + is configurable.
            perror("Error occurred during writing to pipe");
            //NO CRITICAL ERROR, SO NO PROCESS TERMINATION.
        }
        close(fd[1]);
        wait(NULL); // very important for the parent process to wait here such that we don't get zombie process!
        printf("The child process has finished and thus parent process as well.\n");
    }
    return 0;
}