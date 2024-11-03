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
clear()
{
    printf("\033[H\033[2J");
    fflush(stdout);
}

void
func()
{
    puts("C-Alt-Shift-A");
}

int
main(void)
{
    Keypress kp;
    kh_start();

    kh_bind_create("^L", clear); // assign clear to ctrl+l
    kh_bind_create("^C", die);   // assign __die to ctrl+c
    kh_bind_create("^&A", func); // assign __die to ctrl+c
    /*
    kh_bind_create("0", hello);     // assign hello to 0
    kh_bind_create("*", hello);     // assign hellp to *
    kh_bind_create("h", __die);       // assign __die to h
    kh_bind_create("h", hello);     // change function binded to h
    kh_bind_create("^[", esc);      // assign esc to esc
    kh_bind_create("&a", alt);      // assign to alt-a
    kh_bind_create("^&A", ctrlalt); // assign to C-alt-a
    kh_bind_create("@a", supr);     // assign to supr-a
    kh_bind_create("@A", supr2);    // assign to supr-A
    kh_bind_create("^@A", supr3);   // assign to supr-ctrl-A
    kh_bind_create("@&a", supr4);   // assign to supr-alt-a
    kh_bind_create("@^1", supr_1);  // assign to supr-1
    kh_bind_create(get_arrow_str(ARROW_LEFT, SUPR_MOD | CTRL_MOD, str), arrow_mod); */

    while (1)
    {
        kp = kh_wait();
        kh_repr_kp(kp);
    }
    return 0;
}
