#include <stdio.h>
#include <stdbool.h>
#include "config.h"

FILE* open_db(char *filename, bool append) {
    FILE *fp;
    if (append == true) {
        fp = fopen(filename, "a");
    } else {
        fp = fopen(filename, "w");
    }

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
    fprintf(f, "%d, %lf, %ld \n", id, value, ts);
    return 0;
}

int close_db(FILE * f) {
    fclose(f);
    return 0;
}