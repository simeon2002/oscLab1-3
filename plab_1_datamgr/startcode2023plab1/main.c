#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "lib/dplist.h"
#include "datamgr.h"
#include <time.h>

int main(){
    printf("Hello World\n");

    FILE * map = fopen("room_sensor.map", "r");
    ERROR_HANDLER(map == NULL, "File couuldn't open.");
    FILE * data = fopen("sensor_data", "rb");
    ERROR_HANDLER(data == NULL, "File couldn't open.");

    datamgr_parse_sensor_files(map, data);
    printf("The room id is %d\n", datamgr_get_room_id(142));
    printf("average temp: %.2f\n", datamgr_get_avg(15));
    time_t last_ts = datamgr_get_last_modified(15);
    printf("last timestamp: %s", ctime(&last_ts));
    printf("The total number of sensors: %d\n", datamgr_get_total_sensors());
    datamgr_free();

    fclose(map);
    fclose(data);
}