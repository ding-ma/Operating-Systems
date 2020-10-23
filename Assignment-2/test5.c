#include "sut.h"
#include <stdio.h>
#include <string.h>

void hello1() {
    int i;
    char *str;
    sut_open(HOST, PORT);
    for (i = 0; i < 10; i++) {
        str = sut_read(); //should be blocking
        if (strlen(str) != 0) {
            printf("I am SUT-One, message from server: %s\n", str);
        
        } else {
            printf("ERROR!, empty message received \n");
        }
        sut_yield();
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
    sut_create(hello2);
    sut_shutdown();
}
