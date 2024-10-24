#define ASCII_TABLE_REPR
#include "vshkh.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void
die()
{
    kill(getpid(), SIGTERM);
}

void
hello()
{
    printf("Hello\n");
}

int
main(void)
{
    Keypress kp;
    kh_start();

    Keybind kb = kh_bind_parse("^C");
    kh_bind_set_func(&kb, NULL);
    kh_bind_add(kb);

    Keybind kb2 = kh_bind_parse("^C");
    kh_bind_set_func(&kb2, die);
    kh_bind_add(kb2);

    Keybind kb3 = kh_bind_parse("C");
    kh_bind_set_func(&kb3, hello);
    kh_bind_add(kb3);

    assert(kb_is_equal(kb, kb2));
    assert(!kb_is_equal(kb, kb3));


    while (1)
    {
        kp = kh_get();
        if (kh_valid_kp(kp))
        {
            printf("[%-3s]", REPR[(int) kp.c]);
            if (kh_has_ctrl(kp))
                printf(" + CTRL");
            if (kh_has_shift(kp))
                printf(" + SHIFT");
            putchar('\n');
        }
        else
            usleep(100);
    }
    return 0;
}
