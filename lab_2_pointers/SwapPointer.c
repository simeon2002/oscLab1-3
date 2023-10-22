//
// Created by sims0702 on 10/22/23.
//

#include "SwapPointer.h"
void SwapPointer(int* p, int *q) {
    int *temp;
    temp = q;
    q = p;
    p = temp;
}