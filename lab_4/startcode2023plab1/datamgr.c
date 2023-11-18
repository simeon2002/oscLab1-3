
//#include <stdlib.h>
//#include <stdio.h>
#include <assert.h>
#include "datamgr.h"
#include "lib/dplist.h"
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

typedef struct {
    int room_id;
    int sensor_id;
    sensor_value_t data_running_avg[5];
    sensor_ts_t tmsp_last_value;
} sensor_element_t;

void *element_copy(void *element);
void element_free(void **element);
int element_compare(void *x, void *y);

void *element_copy(void *element)
{
    sensor_element_t *copy = malloc(sizeof(sensor_element_t));
    assert(copy != NULL);
    copy->room_id = ((sensor_element_t *)element)->room_id;
    copy->sensor_id = ((sensor_element_t *)element)->sensor_id;
    memcpy(copy->data_running_avg, ((sensor_element_t *)element)->data_running_avg, sizeof(copy->data_running_avg));
    copy->tmsp_last_value = ((sensor_element_t*)element)->tmsp_last_value;
    return (void *)copy;
}

void element_free(void **element)
{
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((sensor_element_t *)x)->room_id < ((sensor_element_t *)y)->room_id) ? -1 : (((sensor_element_t *)x)->room_id == ((sensor_element_t *)y)->room_id) ? 0
                                                                                                                            : 1);
}


void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {
    dplist_t * sensor_list = dpl_create(element_copy, element_free, element_compare);
    // parsing the sensor mappings (room id and sensor id values)
    sensor_element_t *temp_node = (sensor_element_t*)malloc(sizeof(sensor_element_t));
    assert(temp_node!=NULL);
    while ((fscanf(fp_sensor_map, "%d %d\n", &(temp_node->room_id), &(temp_node->sensor_id))) == 2){
        /*note: true for insert_copy needs to be used as you are using a temp_node pointer which will always point to the same value!!!*/
        dpl_insert_at_index(sensor_list, temp_node, dpl_size(sensor_list), true);
    }


//    // printing all elements:
//    for (int i = 0; i < dpl_size(sensor_list); ++i) {
//        sensor_element_t *node = dpl_get_element_at_index(sensor_list, i);
//        printf("room id: %d and sensor id: %d\n", node->room_id, node->sensor_id);
//    }
    free(temp_node);

    /* extracting the sensor data from binary file.*/
    sensor_data_t sensor_reading;
    int counter = 0;
    while(1) {
        if (fread(&sensor_reading, sizeof(sensor_data_t), 1, fp_sensor_data) != 1) break;
        printf("sensor id: %" PRIu16", temperature value: %f C, timestamp: %ld\n",
               sensor_reading.id, sensor_reading.value, sensor_reading.ts);
        counter++;
    }
    printf("%d", counter);




    dpl_free(&sensor_list, true); // note: the memcopied array needs to be freed as well.
}