#include "sut.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hello2();

void hello1() {
    int i;
    printf("Start\n");
    sut_yield();
    sut_open("0.0.0.0", 6969);
    char sbuf[128];
    char *str;
    for (i = 0; i < 100; i++) {
        sut_yield();
        sprintf(sbuf, "Hello world!, message from SUT-One i = %d\n", i);
        sut_write(sbuf, strlen(sbuf));
        printf("Hello world!, this is SUT-One: %d\n", i);
        str = sut_read();
        printf("%s", str);
    }
    sut_close();
    sut_create(hello2);
    sut_exit();
}

void hello2() {
    int i;
    for (i = 0; i < 100; i++) {
        printf("Hello world!, this is SUT-Two: %d\n", i);
        sut_yield();
        if (i == 50) {
            sut_exit();
            return;
        }
    }
}

int main() {
    sut_init();
    sut_create(hello2);
    sut_create(hello1);
    sut_shutdown();
}
