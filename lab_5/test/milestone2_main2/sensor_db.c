#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "logger.h"
#include <fcntl.h>
#include <sys/wait.h>
// status 1: file opening error
// status 2: fork failure
// status 3: writing to db file failure
// status 4: file failed to close
// status 5: pipe creation failure

/* definition of global vars */


FILE* open_db(char *filename, bool append) {
    FILE *fp;
    if (append == true) {
        fp = fopen(filename, "a");
    } else {
        fp = fopen(filename, "w");
    }
    create_log_process();
    // Error handling for file opening
    if (fp == NULL) {
        write_to_log_process("ERROR: could not open db file.");
        perror("Error occurred during file opening");
        exit(1);
    }

    // Communicate with the log process
//    create_log_process(); // Create the log process once before opening the data file
    write_to_log_process("Data file opened.");
//    end_log_process();
    return fp;
}


int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (fprintf(f, "%d, %lf, %ld \n", id, value, ts) < 0) {
//        create_log_process();
        write_to_log_process("ERROR: could not write to db file.");
//        end_log_process();
        perror("Error writing to file");
        fclose(f);
        exit(3);
    }

    // Communicate with the log process
//    create_log_process();
    write_to_log_process("Data inserted.");
//    end_log_process();
    return 0;
}

int close_db(FILE * f) {
    if ( fclose(f) == EOF) {
//        create_log_process();
        write_to_log_process("ERROR: could not close db file.");
//        end_log_process();
        perror("Error closing file");
        exit(4);
    };
    // Communicate with the log process
//     create_log_process();
    write_to_log_process("Data file closed.");

    end_log_process();

    return 0;
}