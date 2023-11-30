// question 1: is this correct for line 51? lock necessary via semaphore! as mutexes are only for the same thread!

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "sbuffer.c"
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#define NUM_OF_THREADS 3

#define ERROR_HANDLER(condition, ...)    do {                       \
                      if (condition) {                              \
                        printf("\nError: in %s - function %s at line %d: %s\n", __FILE__, __func__, __LINE__, __VA_ARGS__); \
                        exit(EXIT_FAILURE);                         \
                      }                                             \
                    } while(0)

// global variables
sbuffer_t *buffer;
sem_t buffer_count_mutex;
sem_t rw_mutex;
pthread_mutex_t reader_signal_mutex;
long long num_of_sensor_readings;

FILE* open_file(char * file_name) {
    FILE* file = fopen(file_name, "r");
    ERROR_HANDLER(file == NULL, "File was not able to open.");
    return file;
}

// writer routine
int writer_routine() {
    // opening file
    FILE* sensor_file_in = open_file("sensor_data");
    // reading file data
    struct stat sb; // == properties of the file.
    if (fstat(fileno(sensor_file_in), &sb) == -1) {
        perror("fstat");
        exit(2);
    }

    /* get num of sensor reading*/
    num_of_sensor_readings = (long long) sb.st_size /
                                       (long long) (sizeof(sensor_id_t) +
                                       sizeof(sensor_value_t)+sizeof(sensor_ts_t));

    /*reading from file + writing to buffer*/
    for (int i = 0; i < num_of_sensor_readings; i++) {
        /* reading sensor data */
        sensor_data_t sensor_node;
        fread(&(sensor_node.id), sizeof(sensor_id_t), 1, sensor_file_in);
        fread(&(sensor_node.value), sizeof(sensor_value_t), 1, sensor_file_in);
        fread(&(sensor_node.ts), sizeof(sensor_ts_t), 1, sensor_file_in);
        usleep(10000);
        sem_wait(&rw_mutex);
        sem_post(&buffer_count_mutex);
        sbuffer_insert(buffer, &sensor_node);
        printf("written to buffer, releasing lock \n\n");
        sem_post(&rw_mutex);
        sleep(2);
    }
    fclose(sensor_file_in);
    printf("I was finished");
    return 0;
}

sensor_data_t sensor_data;
int reader_routine() {
    for (int i = 0; i < num_of_sensor_readings; i++) {
        pthread_mutex_lock(&reader_signal_mutex);
        sem_wait(&buffer_count_mutex);
        sem_wait(&rw_mutex);
        pthread_mutex_lock(&reader_signal_mutex);
        sbuffer_remove(buffer, &sensor_data); // here is the issue.
        printf("read from buffer: sensor_id = %d \n", sensor_data.id);
        sem_post(&rw_mutex);
    }
    printf("reading was a success");
    return 0;
}

int main() {
    /* buffer initialization. */
    ERROR_HANDLER(sbuffer_init(&buffer) == SBUFFER_FAILURE, "An error occurred during creation of the buffer");
    /* thread creation */
    pthread_mutex_init(&reader_signal_mutex, NULL);
    sem_init(&rw_mutex, 0, 1);
    sem_init(&buffer_count_mutex, 0, 0);
    pthread_t th[NUM_OF_THREADS];

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        if (i == 0) {
            pthread_create(th + i, NULL, (void*) writer_routine, NULL);
            continue;
        }
        pthread_create(th + i, NULL, (void*) reader_routine, NULL);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(th[i], NULL);
    }

    sbuffer_free(&buffer);
}


