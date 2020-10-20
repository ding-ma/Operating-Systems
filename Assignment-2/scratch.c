#include <stdio.h>
#include <ucontext.h>


static ucontext_t ctx[3];


static void
f1(void) {
    while (1) {
        puts("start f1");
        swapcontext(&ctx[1], &ctx[0]);
    }
    
}


static void
f2(void) {
    while (1) {
        puts("start f2");
        swapcontext(&ctx[2], &ctx[0]);
    }
}


int
main(void) {
    char st1[8192];
    char st2[8192];
    
    
    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = st1;
    ctx[1].uc_stack.ss_size = sizeof st1;
    ctx[1].uc_link = &ctx[0];
    makecontext(&ctx[1], f1, 0);
    
    
    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp = st2;
    ctx[2].uc_stack.ss_size = sizeof st2;
    ctx[2].uc_link = &ctx[1];
    makecontext(&ctx[2], f2, 0);
    
    while (1) {
        swapcontext(&ctx[0], &ctx[1]);
        swapcontext(&ctx[0], &ctx[2]);
    }
    
    return 0;
}