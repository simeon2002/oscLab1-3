#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "sbuffer.c"

#define NUM_OF_THREADS 3

sbuffer_t *buffer;
FILE *sensor_file_out;
pthread_mutex_t reader_mutex;
int writer_routine() {
    FILE * sensor_file_in = fopen("sensor_data", "r");
    sensor_data_t sensor_node;
    do {
        // read the data
        if (fread(&(sensor_node.id), sizeof(sensor_id_t), 1, sensor_file_in) == 0) {
            break;
        };
        fread(&sensor_node.value, sizeof(sensor_value_t), 1, sensor_file_in);
        fread(&(sensor_node.ts), sizeof(sensor_ts_t), 1, sensor_file_in);
//        printf("%lu reading from buffer: %d, %f, %s", pthread_self(), sensor_node.id, sensor_node.value, ctime(&sensor_node.ts));
        // writing data to buffer
        printf("writing\n");
        sbuffer_insert(buffer, &sensor_node);
    } while(1);
    sensor_node.id = 0;
    sensor_node.value = 0;
    sensor_node.ts = 0;
    sbuffer_insert(buffer, &sensor_node);
    fclose(sensor_file_in);
    printf("\n\n\nWRITER IS DONE\n\n\n");
    for (int i = 0; i < NUM_OF_THREADS - 1; i++) {
        sem_post(&buffer_count);
    }

    return 0;
}
bool transfer_done = false;
int reader_routine() {
    // reading data from buffer + writing to file
    sensor_data_t data;
    do{
        // reading from buffer
         // TODO: question: if I were to use one lock on removing and one lock on printing
        // TODO: the sensor records would sometimes be switches up f.e. 37 and 49 would switch up as it would wait twice for the lock basically.
        // thus.. should I use a lock for the complete code to be a crticial section or what...
        pthread_mutex_lock(&reader_mutex);
        sbuffer_remove(buffer, &data);
        if (transfer_done == true) break;
        printf("%lu reading from buffer: %d, %f, %s", pthread_self(), data.id, data.value, ctime(&data.ts));
        usleep(10);
        if (data.id != 0) fprintf(sensor_file_out, "%d,%f,%lu\n", data.id, data.value, data.ts);
        pthread_mutex_unlock(&reader_mutex);
        if (data.id == 0) {
            transfer_done = true;
            printf("reading is done");
            break;
        }
    } while(1);
    printf("\n\n\nreading finished\n\n\n");
    return 0;
}

int main() {
    sbuffer_init(&buffer);
    pthread_mutex_init(&reader_mutex, NULL);
    sensor_file_out = fopen("sensor_data_out.csv", "w");
    pthread_t th[3];

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (i == 0) pthread_create(th, NULL, (void*)writer_routine, NULL);
        else pthread_create(th + i, NULL, (void *)reader_routine, NULL);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(th[i], NULL);
    }
    sbuffer_free(&buffer);
    return 0;
}