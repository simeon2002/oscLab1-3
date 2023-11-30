#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "sbuffer.c"

#define NUM_OF_THREADS 3

sbuffer_t *buffer;
sem_t rw_mutex;
sem_t buffer_count;
pthread_mutex_t reader_mutex;
FILE *sensor_file_out;
long read_count[10000];
int writer_routine() {
    FILE * sensor_file_in = fopen("sensor_data", "r");
    do {
        sensor_data_t sensor_node;
        if (fread(&(sensor_node.id), sizeof(sensor_id_t), 1, sensor_file_in) == 0) {
            break;
        };
        sensor_node.value = (sensor_value_t*)malloc(sizeof(sensor_value_t));
        fread(sensor_node.value, sizeof(sensor_value_t), 1, sensor_file_in);
        fread(&(sensor_node.ts), sizeof(sensor_ts_t), 1, sensor_file_in);
        printf("writing to buffer.\n");
        sem_wait(&rw_mutex);
        sbuffer_insert(buffer, &sensor_node);
        sem_post(&buffer_count);
        sem_post(&rw_mutex);
    } while(1);
    printf("\n\n\nWRITER IS DONE\n\n\n");
    sensor_data_t sensor_node;
    sensor_node.id = 0;
    sensor_node.value = (sensor_value_t*) 0;
    sensor_node.ts = (sensor_ts_t) 0;
    sem_wait(&rw_mutex);
    sbuffer_insert(buffer, &sensor_node);
    sem_post(&rw_mutex);
    for (int i = 0; i < (NUM_OF_THREADS - 1); i++) { // posting signal for every reader thread.
        sem_post(&buffer_count);
    }
    return 0;
}
sensor_data_t data;
long reader_thread;
long count;
int reader_routine() {
    do{

        // reading from buffer
        pthread_mutex_lock(&reader_mutex);
        sem_wait(&buffer_count);
        sem_wait(&rw_mutex);
        sbuffer_remove(buffer, &data);
        sem_post(&rw_mutex);
        pthread_mutex_unlock(&reader_mutex);

        // writing to data_out file.
        if (data.id == 0 ) {
            break;
        } else {
            // QUESTION: WHY SIGFAULT ERROR RECEIVED?
            pthread_mutex_lock(&reader_mutex);
            fprintf(sensor_file_out,"%d,%f,%s", data.id, *data.value, ctime(&data.ts));
            pthread_mutex_unlock(&reader_mutex);
        }

    } while(1);
    printf("\n\n\nreading finished\n\n\n");
    return 0;
}

int main() {
    sbuffer_init(&buffer);
    sensor_file_out = fopen("sensor_data_out.csv", "w");

    sem_init(&rw_mutex, 0, 1);
    sem_init(&buffer_count, 0, 0);
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