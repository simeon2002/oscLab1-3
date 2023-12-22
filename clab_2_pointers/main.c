#include <stdio.h>
#include <stdlib.h>
#include "SwapPointer.h"

void swapPointers(int **p, int **q) {
  //  int **temp = p;
    /* doesn't work because now we provide the address of the integer pointer p (in main) to temp
    the issue here is that in the instruction below, we change the address of the integer pointer p to the address of q
    whilst temp is also pointer towards the same address of the integer pointer p. This means that in the 2nd instr below
    it will also be assigned the same address as above it.
    */
    // conclusion: the content itself should be stored instead of storing the address to the content which will contain
    // a different value after the swapping.
    int *temp = *p;
    *p = *q;
    *q = temp;
}

void charSwap(char *ch1, char *ch2) {
    char *temp = ch1;
    *ch1 = *ch2;
    *ch2 = *temp; // note: this doesn't work because as temp stores the address of ch1, it will change when *ch1=*ch2 is exe!
    printf("testing");
}


int main() {
//    int *a = malloc(sizeof(int));
//    int *b = malloc(sizeof(int));
//    *a = 1;
//    *b = 2;
//    printf("a is %d and b is %d\n", *a, *b);
//    printf("the address of a is %u and the address of b is %u\n", a, b);

    int a = 1;
    int b = 2;
    int *p = &a;
    int *q = &b;
    printf("%u, %u\n", p, q);
    swapPointers(&p, &q);
    printf("%u, %u\n", p, q);

    char ch1 = 't';
    char ch2 = 'b';
    charSwap(&ch1, &ch2);
    printf("%c and %c", ch1, ch2);
}

