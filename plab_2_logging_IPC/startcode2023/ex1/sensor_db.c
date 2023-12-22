#include <stdio.h>
#include <stdbool.h>
#include "config.h"

FILE* open_db(char *filename, bool append) {
    if (append == true) {
        return fopen(filename, "a");
    }
    return fopen(filename, "w");
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    fprintf(f, "%d, %lf, %ld \n", id, value, ts);
    return 0;
}

int close_db(FILE * f) {
    fclose(f);
    return 0;
}