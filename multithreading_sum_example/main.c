#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


int sum = 0;
pthread_mutex_t mutex;

void* test() {
    printf("Testing it out\n");
    int value = (rand() % 6) + 1;
    int* res = malloc(sizeof(int));
    *res = value;
    pthread_mutex_lock(&mutex);
    sum++;
//    printf("%d\n", sum);
    pthread_mutex_unlock(&mutex);
    return (void*)res;
}

int main() {
    int * res_main;
    pthread_t th[5];
    srand(time(NULL));

    for (int i = 0; i < 5; i++) {
        pthread_create(th + i, NULL, test, NULL);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(th[i], (void **) &res_main);
        printf("%d\n", *res_main);
    }
    free(res_main);
    pthread_mutex_destroy(&mutex);
    return 0;
}
