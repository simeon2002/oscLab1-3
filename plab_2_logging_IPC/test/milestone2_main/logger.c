#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<sys/wait.h>
#define BUFFER_SIZE 100

FILE *fp;
int fd[2], pid;
int i = 0;
char received_message[BUFFER_SIZE];

int create_log_process() {
    // log file opening
    fp = fopen("gateway.log", "a");

    // creation of communication and child process
    pipe(fd);
    pid = fork();
    // error handle
    if (pid == -1) {
        perror("fork failed");
        exit(2);
    }
    return 0;
}

int end_log_process() {
    fclose(fp);
    return 0;
}


int write_to_log_process(char *msg) {
    if (pid == 0) { // child process
        // reading from pipe
        close(fd[1]);
        read(fd[0], received_message, sizeof(received_message));
        printf("testing\n");
        close(fd[0]);

        // timestamp creation
        time_t timer;
        time(&timer);
        struct tm *timestamp_info;
        timestamp_info = localtime(&timer);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%a %b %d %X %Y", timestamp_info);
        // writing message to log file.
        fprintf(fp, "%d - %s - %s\n", i, timestamp, msg);
        // Increment the sequence number only for actual log entries
        i++;
    } else { // parent process

        printf("testing parent\n");
        // Write the message to the pipe in the parent process
        close(fd[0]);
        write(fd[1], msg, sizeof(msg) + 1);  // +1 to include the null terminator
        close(fd[1]);
    }
    return 0;
}



// questions:
// 1. do I need to declare the sequence number on the top of the file?
// 2. to be able to access the FP in write_to_log_process, the only way to do this would be to
//    initialize the file pointer as a global variable, correct? follow up: is this a better
//    practice than just providing it as an argument to the function?? Thanks in advance.