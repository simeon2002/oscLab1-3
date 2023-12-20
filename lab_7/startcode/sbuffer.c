/**
 * \author {AUTHOR}
 */


#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include <semaphore.h>
#include <pthread.h>

sem_t rw_mutex;
sem_t buffer_count;
#ifndef NUM_OF_THREADS
#define NUM_OF_THREADS 3
#endif
/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
};



int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    sem_init(&buffer_count, 0, 1);
    sem_init(&rw_mutex, 0, 1);
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL; // initializing fields
    (*buffer)->tail = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) { // if empty
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) { // freeing elements
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer); // freeing buffer pointer
    *buffer = NULL; // pointer to null
    return SBUFFER_SUCCESS;
}

// data param = sensor_data you want to remove
// buffer param = dplist.
int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;
    sem_wait(&buffer_count);
    sem_wait(&rw_mutex);
    *data = buffer->head->data; // retrieves sensor data from the buffer.
    dummy = buffer->head; // buffer_node to be removed.
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next; // use of FIFO
    }
    sem_post(&rw_mutex);
    free(dummy);
    printf("printing from buffer\n");
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;

    // failure cases
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;

    // success cases
    dummy->data = *data;
    dummy->next = NULL;
    sem_wait(&rw_mutex);
    if (buffer->tail == NULL) // buffer empty
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    sem_post(&buffer_count);
    sem_post(&rw_mutex);
    if (buffer->head == NULL) {
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
    }
    printf("writing to buffer\n");
    return SBUFFER_SUCCESS;
}