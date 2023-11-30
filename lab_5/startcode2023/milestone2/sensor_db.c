#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include "logger.h"
//#include <fcntl.h>
//#include <sys/wait.h>

// status 1: file opening error
// status 2: fork failure
// status 3: writing to db file failure
// status 4: file failed to close
// status 5: pipe creation failure

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
        exit(1);
    }

    // Communicate with the log process
    write_to_log_process("Data file opened.");
    return fp;
}


int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (fprintf(f, "%d, %lf, %ld \n", id, value, ts) < 0) {
        write_to_log_process("ERROR: could not write to db file.");
        perror("Error writing to file");
        fclose(f);
        end_log_process();
        exit(3);
    }
//    printf("this is the parent process during insertion of data \n");
    // Communicate with the log process
    write_to_log_process("Data inserted.");
    return 0;
}

int close_db(FILE * f) {
    if ( fclose(f) == EOF) {
        write_to_log_process("ERROR: could not close db file.");
        perror("Error closing file");
        end_log_process();
        exit(4);
    };
    // Communicate with the log process
    write_to_log_process("Data file closed.");
    end_log_process();
    return 0;
}