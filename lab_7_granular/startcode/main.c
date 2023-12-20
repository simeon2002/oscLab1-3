#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "sbuffer.c"
// TODO: question: if I were to use ontinge lock on removing and one lock on prin
// TODO: the sensor records would sometimes be switches up f.e. 37 and 49 would switch up as it would wait twice for the lock basically.
// thus.. should I use a lock for the complete code to be a crticial section or what...


// todo: - ask above question
// todo: do error handling for all functions.

#define NUM_OF_THREADS 3

sbuffer_t *buffer;
FILE *sensor_file_out;
pthread_mutex_t reader_mutex;

int writer_routine() {
    FILE * sensor_file_in = fopen("sensor_data", "r");
    if (sensor_file_in == NULL) {
        perror("Error opening sensor_data");
        return EXIT_FAILURE;
    }
    sensor_data_t sensor_node;
    do {
        // read the data
        size_t read1 = fread(&(sensor_node.id), sizeof(sensor_id_t), 1, sensor_file_in);
        if (read1 == 0) {
            if (feof(sensor_file_in)) { // end-of-file , no error
                break;
            } else { // file error: exit occurs as this is critical to the functioning
                perror("Error reading sensor_id from file");
                fprintf(stderr, "Stopping execution of program");
                return EXIT_FAILURE;
            }
        }

        size_t read2 = fread(&sensor_node.value, sizeof(sensor_value_t), 1, sensor_file_in);
        if (read2 != 1) {
                perror("Error reading temperature from file");
                fprintf(stderr, "Stopping execution of program");
            return EXIT_FAILURE;
        }

        size_t read3 = fread(&(sensor_node.ts), sizeof(sensor_ts_t), 1, sensor_file_in);
        if (read3 != 1) {
                perror("Error reading timestamp from file");
                fprintf(stderr, "Stopping execution of program");
                return EXIT_FAILURE;
        }

        // writing data to buffer
        printf("writing\n");
        sbuffer_insert(buffer, &sensor_node); //TODO
        usleep(10000);
    } while(1);
    fclose(sensor_file_in);
    sensor_node.id = 0;
    sensor_node.value = 0;
    sensor_node.ts = 0;
    printf("\n\n\nWRITER IS DONE\n\n\n");
    for (int i = 0; i < NUM_OF_THREADS - 1; i++) { // posting such that the other threads can also leave the routine.
        sbuffer_insert(buffer, &sensor_node); //TODO
    }

    return EXIT_SUCCESS;
}
int reader_routine() {
    // reading data from buffer + writing to file
    sensor_data_t data;
    do{
        // reading from buffer
        sbuffer_remove(buffer, &data); //TODO
        printf("%lu reading from buffer: %d, %f, %s", pthread_self(), data.id, data.value, ctime(&data.ts));
        usleep(25000);
        pthread_mutex_lock(&reader_mutex);
        if (data.id != 0) fprintf(sensor_file_out, "%d,%f,%lu\n", data.id, data.value, data.ts);
        pthread_mutex_unlock(&reader_mutex);
        if (data.id == 0) {
            printf("reading is done");
            break;
        }
    } while(1);
    printf("\n\n\nreading finished\n\n\n");
    return EXIT_SUCCESS;
}

int main() {
    // buffer initialization
    sbuffer_init(&buffer); //TODO

    // Initialize mutex
    if (pthread_mutex_init(&reader_mutex, NULL) != 0) {
        perror("Mutex initialization failed \n");
        exit(EXIT_FAILURE);
    }

    // opening writing sensor file
    sensor_file_out = fopen("sensor_data_out.csv", "w");
    if (sensor_file_out == NULL) {
        perror("Error opening sensor_data_out.csv");
        exit(EXIT_FAILURE);
    }

    // create threads
    pthread_t th[NUM_OF_THREADS];

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (i == 0) {
            if (pthread_create(th, NULL, (void *) writer_routine, NULL) != 0) {
                perror("Error during creation of writer thread\n");
                exit(EXIT_FAILURE);
            }
            else {
                if (pthread_create(th + i, NULL, (void *)reader_routine, NULL) != 0) {
                    perror("Error during creation of reader thread\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    // waiting for threads to finish
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Error joining thread.\n");
            exit(EXIT_FAILURE);
        }
    }
    // clean up of resources
    sbuffer_free(&buffer); //TODO
    fclose(sensor_file_out);
    return 0;
}