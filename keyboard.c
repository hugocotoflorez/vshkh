#include "vshkh.h"
#include <pthread.h>
#include <stdio.h> // debug
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

static int       ENABLED = 1;
static int       STARTED = 0;
static int       QUIT    = 0;
static pthread_t main_thread;

/* Sleep time if nothing read */
#define STIME 100
/* Sleep time if disabled */
#define STDIS 100

static struct termios origin_termios[2];

static void
__enable_raw_mode()
{
    struct termios raw_opts;
    tcgetattr(STDIN_FILENO, origin_termios);
    raw_opts = *origin_termios;
    cfmakeraw(&raw_opts);
    raw_opts.c_oflag |= (OPOST | ONLCR); // Activa la conversión de '\n' en '\r\n'
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_opts);
}

static void
__disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, 0, &origin_termios[1]);
}

static void *
__keyboard_handler(void *args)
{
    ssize_t  n;
    char     buf[1];
    Keypress kp;
    BindFunc func;

    __enable_raw_mode();

    while (!QUIT)
    {
        if (ENABLED)
            switch (n = read(STDIN_FILENO, buf, sizeof(buf)))
            {
                case -1:
                case 0:
                    usleep(STIME);
                    break;

                default:
                    switch (*buf)
                    {
                        case 0x0 ... 0x1F:
                            kp.mods = CTRL_MOD | SHIFT_MOD;
                            kp.c    = *buf + '@';
                            break;

                        case 'A' ... 'Z':
                            kp.mods = SHIFT_MOD;
                            kp.c    = *buf;
                            break;

                        default:
                            kp.mods = NO_MOD;
                            kp.c    = *buf;
                            break;
                    }

                    /* De momento solo funciona para
                     * keybinds de un solo caracter,
                     * control y shift incluidos. */
                    Keybind kb;
                    kb = kh_bind_new();
                    kh_bind_append(&kb, kp);
                    func = kh_bind_get(kb);
                    if (func)
                        func();
                    else
                        buffer_add(kp);
            }
        else
            usleep(STDIS);
    }

    __disable_raw_mode();

    return NULL;
}

static __attribute__((constructor)) void
__init__()
{
    buffer_new(10);
    array_new();
}

/* Get a keypress that is waiting in buffer and
 * remove it from buffer. If no keypress is in
 * buffer it return a keypress that return 0
 * if passed to kh_valid_kp. */
Keypress
kh_get()
{
    return buffer_pop();
}

/* The keyboard handler would be initialized in
 * a different thread, so this funcion returns
 * once it is created. This function also wake up
 * the handler if paused. */
void
kh_start()
{
    switch (STARTED)
    {
        case 0:
            pthread_create(&main_thread, NULL, __keyboard_handler, NULL);

        case 1:
            ENABLED = 1;
            break;
    }
}

/* Pause the handler, the input would be read
 * once it starts again */
void
kh_pause()
{
    ENABLED = 0;
}

/* Close the handler and restore all values
 * to default */
void
kh_end()
{
    QUIT = 1;
    pthread_join(main_thread, NULL);
}

/* Wait for keyboard input and return the first
 * keypress */
Keypress kh_wait();

/* Ignore buffered keypresses and empty the
 * buffer. Removed keypressed cant be accesed
 * after calling this funcion. */
Keypress kh_flush();
