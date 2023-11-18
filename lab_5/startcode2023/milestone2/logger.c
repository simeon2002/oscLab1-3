#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int i = 0; // global variable for sequence number in log message.
FILE *fp; // declaration of the file pointer as global var for in-scope reachability in other functions

//DONE
int write_to_log_process(char *msg) { // msg = event-info message
    // creating current timestamp
    // Get the current time
    time_t timer;
    time(&timer);
    struct tm *timestamp_info;
    timestamp_info = localtime(&timer);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%x %X", timestamp_info);

    // writing the message
    fprintf(fp, "%d - %s - %s", i, timestamp, msg);
    i++;
    return 0;
}

int create_log_process() {
    // log file opening
    fp = fopen("gateway.log", "a");

    return 0;
}

int end_log_process() {
    fclose(fp);
    return 0;
}


// questions:
// 1. do I need to declare the sequence number on the top of the file?
// 2. to be able to access the FP in write_to_log_process, the only way to do this would be to
//    initialize the file pointer as a global variable, correct? follow up: is this a better
//    practice than just providing it as an argument to the function?? Thanks in advance.