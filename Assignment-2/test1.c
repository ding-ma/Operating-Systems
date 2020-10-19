#include "sut.h"
#include <stdio.h>
#include <stdlib.h>

void hello1() {
    int i;
    for (i = 0; i < 100; i++) {
        printf("Hello world!, this is SUT-One \n");
        sut_yield();
    }
}

void hello2() {
    int i;
    for (i = 0; i < 100; i++) {
        printf("Hello world!, this is SUT-Two \n");
        sut_yield();
    }
}

int main() {
    sut_init();
    sut_create(hello1);
    sut_create(hello2);
    sut_exit();
}
