#include "vshkh.h"
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

static int       ENABLED = 1;
static int       STARTED = 0;
static int       QUIT    = 0;
static pthread_t main_thread;

/* Sleep time if nothing read */
#define STIME 10000
/* Sleep time if disabled */
#define STDIS 10000
/* sleep time while waiting in kh_wait */
#define TWAIT 10000

static struct termios origin_termios;
static int            flags;

/* SUPR + [SHIFT] + key
 * Conclussion: First two numbers are totally random.
 * The last number is 10 if shift. I dont try to use other mods.
 * WTF is this, it has no order, why the A,B,C are at the end? wtff */
// clang-format off
static char supr_lookup_table[10][23][11] = {
[ 1][ 0][ 9]='d',
[ 1][ 1][ 9]='e',
[ 1][ 2][ 9]='f',
[ 1][ 3][ 9]='g',
[ 1][ 4][ 9]='h',
[ 1][ 5][ 9]='i',
[ 1][ 6][ 9]='j',
[ 1][ 7][ 9]='k',
[ 1][ 8][ 9]='l',
[ 1][ 9][ 9]='m',
[ 1][10][ 9]='n',
[ 1][11][ 9]='o',
[ 1][12][ 9]='p',
[ 1][13][ 9]='q',
[ 1][14][ 9]='r',
[ 1][15][ 9]='s',
[ 1][16][ 9]='t',
[ 1][17][ 9]='u',
[ 1][18][ 9]='v',
[ 1][19][ 9]='w',
[ 1][20][ 9]='x',
[ 1][21][ 9]='y',
[ 1][22][ 9]='z',
[ 1][ 0][10]='D',
[ 1][ 1][10]='E',
[ 1][ 2][10]='F',
[ 1][ 3][10]='G',
[ 1][ 4][10]='H',
[ 1][ 5][10]='I',
[ 1][ 6][10]='J',
[ 1][ 7][10]='K',
[ 1][ 8][10]='L',
[ 1][ 9][10]='M',
[ 1][10][10]='N',
[ 1][11][10]='O',
[ 1][12][10]='P',
[ 1][13][10]='Q',
[ 1][14][10]='R',
[ 1][15][10]='S',
[ 1][16][10]='T',
[ 1][17][10]='U',
[ 1][18][10]='V',
[ 1][19][10]='W',
[ 1][20][10]='X',
[ 1][21][10]='Y',
[ 1][22][10]='Z',
[ 3][ 2][ 9]=' ',
[ 3][ 9][ 9]='\'',
[ 4][ 4][ 9]=',',
[ 4][ 5][ 9]='-',
[ 4][ 6][ 9]='.',
[ 4][ 7][ 9]='/',
[ 4][ 9][ 9]='1',
[ 5][ 0][ 9]='2',
[ 5][ 1][ 9]='3',
[ 5][ 2][ 9]='4',
[ 5][ 3][ 9]='5',
[ 5][ 4][ 9]='6',
[ 5][ 5][ 9]='7',
[ 5][ 6][ 9]='8',
[ 5][ 7][ 9]='9',
[ 5][ 8][ 9]='0',
[ 5][ 9][ 9]=';',
[ 4][ 4][10]='<',
[ 4][ 5][10]='_',
[ 4][ 6][10]='>',
[ 4][ 7][10]='?',
[ 4][ 8][10]=')',
[ 4][ 9][10]='!',
[ 5][ 0][10]='@',
[ 5][ 1][10]='#',
[ 5][ 2][10]='$',
[ 5][ 3][10]='%',
[ 5][ 4][10]='^',
[ 5][ 5][10]='&',
[ 5][ 6][10]='*',
[ 5][ 7][10]='(',
[ 5][ 9][10]=':',
[ 6][ 1][ 9]='=',
[ 6][ 1][10]='+',
[ 9][ 1][ 9]='[',
[ 9][ 2][ 9]='\\',
[ 9][ 3][ 9]=']',
[ 9][ 6][ 9]='`',
[ 9][ 7][ 9]='a',
[ 9][ 8][ 9]='b',
[ 9][ 9][ 9]='c',
[ 9][ 1][10]='{',
[ 9][ 2][10]='|',
[ 9][ 3][10]='}',
[ 9][ 6][10]='~',
[ 9][ 7][10]='A',
[ 9][ 8][10]='B',
[ 9][ 9][10]='C',
};
// clang-format on

static void
__enable_raw_mode()
{
    struct termios raw_opts;
    tcgetattr(STDIN_FILENO, &origin_termios);
    raw_opts = origin_termios;
    cfmakeraw(&raw_opts);
    raw_opts.c_oflag |= (OPOST | ONLCR); // Activa la conversión de '\n' en '\r\n'

    // raw_opts.c_lflag &= ~ICANON;         // to set the timeout
    // raw_opts.c_cc[VTIME] = 1;            // timeout of 0,1s
    // raw_opts.c_cc[VMIN]  = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_opts);
    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

static void
__disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &origin_termios);
    fcntl(STDIN_FILENO, F_SETFL, flags);
    system("reset"); // force the terminal to reset
}

void
__kp_action(Keypress kp)
{
    Keybind  kb;
    BindFunc func;

    kb = kh_bind_new();
    kh_bind_append(&kb, kp);
    func = kh_bind_get(kb);
    if (func)
        func();
    else
        buffer_add(kp);
}


Keypress
__single_key_analize(char c)
{
    Keypress kp;

    switch (c)
    {
        case 0x0 ... 0x1F:
            kp.mods = CTRL_MOD | SHIFT_MOD;
            kp.c    = c + '@';
            break;

        case 'A' ... 'Z':
            kp.mods = SHIFT_MOD;
            kp.c    = c;
            break;

        default:
            kp.mods = NO_MOD;
            kp.c    = c;
            break;
    }
    return kp;
}


void
__analize(char c)
{
    /* De momento solo funciona para
     * keybinds de un solo caracter,
     * control y shift incluidos. */
    Keypress kp = __single_key_analize(c);

    __kp_action(kp);
}

void
__esc_special(char *buf)
{
    ssize_t  n;
    Keypress kp;

    switch (n = read(STDIN_FILENO, buf + 1, 2))
    {
        case 2:
            if (buf[1] != '[')
                goto __normal__; // just to avoid nesting

            switch (buf[2])
            {
                case 'A':
                    __kp_action((Keypress) {
                    .c    = ARROW_UP,
                    .mods = IS_ARROW,
                    });
                    return;

                case 'B':
                    __kp_action((Keypress) {
                    .c    = ARROW_DOWN,
                    .mods = IS_ARROW,
                    });
                    return;

                case 'C':
                    __kp_action((Keypress) {
                    .c    = ARROW_RIGHT,
                    .mods = IS_ARROW,
                    });
                    return;

                case 'D':
                    __kp_action((Keypress) {
                    .c    = ARROW_LEFT,
                    .mods = IS_ARROW,
                    });
                    return;
            }
        __normal__:
            __analize(buf[0]);
            __analize(buf[1]);
            __analize(buf[2]);

        case 1:
            // Alt mod
            /* Analize single key for allow alt+ctrl */
            kp = __single_key_analize(buf[1]);
            kp.mods |= ALT_MOD;
            __kp_action(kp);
            return;

        case 0:
        case -1: // eof -> no input
            __analize(buf[0]);

        default:
            break;
    }
}

static void *
__keyboard_handler(void *args)
{
    ssize_t n;
    char    buf[3];

    __enable_raw_mode();

    while (!QUIT)
    {
        if (ENABLED)
            switch (n = read(STDIN_FILENO, buf, 1))
            {
                case -1:
                case 0:
                    usleep(STIME);
                    break;

                default:
                    if (*buf == 0x1b) // esc
                        __esc_special(buf);

                    else
                        __analize(*buf);
            }
        else
            usleep(STDIS);
    }

    __disable_raw_mode();

    return NULL;
}

static void
die()
{
    QUIT = 1;
    __disable_raw_mode();
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
    die();
    pthread_join(main_thread, NULL);
}

/* Wait for keyboard input and return the first
 * keypress */
Keypress
kh_wait()
{
    Keypress kp;

    while (!kh_valid_kp(kp = kh_get()))
        usleep(TWAIT);
    return kp;
}

/* Ignore buffered keypresses and empty the
 * buffer. Removed keypressed cant be accesed
 * after calling this funcion. */
Keypress kh_flush();
