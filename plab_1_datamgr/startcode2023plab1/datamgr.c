// question: if a lib like stdlib is included in f.e. the header file of datamgr.c or in another header file
            // do I still need to include it here?
// question 2: why can't I read the sensor_element_t struct in one memory block completely like with read from unitsd.h? (line 68 approx.)
// question 4: for the total amount of sensors ... does this mean the sensors encountered in the binary fine, also the non-existent once
            // or just the ones that are existent.
// question 5: why is does temp too hot not come in a red color on screen as an error?
// quesetiotn 6: is it necessary to use ERROR_HANDLER for the reads() or won't that be a problem?
// todo: adding loggers.


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

#define RUNNING_AVG_LENGTH 5
dplist_t *sensor_list;

/* Definition of element_t */
typedef struct {
    int room_id;
    int sensor_id;
    sensor_value_t data_running_avg[RUN_AVG_LENGTH];
    sensor_value_t sum;
    sensor_value_t running_avg;
    int temp_count;
    sensor_ts_t last_timestamp;

} sensor_element_t;

// dplist functions
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



// datamgr functions

/* parsing function */
void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {
    /* sensor list creation*/
    sensor_list = dpl_create(element_copy, element_free, element_compare);

    /* parsing of room to sensor mappings */
    int counter = 0;
    while (1){
        sensor_element_t *temp_node = (sensor_element_t*)malloc(sizeof(sensor_element_t));
        ERROR_HANDLER(temp_node == NULL, EXIT_OUT_OF_MEMORY, "Memory allocation failed");


        int items_read = fscanf(fp_sensor_map, "%d %d\n", &(temp_node->room_id), &(temp_node->sensor_id));
        if (items_read == 2) { // reading sensor id and room id
            dpl_insert_at_index(sensor_list, temp_node, counter++, true); // copy necessary as I use same node.
            free(temp_node);
        } else if (items_read == EOF){ // end of file
            free(temp_node);
            break;
        } else { // error occurred
            free(temp_node);
            ERROR_HANDLER(1, EXIT_FILE_ERROR, "Error occurred during reading from file");
        };
    }
    ERROR_HANDLER(dpl_get_element_at_index(sensor_list, 0) == NULL, 1, "The sensor list is empty, "
                                                                    "please add sensor_room mappings before continuing.");

    /* initializing data_avg_temp with NAN values */
    sensor_element_t *sensor_node;
    for (int i = 0; i < dpl_size(sensor_list); ++i) {
        sensor_node = (sensor_element_t*)dpl_get_element_at_index(sensor_list, i); // no copy returned
        for (int j = 0; j < RUN_AVG_LENGTH; j++) {
            sensor_node->data_running_avg[j] = NAN;
            sensor_node->temp_count = 0;
            sensor_node->sum = 0.0;
            sensor_node->running_avg = 0.0;
        }
    }

    /* parsing binary file */
    parse_temp_reading_and_ts(sensor_list, fp_sensor_data);

    /* extracting the sensor data from binary file.*/
//    sensor_id_t id;
//    sensor_value_t value;
//    sensor_ts_t ts;
//    counter = 0;
//    while(1) {
//        size_t bytes_read = fread(&id, sizeof(id), 1, fp_sensor_data);
//        if (bytes_read == 0) break;
//        fread(&value, sizeof(value), 1, fp_sensor_data);
//        fread(&ts, sizeof(ts), 1, fp_sensor_data);
//        counter++;
//    }
//   printf("%d\n", counter);
//    struct stat sb;
//    fstat(fileno(fp_sensor_data), &sb);
////    printf("%d", sizeof );
//    printf("test %lu\n", sb.st_size / 18);
    for (int i = 0; i < dpl_size(sensor_list); i++) {
        sensor_element_t *sensor_node = dpl_get_element_at_index(sensor_list, i);
        printf("sensorid: %d, %f, %s", sensor_node->sensor_id, sensor_node->running_avg, ctime(&(sensor_node->last_timestamp)));
    }
}

/* parsing last 5 temperature readings of each and last timestamp of each*/
void parse_temp_reading_and_ts(dplist_t *sensor_list, FILE *fp_data) {
    int num_of_sensors = dpl_size(sensor_list);
    ERROR_HANDLER(num_of_sensors == -1, 1, "No sensor are present at this point of time.");

    do {
        int result;

        sensor_id_t sensor_id;
        sensor_value_t sensor_value;
        sensor_ts_t sensor_ts;
        result = fread(&sensor_id, sizeof(sensor_id_t), 1, fp_data);
        if (result != 1) {
            if (feof(fp_data)) {
                // done with reading
                break;
            } else if (ferror(fp_data)) {
                ERROR_HANDLER(1, EXIT_FILE_ERROR, "Error reading file");
            }
        }

        result = fread(&sensor_value, sizeof(sensor_value_t), 1, fp_data);
        if (result != 1) {
            ERROR_HANDLER(1, EXIT_FILE_ERROR, "Error reading file");
        }

        result = fread(&sensor_ts, sizeof(sensor_ts_t), 1, fp_data);
        if (result != 1) {
            ERROR_HANDLER(1, EXIT_FILE_ERROR, "Error reading file");
        }

        for (int i = 0; i < num_of_sensors; i++) {
            sensor_element_t* sensor_node = (sensor_element_t*) dpl_get_element_at_index(sensor_list, i);
            if (sensor_node->sensor_id == sensor_id) {
                sensor_node->last_timestamp = sensor_ts;
                if (sensor_node->temp_count == RUNNING_AVG_LENGTH) {
                    sensor_value_t old_running_avg = sensor_node->running_avg;
                    sensor_node->running_avg = old_running_avg + (sensor_value - old_running_avg) / RUNNING_AVG_LENGTH;
                } else { // if count not yet reached.
                    sensor_node->temp_count++;
                    sensor_node->sum += sensor_value;
                    sensor_node->running_avg = 1.0* sensor_node->sum / sensor_node->temp_count;
                }
//                printf("current temperature for sensor %d; %f\n", sensor_node->sensor_id, sensor_node->running_avg);
//                printf("last ts: %s\n", ctime(&(sensor_node->last_timestamp)));
            }
        }
    } while (1);
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
    ERROR_HANDLER(1, 1, "Sensor id doesn't exist.");
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id) {
    sensor_value_t avg_temp;
    sensor_element_t *sensor;

    for (int i = 0; i < dpl_size(sensor_list); i++) {
        sensor_element_t *sensor_node = dpl_get_element_at_index(sensor_list, i);
        if (sensor_id == sensor_node->sensor_id) {
            sensor = dpl_get_element_at_index(sensor_list, i);
            break;
        }
        ERROR_HANDLER(i == dpl_size(sensor_list) - 1, 1, "Sensor id doesn't exist.");
        }

    if (sensor->temp_count != RUNNING_AVG_LENGTH) { // checking only the last value is enough!
        return 0;
    } else {
        return sensor->running_avg;
    }


    // checking if temp dev is bigger or smaller than temp dev for correction.
    // TODO: add to logging processing by writing write_to_log_process() here.
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
    ERROR_HANDLER(1, 1, "Sensor id doesn't exist.");
}

int datamgr_get_total_sensors() {
    return dpl_size(sensor_list);
}

// question 3: whenever there is an error during reading, do I need to exit the process or? (line 68 approx.)
