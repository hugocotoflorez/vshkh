#ifndef VSHKH_H
#define VSHKH_H

/***************************************************
 * VSHKH user header file                          *
 ***************************************************
 * Author: Hugo Coto Florez                        *
 *                                                 *
 * Wellcome to Very Simple Hugo's Keyboard Handler *
 * The aim of this proyect is to give an easy to   *
 * use keyboard handler. This proyect is part of   *
 * nhoutpad2.                                      *
 *                                                 *
 * User header hide functions that user MUST NOT   *
 * use. If the function prefix is not kh_ and is   *
 * not declared in this file, use it without any   *
 * waranty.                                        *
 *                                                 *
 * Contact info: hugo.coto@rai.usc.es              *
 * -> github.com/hugocotoflorez(/vshkh)            *
 ***************************************************/

/*********************************************************
 * Function Apendix                                      *
 *********************************************************
 * Arrowkey kh_is_arrow(Keypress);                       *
 * Keybind  kh_bind_parse(const char *str);              *
 * Keybind  kh_bind_append(Keybind *kb, Keypress kp);    *
 * void     kh_bind_set_func(Keybind kb, BindFunc func); *
 * Keybind  kh_bind_new();                               *
 * BindFunc kh_bind_get(Keybind kb);                     *
 * void     kh_bind_remove(Keybind);                     *
 * void     kh_bind_add(Keybind);                        *
 * int      kh_valid_kb(Keybind);                        *
 * Keypress kh_flush();                                  *
 * Keypress kh_wait();                                   *
 * Keypress kh_get();                                    *
 * void     kh_end();                                    *
 * void     kh_pause();                                  *
 * void     kh_start();                                  *
 * int      kh_has_ctrl(Keypress);                       *
 * int      kh_has_shift(Keypress);                      *
 * int      kh_has_alt(Keypress);                        *
 * int      kh_valid_kp(Keypress);                       *
 *********************************************************/

/***************************************************
 * ---| MISC                                  |--- *
 ***************************************************/

/* User dont have to handle this struct manually,
 * but due to Author's metodology it is shared
 * to allow custom implementations.
 * See kh_has_ctrl and related. */
typedef enum
{
    NO_MOD     = 1 << 0,
    CTRL_MOD   = 1 << 1,
    SHIFT_MOD  = 1 << 2,
    ALT_MOD    = 1 << 3,
    IS_ARROW   = 1 << 4,
    IS_INVALID = 1 << 5,
} Mods;

/* Keypress return a character with the mods used.
 * If an upercase letter is returned, c is an upercase
 * letter and shift mod is set.
 * If Keypress is typecasted to char it store the char
 * inside Keypress */
typedef struct
{
    char c;
    Mods mods;
} Keypress;

#define INVALID_KP \
    (Keypress) { .c = '\0', .mods = IS_INVALID, }

/***************************************************
 * ---| VSHHK interface (keyboard.c)          |--- *
 ***************************************************/

/* The keyboard handler would be initialized in
 * a different thread, so this funcion returns
 * once it is created. This function also wake up
 * the handler if paused. */
void kh_start();

/* Pause the handler, the input would be read
 * once it starts again */
void kh_pause();

/* Close the handler and restore all values
 * to default */
void kh_end();

/* Get a keypress that is waiting in buffer and
 * remove it from buffer. If no keypress is in
 * buffer it return a keypress that return 0
 * if passed to kh_valid_kp. */
Keypress kh_get();

/* Wait for keyboard input and return the first
 * keypress */
Keypress kh_wait();

/* Ignore buffered keypresses and empty the
 * buffer. Removed keypressed cant be accesed
 * after calling this funcion. */
Keypress kh_flush();


/***************************************************
 * ---| Keybinds (keybinds.c)                 |--- *
 ***************************************************/

/* The keyboard handler can also execute a
 * void(*foo)(void) function if set to a key
 * or sequence of keys. Funtions below are
 * used to set or delete keybinds. */

/* Function prototype */
typedef void (*BindFunc)(void);

/* Max number of keypresses that can be stored
 * in a single keybind */
#define KEYBINDLEN 2

/* A keybind is a sequence of keypresses.
 * The keypresses should be at least 1, and
 * less or equal than KEYBINDLEN. KP is an
 * array of keypresses, NULL terminated. */
typedef struct
{
    Keypress kp[KEYBINDLEN + 1]; // all keypresses have to be valid
    BindFunc func;               // function that is going to be executed
} Keybind;

#define INVALID_KB (Keybind){ 0 }

/* return >0 if keybind is valid or 0 otherwise */
int kh_valid_kb(Keybind);

/* Add a keybind. If same sequences of keypresses are
 * yet stored, it only modified func. Note
 * that previous one would be overwritten. */
void kh_bind_add(Keybind);

/* Remove a keybind if exists */
void kh_bind_remove(Keybind);

/* Get the function from keybind that share KB
 * keypresses or NULL if not found */
BindFunc kh_bind_get(Keybind kb);

/* Initialize an empty keybind */
Keybind kh_bind_new();

/* Set the func to a keybind. This call dont
 * modify yet stored keybinds, so it is needed
 * to be called BEFORE add */
void kh_bind_set_func(Keybind kb, BindFunc func);

/* Append a keypress to the keybind */
Keybind kh_bind_append(Keybind *kb, Keypress kp);


/* Return a Keybind with the same sequence at
 * str, being str a correct formatted textual
 * keybind. If some error happened return an
 * invalid Keybind. It can be tested using
 * kh_valid_kb(Keybind).
 * See STR Keybind format before. */
Keybind kh_bind_parse(const char *str);

/***************************************************
 * ---| Utils (utils.c)                       |--- *
 ***************************************************/

typedef enum
{
    NO_ARROW = 0,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
} Arrowkey;

/* Return one of the values above.
 * Arrowkey can be used as a boolean
 * value, so if return value is false,
 * keypress is not an arrow key. */
Arrowkey kh_is_arrow(Keypress);

/* Following functions return >0 if keypress has
 * the modifier in the function name pressed or
 * 0 otherwise */
int kh_has_ctrl(Keypress);
int kh_has_shift(Keypress);
int kh_has_alt(Keypress);
int kh_valid_kp(Keypress);

#endif
