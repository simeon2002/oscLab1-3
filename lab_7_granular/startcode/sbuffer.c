/**
 * \author {AUTHOR}
 */

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include <semaphore.h>
#include <pthread.h>

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

sem_t rw_mutex;
sem_t buffer_count;
pthread_mutex_t reader_mutex;

int sbuffer_init(sbuffer_t **buffer) {
    sem_init(&buffer_count, 0, 0);
    sem_init(&rw_mutex, 0, 1);
    pthread_mutex_init(&reader_mutex, NULL);
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}


int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    // failure case
    if (buffer == NULL) return SBUFFER_FAILURE;
    sem_wait(&buffer_count); // waits to get something in the buffer.
    // no data case
    if (buffer->head == NULL) return SBUFFER_NO_DATA; // TODO QUESTION: there is not really a need for that one in my case?
    // data removed cases
    sem_wait(&rw_mutex); // start buffer lock
    *data = buffer->head->data;
    dummy = buffer->head;
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }
    sem_post(&rw_mutex); // end buffer lock
    free(dummy);
    return SBUFFER_SUCCESS;
}
int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    dummy = (sbuffer_node_t*) malloc(sizeof(sbuffer_node_t));
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        sem_wait(&rw_mutex); // lock can start here, because reader can't access buffer it iis empty anyways.
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
        sem_post(&rw_mutex);
    }
    sem_post(&buffer_count); // signals that there is something in buffer.
    return SBUFFER_SUCCESS;
}
