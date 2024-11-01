#include <pthread.h>
#define ASCII_TABLE_REPR
#include "vshkh.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
die()
{
    kh_end();
    exit(0);
}

void
hello()
{
    printf("Hello\n");
}

void
pause_wrap()
{
    kh_pause();
    sleep(2);
    kh_start();
}

void
esc()
{
    printf("ESC\n");
}

void
clear()
{
    printf("\033[H\033[2J");
    fflush(stdout);
}

int
main(void)
{
    Keypress kp;
    kh_start();

    kh_bind_create("^C", die);   // assign die to ctrl+c
    kh_bind_create("^L", clear); // assign clear to ctrl+l
    kh_bind_create("^[", esc);   // assign esc to esc
    /*
        kh_bind_create("0", hello);
        kh_bind_create("*", hello);
        kh_bind_create("h", die);
        kh_bind_create("h", hello); // change function binded to h
     */

    while (1)
    {
        kp = kh_wait();

        if (kh_has_ctrl(kp))
            printf("^");

        if (kh_has_alt(kp))
            printf("&");

        if (kh_is_arrow(kp))
            switch (kp.c)
            {
                case ARROW_UP:
                    printf("arrow up");
                    break;
                case ARROW_DOWN:
                    printf("arrow down");
                    break;
                case ARROW_LEFT:
                    printf("arrow left");
                    break;
                case ARROW_RIGHT:
                    printf("arrow right");
                    break;
            }

        else
            printf("%s", REPR[(int) kp.c]);

        putchar('\n');
    }
    return 0;
}
