// question: if a lib like stdlib is included in f.e. the header file of datamgr.c or in another header file
            // do I still need to include it here?
// question 2: why can't I read the sensor_element_t struct in one memory block completely like with read from unitsd.h? (line 68 approx.)
// question 4: for the total amount of sensors ... does this mean the sensors encountered in the binary fine, also the non-existent once
            // or just the ones that are existent.
// question 5: why is does temp too hot not come in a red color on screen as an error?
// quesetiotn 6: is it necessary to use ERROR_HANDLER for the reads() or won't that be a problem?
// todo: catch the erorr for fprintf()
// todo: change approach of determining avg_temp!

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "datamgr.h"
#include "lib/dplist.h"
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h> // for NaN

dplist_t *sensor_list;

/* Definition of element_t */
typedef struct {
    int room_id;
    int sensor_id;
    sensor_value_t data_running_avg[RUN_AVG_LENGTH];
    sensor_ts_t last_timestamp;
} sensor_element_t;

/* defintion of the 3 user-defined functions. */


void *element_copy(void *element)
{
    sensor_element_t *copy = malloc(sizeof(sensor_element_t) * RUN_AVG_LENGTH);
    assert(copy != NULL);
    copy->room_id = ((sensor_element_t *)element)->room_id;
    copy->sensor_id = ((sensor_element_t *)element)->sensor_id;
    memcpy(copy->data_running_avg, ((sensor_element_t *)element)->data_running_avg, sizeof(copy->data_running_avg));
    copy->last_timestamp = ((sensor_element_t*)element)->last_timestamp;
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




/* parsing function */
void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {
    /* sensor list creation*/
    sensor_list = dpl_create(element_copy, element_free, element_compare);

    /* parsing of room to sensor mappings */
    int counter = 0;
    while (1){
        //note: true for insert_copy needs to be used as you are using a temp_node pointer which will always point to the same value!!!
        sensor_element_t *temp_node = (sensor_element_t*)malloc(sizeof(sensor_element_t));
        assert(temp_node!=NULL);
        if ((fscanf(fp_sensor_map, "%d %d\n", &(temp_node->room_id), &(temp_node->sensor_id))) == 2) {
            dpl_insert_at_index(sensor_list, temp_node, counter++, true); // copy necessary as I use same node.
            free(temp_node);
        } else {
            free(temp_node);
            break;
        };
    }
    ERROR_HANDLER(dpl_get_element_at_index(sensor_list, 0) == NULL, "The sensor list is empty at this point of time");

    /* initializing data_avg_temp with NAN values */
    for (int i = 0; i < dpl_size(sensor_list); ++i) {
        sensor_element_t *sensor_node = (sensor_element_t*)dpl_get_element_at_index(sensor_list, i);
        for (int j = 0; j < RUN_AVG_LENGTH; j++) {
            sensor_node->data_running_avg[j] = NAN;
        }
    }

    /* parsing binary file */
    parse_temp_reading_and_ts(sensor_list, fp_sensor_data);

    /* extracting the sensor data from binary file.*/
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
    int counter = 0;
    while(1) {
        size_t bytes_read = fread(&id, sizeof(id), 1, fp_sensor_data);
        if (bytes_read == 0) break;
        fread(&value, sizeof(value), 1, fp_sensor_data);
        fread(&ts, sizeof(ts), 1, fp_sensor_data);
        counter++;
    }
   printf("%d", counter);
    struct stat sb;
    fstat(fileno(fp_sensor_data), &sb);
    printf("%d", sizeof )
    printf("%lu", sb.st_size / 18);

}

/* parsing last 5 temperature readings of each and last timestamp of each*/
void parse_temp_reading_and_ts(dplist_t *list, FILE *fp_data) {
    int num_of_sensors = dpl_size(list);
    ERROR_HANDLER(num_of_sensors == -1, "No sensor are present at this point of time.");
    struct stat sb; // == properties of the file.
    if (fstat(fileno(fp_data), &sb) == -1) {
        perror("fstat");
        exit(2);
    }

    /* get num of sensor reading*/
    long long num_of_sensor_readings = (long long) sb.st_size /
                                       (long long) (sizeof(sensor_id_t) + sizeof(sensor_value_t)+sizeof(sensor_ts_t)); // size of file / size of one sensor struct

    /* reading data from file */
    sensor_data_t *data_arrays = malloc(num_of_sensor_readings * sizeof(sensor_element_t));
    assert(data_arrays != NULL);
    for (int i = 0; i < num_of_sensor_readings; i++) {
        fread(&data_arrays[i].id, sizeof(sensor_id_t), 1, fp_data);
        if (ferror(fp_data)) {
            ERROR_HANDLER(1, "An error occurred during reading");
        }
        fread(&data_arrays[i].value, sizeof(sensor_value_t), 1, fp_data);
        if (ferror(fp_data)) {
            ERROR_HANDLER(1, "An error occurred during reading");
        }
        fread(&data_arrays[i].ts, sizeof(sensor_ts_t), 1, fp_data);
        if (ferror(fp_data)) {
            ERROR_HANDLER(1, "An error occurred during reading");
        }
    }

    /* getting last \RUN_AVG_LENGTH size time values for every sensor + last timestamp value */
    int size = 0;
    for (int j = num_of_sensors - 1; j >= 0; j--) {
        sensor_element_t  *sensor_node = (sensor_element_t*)dpl_get_element_at_index(list, j);
        for (int i = (int)num_of_sensor_readings - 1; i >= 0; i--) { // looping through temp_values and senosr values
            if (size == RUN_AVG_LENGTH) break;
            if (sensor_node->sensor_id != data_arrays[i].id) {
                continue;
            }
            // adding temp value if sensor_id are equivalent
            sensor_node->data_running_avg[size] = data_arrays[i].value;
            // adding ts of last temp value
            if (size == 0) { // if it still the first matching temp value encounter from the end of the file
                sensor_node->last_timestamp = data_arrays[i].ts;
            }
            size++;
        }
        size = 0;
    }

//    /* DEBUGGING: printing sensor list */
//    for (int i = 0; i < num_of_sensors; ++i) {
//        sensor_element_t *sensor_node = (sensor_element_t*)dpl_get_element_at_index(list, i);
//        printf("sensor values for sensor %d\n", sensor_node->sensor_id);
//        for (int j = 0; j < RUN_AVG_LENGTH; j++) {
//            printf("The temp value is: %f\n", sensor_node->data_running_avg[j]);
//        }
//        printf("The last timestamp is: %s\n\n", ctime(&sensor_node->last_timestamp));
//    }
    free(data_arrays);
}

void datamgr_free() {
    dpl_free(&sensor_list, true);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id) {
    for (int i = 0; i < dpl_size(sensor_list); i++) { // data will already have been parsed, so no need to check dpl_size for -1 i.e. being empty
        sensor_element_t *sensor_node = dpl_get_element_at_index(sensor_list, i);
        if (sensor_id == sensor_node->sensor_id) {
            return sensor_node->room_id;
        }
    }
    ERROR_HANDLER(1, "Sensor id doesn't exist.");
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id) {
    sensor_value_t avg_temp;
    sensor_value_t temp_data[RUN_AVG_LENGTH];

    for (int i = 0; i < dpl_size(sensor_list); i++) {
        sensor_element_t *sensor_node = dpl_get_element_at_index(sensor_list, i);
        if (sensor_id == sensor_node->sensor_id) {
            memcpy(temp_data, sensor_node->data_running_avg, sizeof(sensor_value_t) * RUN_AVG_LENGTH);
            break;
        }
        ERROR_HANDLER(i == RUN_AVG_LENGTH - 1, "Sensor id doesn't exist.");
        }

    // checking temp_data size.
    if (isnan(temp_data[RUN_AVG_LENGTH - 1])) { // checking only the last value is enough!
        return 0;
    } else { // calc average
        sensor_value_t sum = 0;
        for (int i = 0; i < RUN_AVG_LENGTH; i++) {
            sum += temp_data[i];
        }
        avg_temp = sum / RUN_AVG_LENGTH;
    }
    // checking if temp dev is bigger or smaller than temp dev for correction.
    if (avg_temp > SET_MAX_TEMP) {
        fprintf(stderr, "The room is too hot.\n");
    }
    else if (avg_temp < SET_MIN_TEMP) fprintf(stderr, "The room is too cold.\n");
    return avg_temp;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id) {
    sensor_element_t *sensor_node;
    for (int i = 0; i < dpl_size(sensor_list); ++i) {
        sensor_node = dpl_get_element_at_index(sensor_list, i);
        if (sensor_id == sensor_node->sensor_id) {
            return sensor_node->last_timestamp;
        }
    }
    ERROR_HANDLER(1, "Sensor id doesn't exist.");
}

int datamgr_get_total_sensors() {
    return dpl_size(sensor_list);
}

// question 3: whenever there is an error during reading, do I need to exit the process or? (line 68 approx.)
