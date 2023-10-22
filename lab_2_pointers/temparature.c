//
// Created by sims0702 on 10/22/23.
//
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include<unistd.h>

int main() {
    int i = 0;
    while (1) {
        // generating random number
        srand(time(0));
        int range = 45;
        float temp = (float) rand() / (float) (RAND_MAX) * range - 10;
        // generating date
        time_t result = time(NULL);
        struct tm *dateTime = localtime(&result);

        // display to screen.
        printf("Temperature: %.2f degrees celcius @%s", temp, asctime(dateTime));
        fflush(stdout); // flushes the output buffer as to display it to screen.
        sleep(1); // flush necessary because otherwise everything will only be printed at the end basically.
    }
}

//
//// using a time_t type to print out the time.
//time_t result = time(NULL);
//char *test = ctime(&result);
//char testtt[] = "testing";
//printf("%s", ctime(&result));
//printf("%s", test);
//printf("test is here: %s\n", &testtt); // note: so when printing a string, you need to give a pointer as
//// it is an array and it will point towards the first element of the array(/ the first char).
//
//
//// using a tm structure.
//time_t result2 = time(NULL);
//struct tm *tmStructure = localtime(&result2);
//printf("%s", asctime(tmStructure));
//}