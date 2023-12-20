#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "sbuffer.h"

#define NUM_OF_THREADS 3

sbuffer_t *buffer;
FILE *sensor_file_out;
pthread_mutex_t reader_mutex;

int writer_routine() {
    FILE * sensor_file_in = fopen("sensor_data", "r");
    do {
        sensor_data_t sensor_node;
        // read the data
        if (fread(&(sensor_node.id), sizeof(sensor_id_t), 1, sensor_file_in) == 0) {
            break;
        };
        sensor_node.value = (sensor_value_t*)malloc(sizeof(sensor_value_t));
        fread(sensor_node.value, sizeof(sensor_value_t), 1, sensor_file_in);
        fread(&(sensor_node.ts), sizeof(sensor_ts_t), 1, sensor_file_in);
        // insert into buffer.
        sbuffer_insert(buffer, &sensor_node);
    } while(1);
    printf("\n\n\nWRITER IS DONE\n\n\n");

    return 0;
}

sensor_data_t data;
int reader_routine() {
    do{
        // reading from buffer
        sbuffer_remove(buffer, &data);
        // writing to data_out file.
        if (data.id == 0 ) {
            break;
        } //else {
//            // QUESTION: WHY SIGFAULT ERROR RECEIVED?
//            pthread_mutex_lock(&reader_mutex);
//            fprintf(sensor_file_out,"%d,%f,%s", data.id, *data.value, ctime(&data.ts));
//            pthread_mutex_unlock(&reader_mutex);
//        }

    } while(1);
    printf("\n\n\nreading finished\n\n\n");
    return 0;
}

int main() {
    sbuffer_init(&buffer);
    sensor_file_out = fopen("sensor_data_out.csv", "w");

    pthread_mutex_init(&reader_mutex, NULL);
    pthread_t th[3];

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (i == 0) pthread_create(th, NULL, (void*)writer_routine, NULL);
        else pthread_create(th + i, NULL, (void *)reader_routine, NULL);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(th[i], NULL);
    }
    return 0;
}