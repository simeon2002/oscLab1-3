#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "logger.h"
#include <fcntl.h>
#define BUFFER_SIZE 100
// status 1: file opening error
// status 2: fork failure
// status 3:

/* definition of global file descriptors */


FILE* open_db(char *filename, bool append) {
    FILE *fp;
    if (append == true) {
         fp = fopen(filename, "a");
    }
    else {
        fp = fopen(filename, "w");
    }
    // error handle
    if (fp == NULL) {
        perror("The following error has occurred:");
        exit(1);
    }

    // use of pipeline
    int fd[2];
    pipe(fd);
    int pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(2);
    }

    if (pid == 0) {
        char received_message[BUFFER_SIZE];
        read(fd[0], received_message, sizeof(received_message));
        create_log_process();
        write_to_log_process(received_message);
        end_log_process();
    } else if(pid > 0) {
        close(fd[0]);
        char message[BUFFER_SIZE] = "A new csv file is created or an existing file has been opened.";
        write(fd[1], message, sizeof(message));
    }
    return fp;
    }

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    fprintf(f, "%d, %lf, %ld \n", id, value, ts);

    return 0;
}

int close_db(FILE * f) {
    fclose(f);
    return 0;
}