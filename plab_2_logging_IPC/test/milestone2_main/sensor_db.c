#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "logger.h"
#include <fcntl.h>
// status 1: file opening error
// status 2: fork failure
// status 3:

FILE* open_db(char *filename, bool append) {
    FILE *fp;
    if (append == true) {
        fp = fopen(filename, "a");
    } else {
        fp = fopen(filename, "w");
    }
    create_log_process(); // create the child process basically.

    // Error handling for file opening
    if (fp == NULL) {
        write_to_log_process("Error occurred during file opening.");
        perror("The following error has occurred:");
        exit(1);
    }

    // file opening success
    write_to_log_process("Data file opened.");
    return fp;
}


int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    fprintf(f, "%d, %lf, %ld \n", id, value, (long)ts);
    // Communicate with the log process
    write_to_log_process("Data inserted.");
    return 0;
}

int close_db(FILE * f) {
    fclose(f);
    // Communicate with the log process
    write_to_log_process("Data file closed.");
    end_log_process();
    return 0;
}