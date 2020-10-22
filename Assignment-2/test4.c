#include "sut.h"
#include <stdio.h>
#include <string.h>

void hello1() {
    int i;
    char sbuf[128];
    for (i = 0; i < 100; i++) {
        sprintf(sbuf, "Hello world!, message from SUT-One i = %d \n", i);
        printf("Hello world!, this is SUT-One %d\n", i);
        sut_write(sbuf, strlen(sbuf)); //put in io Q. gets executed. IO puts it back to task ready queue
        sut_yield(); //then yield runs again.
    }
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
    sut_create(hello2);
    sut_shutdown();
}
