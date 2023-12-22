#include <stdio.h>
#include <time.h>

int main () {
    time_t curtime;

    time(&curtime);

    printf("Current time = %s", ctime(&curtime));
sleep(3);
    time(&curtime);
    printf("Current time = %s", ctime(&curtime));
    printf("Current time = %s", ctime(&curtime));

    return(0);
}