#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include "logger.h"
//#include <fcntl.h>
//#include <sys/wait.h>
// status 1: file opening error
#define FILE_OPENING_ERROR 1
// status 2: fork failure
#define FORK_FAILURE 2
// status 3: writing to db file failure
#define SENSOR_DB_WRITE_ERROR 3
// status 4: file failed to close
#define FILE_CLOSING_ERROR 4
// status 5: pipe creation failure
#define PIPE_CREATION_ERROR 5

/* definition of global vars */


FILE* open_db(char *filename, bool append) {
    // creation logging process
    create_log_process();

    FILE *fp;
    if (append == true) {
        fp = fopen(filename, "a");
    } else {
        fp = fopen(filename, "w");
    }
    // Error handling for file opening
    if (fp == NULL) {
        write_to_log_process("ERROR: could not open db file.");
        perror("Error occurred during file opening");
        end_log_process();
        exit(FILE_OPENING_ERROR);
    }

    // Communicate with the log process
    write_to_log_process("DB file opened.");
    return fp;
}


int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (fprintf(f, "%d, %lf, %ld \n", id, value, ts) < 0) {
        write_to_log_process("ERROR: could not write to DB file.");
        perror("Error writing to file");
        fclose(f);
        end_log_process();
        return SENSOR_DB_WRITE_ERROR;
    }
//    printf("this is the parent process during insertion of data \n");
    // Communicate with the log process
    write_to_log_process("Data inserted.");
    return 0;
}

int close_db(FILE * f) {
    if ( fclose(f) == EOF) {
        write_to_log_process("ERROR: could not close DB file.");
        perror("Error closing file");
        end_log_process();
        return FILE_CLOSING_ERROR;
    };
    // Communicate with the log process
    write_to_log_process("Data file closed.");
    end_log_process();
    return 0;
}