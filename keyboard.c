#include "vshkh.h"
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdio.h>
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

#define BUFSIZE 10

static struct termios origin_termios;
static int            flags;

/* SUPR + SHIFT + key
 * Uper case characters are placed over
 * lower case ones that share key place. */
static char supr_shift_lookup[] = {
    [0] = '?',
    [1] = '?',
    [2] = '?',
    [3] = '?',
    [4] = '?',
    [5] = '?',
    [6] = '?',
    [7] = '?',
    [8] = '?',
    [9] = '?',
    [10] = '?',
    [11] = '?',
    [12] = '?',
    [13] = '?',
    [14] = '?',
    [15] = '?',
    [16] = '?',
    [17] = '?',
    [18] = '?',
    [19] = '?',
    [20] = '?',
    [21] = '?',
    [22] = '?',
    [23] = '?',
    [24] = '?',
    [25] = '?',
    [26] = '?',
    [27] = '?',
    [28] = '?',
    [29] = '?',
    [30] = '?',
    [31] = '?',
    [32] = '?',
    [33] = '?',
    [34] = '?',
    [35] = '?',
    [36] = '?',
    [37] = '?',
    [38] = '?',
    [39] = '?',
    [40] = '?',
    [41] = '?',
    [42] = '?',
    [43] = '?',
    [44] = '<',
    [45] = '_',
    [46] = '>',
    [47] = '?',
    [48] = ')',
    [49] = '!',
    [50] = '@',
    [51] = '#',
    [52] = '$',
    [53] = '%',
    [54] = '^',
    [55] = '&',
    [56] = '*',
    [57] = '(',
    [58] = '?',
    [59] = ':',
    [60] = '?',
    [61] = '?',
    [62] = '?',
    [63] = '?',
    [64] = '?',
    [65] = '?',
    [66] = '?',
    [67] = '?',
    [68] = '?',
    [69] = '?',
    [70] = '?',
    [71] = '?',
    [72] = '?',
    [73] = '?',
    [74] = '?',
    [75] = '?',
    [76] = '?',
    [77] = '?',
    [78] = '?',
    [79] = '?',
    [80] = '?',
    [81] = '?',
    [82] = '?',
    [83] = '?',
    [84] = '?',
    [85] = '?',
    [86] = '?',
    [87] = '?',
    [88] = '?',
    [89] = '?',
    [90] = '?',
    [91] = '{',
    [92] = '|',
    [93] = '}',
    [94] = '?',
    [95] = '?',
    [96] = '~',
    [97] = 'A',
    [98] = 'B',
    [99] = 'C',
    [100] = 'D',
    [101] = 'E',
    [102] = 'F',
    [103] = 'G',
    [104] = 'H',
    [105] = 'I',
    [106] = 'J',
    [107] = 'K',
    [108] = 'L',
    [109] = 'M',
    [110] = 'N',
    [111] = 'O',
    [112] = 'P',
    [113] = 'Q',
    [114] = 'R',
    [115] = 'S',
    [116] = 'T',
    [117] = 'U',
    [118] = 'V',
    [119] = 'W',
    [120] = 'X',
    [121] = 'Y',
    [122] = 'Z',
    [123] = '?',
    [124] = '?',
    [125] = '?',
    [126] = '?',
    [127] = '?',
};

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

Keypress
__get_arrow_kp(char c)
{
    switch (c)
    {
        case 'A':
            return ((Keypress) {
            .c    = ARROW_UP,
            .mods = IS_ARROW,
            });

        case 'B':
            return ((Keypress) {
            .c    = ARROW_DOWN,
            .mods = IS_ARROW,
            });

        case 'C':
            return ((Keypress) {
            .c    = ARROW_RIGHT,
            .mods = IS_ARROW,
            });

        case 'D':
            return ((Keypress) {
            .c    = ARROW_LEFT,
            .mods = IS_ARROW,
            });
    }
    return INVALID_KP;
}


void
__supr_get_mods(Keypress *kp, int supr_mod, int supr_key)
{
    switch (supr_mod) /* SUPR MODS */
    {
        case 0x9: // no mod
            break;

        case 0xA: // shift
            kp->mods |= (SHIFT_MOD);
            kp->c = supr_shift_lookup[supr_key];
            break;
        case 0xB: // alt
            kp->mods |= (ALT_MOD);
            break;
        case 0xD: // ctrl
            kp->mods |= (CTRL_MOD);
            break;
        case 0xC: // shift + alt
            kp->mods |= (SHIFT_MOD | ALT_MOD);
            kp->c = supr_shift_lookup[supr_key];
            break;
        case 0xE: // shift + ctrl
            kp->mods |= (CTRL_MOD | SHIFT_MOD);
            kp->c = supr_shift_lookup[supr_key];
            break;
        case 0xF: // ctrl + alt
            kp->mods |= (CTRL_MOD | ALT_MOD);
            break;
        case 0x10: // shift + ctrl + alt
            kp->mods |= (CTRL_MOD | SHIFT_MOD | ALT_MOD);
            kp->c = supr_shift_lookup[supr_key];
            break;
    }
}

Keypress
__supr_arrow_mod(char *buf, ssize_t n)
{
    Keypress kp;
    int      supr_key;
    int      supr_mod;
    char     c;

    sscanf(buf, "\x1b[%d;%d%c", &supr_key, &supr_mod, &c);
    kp = __get_arrow_kp(c);
    kp.mods |= SUPR_MOD;
    __supr_get_mods(&kp, supr_mod, supr_key);
    return kp;
}

Keypress
__supr_mod(char *buf, ssize_t n)
{
    Keypress kp;
    int      supr_key;
    int      supr_mod;

    sscanf(buf, "\x1b[%d;%du", &supr_key, &supr_mod);
    // printf("BUFFER size: %zd -> %d %d \n", n, supr_key, supr_mod);
    /* defaults */
    kp.c    = supr_key;
    kp.mods = SUPR_MOD;

    __supr_get_mods(&kp, supr_mod, supr_key);

    return kp;
}

void
__esc_special(char *buf)
{
    ssize_t  n;
    Keypress kp;

    switch (n = read(STDIN_FILENO, buf + 1, BUFSIZE - 1))
    {
        case 2:
            if (buf[1] != '[')
                goto __normal__; // just to avoid nesting

            kp = __get_arrow_kp(buf[2]);
            /* Is an arrow, or INVALID_KP if not. */
            if (kh_valid_kp(kp))
            {
                __kp_action(kp);
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
            break;

        default:
            if (buf[n] == 'u')
                kp = __supr_mod(buf, n);

            else if (buf[n] >= 'A' && buf[n] <= 'D')
                kp = __supr_arrow_mod(buf, n);

            __kp_action(kp);
            break;
    }
}

static void *
__keyboard_handler(void *args)
{
    ssize_t n;
    char    buf[BUFSIZE];

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
