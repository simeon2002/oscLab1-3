#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "sbuffer.h"

#define NUM_OF_THREADS 3

// global variables
sbuffer_t *buffer;
FILE *sensor_file_out;
pthread_mutex_t reader_mutex;

int writer_routine() {
    // opening file to read from
    FILE * sensor_file_in = fopen("sensor_data", "r");
    if (sensor_file_in == NULL) {
<<<<<<< HEAD
        perror("Error opening sensor_data");
        return EXIT_FAILURE;
=======
        perror("Error occurred during opening read file");
        exit(EXIT_FAILURE);
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109
    }
    sensor_data_t sensor_node;
    do {
        // read the data
        size_t read1 = fread(&(sensor_node.id), sizeof(sensor_id_t), 1, sensor_file_in);
        if (read1 == 0) {
<<<<<<< HEAD
            if (feof(sensor_file_in)) { // end-of-file , no error
                break;
            } else { // file error: exit occurs as this is critical to the functioning
                perror("Error reading sensor_id from file");
                fprintf(stderr, "Stopping execution of program");
                return EXIT_FAILURE;
            }
=======
            if (feof(sensor_file_in)) {
                break;
            } else {
                perror("Error reading sensor_id from file");
                fprintf(stderr, "Stopping execution of program");
                exit(EXIT_FAILURE);
            }
        } else if (read1 != 1) {
            perror("Error reading sensor_id from file");
            fprintf(stderr, "Stopping execution of program");
            exit(EXIT_FAILURE);
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109
        }

        size_t read2 = fread(&sensor_node.value, sizeof(sensor_value_t), 1, sensor_file_in);
        if (read2 != 1) {
<<<<<<< HEAD
                perror("Error reading temperature from file");
                fprintf(stderr, "Stopping execution of program");
            return EXIT_FAILURE;
=======
            perror("Error reading temperature from file");
            fprintf(stderr, "Stopping execution of program");
            exit(EXIT_FAILURE);
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109
        }

        size_t read3 = fread(&(sensor_node.ts), sizeof(sensor_ts_t), 1, sensor_file_in);
        if (read3 != 1) {
<<<<<<< HEAD
                perror("Error reading timestamp from file");
                fprintf(stderr, "Stopping execution of program");
                return EXIT_FAILURE;
=======
            perror("Error reading timestamp from file");
            fprintf(stderr, "Stopping execution of program");
            exit(EXIT_FAILURE);
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109
        }

        // writing data to buffer
        printf("writing\n");
<<<<<<< HEAD
        sbuffer_insert(buffer, &sensor_node); //TODO
=======
        if (sbuffer_insert(buffer, &sensor_node) == SBUFFER_FAILURE) {
            perror("Error occurred during insertion of data into buffer");
            exit(EXIT_FAILURE);
        }
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109
        usleep(10000);
    } while(1);
    fclose(sensor_file_in);
    sensor_node.id = 0;
    sensor_node.value = 0;
    sensor_node.ts = 0;
    printf("\n\n\nWRITER IS DONE\n\n\n");
    for (int i = 0; i < NUM_OF_THREADS - 1; i++) { // posting such that the other threads can also leave the routine.
<<<<<<< HEAD
        sbuffer_insert(buffer, &sensor_node); //TODO
=======
        if (sbuffer_insert(buffer, &sensor_node) == SBUFFER_FAILURE) {
            perror("Error occurred during insertion of data into buffer");
            exit(EXIT_FAILURE);
        }
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109
    }

    return EXIT_SUCCESS;
}
int reader_routine() {
    // reading data from buffer + writing to file
    sensor_data_t data;
    do{
        // reading from buffer
<<<<<<< HEAD
        sbuffer_remove(buffer, &data); //TODO
=======
        if (sbuffer_remove(buffer, &data) == SBUFFER_FAILURE) {
            perror("Error occurred during removal of data from buffer");
            exit(EXIT_FAILURE);
        }
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109
        printf("%lu reading from buffer: %d, %f, %s", pthread_self(), data.id, data.value, ctime(&data.ts));
        if (pthread_mutex_lock(&reader_mutex) != 0) {
            perror("Critical error during locking mutex");
            exit(EXIT_FAILURE);
        }
        if (data.id != 0) {
            if (fprintf(sensor_file_out, "%d,%f,%lu\n", data.id, data.value, data.ts) < 0) {
                perror("Error during writing to output file");
                pthread_mutex_unlock(&reader_mutex);
                exit(EXIT_FAILURE);
            }
        }
        if (pthread_mutex_unlock(&reader_mutex) != 0) {
            perror("Critical error unlocking mutex");
            exit(EXIT_FAILURE);
        }
        usleep(25000);
        if (data.id == 0) {
            break;
        }
    } while(1);
    printf("\n\n\nreading finished\n\n\n");
    return EXIT_SUCCESS;
}

int main() {
<<<<<<< HEAD
    // buffer initialization
    sbuffer_init(&buffer); //TODO
=======
    // Buffer initialization
    if (sbuffer_init(&buffer) == SBUFFER_FAILURE) {
        perror("Error occurred during initialization of buffer");
        return EXIT_FAILURE;
    }
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109

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
<<<<<<< HEAD
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
=======
    }

    //threads creation
    pthread_t th[3];
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (i == 0) {
            if (pthread_create(th, NULL, (void*)writer_routine, NULL) != 0) {
                perror("Writer thread creation failed");
                exit(EXIT_FAILURE);
            }
            continue;
        }
        else {
            if (pthread_create(th + i, NULL, (void *)reader_routine, NULL) != 0){
                perror("Reader thread creation failed");
                exit(EXIT_FAILURE);
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

    // freeing resources
    if (sbuffer_free(&buffer) == SBUFFER_FAILURE) { // NO CRITICAL ERROR!
        perror("Error occurred during freeing of the resources");
    }
>>>>>>> 3131320fee5b7d98d4e8601761339d8d73a7c109
    fclose(sensor_file_out);
    return 0;
}