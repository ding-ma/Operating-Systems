#include "sut.h"
#include <stdio.h>
#include <string.h>

void hello1() {
    int i;
    char sbuf[128];
    sut_open("0.0.0.0", 6969);
    for (i = 0; i < 100; i++) {
        sprintf(sbuf, "Hello world!, message from SUT-One i = %d \n", i);
        sut_write(sbuf, strlen(sbuf));
//        sut_yield();
    }
    sut_close();
    sut_exit();
}

void hello2() {
    int i;
    for (i = 0; i < 100; i++) {
        printf("Hello world!, this is SUT-Two %d\n", i);
        sut_yield();
    }
    sut_exit();
}

int main() {
    sut_init();
    sut_create(hello1);
    // sut_create(hello2);
    sut_shutdown();
}
