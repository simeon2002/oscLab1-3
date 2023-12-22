#include <stdio.h>

int main(void) {
//    int number = 1;
//    printf("%lu \n", sizeof(number));
    int myVar;
    size_t intSize = sizeof(myVar);
    printf("An integer is %lu bytes long.\n", intSize);
    printf("A char is %zu bytes long.\n", sizeof(char));
    printf("A double is %zu bytes long. \n", sizeof(double));
    printf("A float is %zu bytes long. \n", sizeof(long));
    printf("A VOID is %zu bytes long. \n", sizeof(short));
//    printf("A pointer is %zu bytes long.", sizeof(void*);
    return 0;
}