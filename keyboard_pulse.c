#define ASCII_TABLE_REPR
#define ASCII_TABLE_DESC
#include "vshkh.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

/* This program is here for legacity */

static struct termios origin_termios[2];

void
set_raw_mode(int fd)
{
    struct termios raw_opts;
    tcgetattr(fd, origin_termios);
    raw_opts = *origin_termios;
    cfmakeraw(&raw_opts);
    raw_opts.c_oflag |= (OPOST | ONLCR); // Activa la conversión de '\n' en '\r\n'
    tcsetattr(fd, TCSANOW, &raw_opts);
}

void
exit_handler(int s)
{
    static FILE *f = NULL;
    if (f)
        fclose(f);
    tcsetattr(STDIN_FILENO, 0, &origin_termios[1]);
    exit(0);
}

int
main(int argc, char *argv[])
{
    char         c;
    ssize_t      n;
    static FILE *f;

    if (argc == 2)
        f = fopen(argv[1], "a");

    signal(SIGINT, exit_handler);
    signal(SIGTERM, exit_handler);
    signal(SIGKILL, exit_handler);

    set_raw_mode(STDIN_FILENO);

    puts("Send a SIGINT to quit");
    puts("Press any key");

    while (1)
    {
        switch (n = read(STDIN_FILENO, &c, 1))
        {
            case -1:
                puts("Error");
            case 0:
                usleep(100);
                break;
            default:
                printf("[%-3s] Hx: %#-3x (%s)\n", REPR[c], c, DESC[c]);
                if (f)
                    fprintf(f, "[%-3s] Hx: %#-3x (%s)\n", REPR[c], c, DESC[c]);
                break;
        }
    }
    return 0;
}
