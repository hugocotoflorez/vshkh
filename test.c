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

void
ctrlalt()
{
    printf("ctrl+alt+a\n");
}

void
alt()
{
    printf("alt+a\n");
}

void
supr()
{
    printf("supr+a\n");
}

void
supr2()
{
    printf("supr+A\n");
}

void
supr3()
{
    printf("supr+ctrl+a\n");
}

void
supr4()
{
    printf("supr+alt+a\n");
}

void
supr_1()
{
    printf("supr+1\n");
}

int
main(void)
{
    Keypress kp;
    kh_start();

    kh_bind_create("^C", die);      // assign die to ctrl+c
    kh_bind_create("^L", clear);    // assign clear to ctrl+l
    kh_bind_create("^[", esc);      // assign esc to esc
    kh_bind_create("&a", alt);      // assign to alt-a
    kh_bind_create("^&A", ctrlalt); // assign to C-alt-a
    kh_bind_create("@a", supr);     // assign to supr-a
    kh_bind_create("@A", supr2);    // assign to supr-A
    kh_bind_create("^@A", supr3);   // assign to supr-ctrl-A
    kh_bind_create("@&a", supr4);   // assign to supr-alt-a
    kh_bind_create("@^1", supr_1);  // assign to supr-1
    /*
        kh_bind_create("0", hello);
        kh_bind_create("*", hello);
        kh_bind_create("h", die);
        kh_bind_create("h", hello); // change function binded to h
     */

    while (1)
    {
        kp = kh_wait();

        if (kh_has_supr(kp))
            printf("SUPR+");

        if (kh_has_ctrl(kp))
            printf("CTRL+");

        if (kh_has_alt(kp))
            printf("ALT+");

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
