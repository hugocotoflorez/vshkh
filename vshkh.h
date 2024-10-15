#ifndef VSHKH_H
#define VSHKH_H

/***************************************************
 * VSHKH                                           *
 ***************************************************
 * Author: Hugo Coto Florez                        *
 *                                                 *
 * Wellcome to Very Simple Hugo's Keyboard Handler *
 * The aim of this proyect is to give an easy to   *
 * use keyboard handler. This proyect is part of   *
 * nhoutpad2.                                      *
 *                                                 *
 * Contact info: hugo.coto@rai.usc.es              *
 * -> github.com/hugocotoflorez(/vshkh)            *
 ***************************************************/


/***************************************************
 * ---| MISC                                  |--- *
 ***************************************************/

/* User dont have to handle this struct manually,
 * but due to Author's metodology it is shared
 * to allow custom implementations.
 * See kh_has_ctrl and related. */
typedef enum
{
    NO_MOD = 0,
    CTRL_MOD,
    SHIFT_MOD,
    ALT_MOD,
    IS_ARROW,
    IS_INVALID,
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
 * once it is created. The return value is the
 * thread id. */
int kh_start();

/* Pause the handler, the input would be read
 * once it starts again */
int kh_pause();

/* Close the handler and restore all values
 * to default */
int kh_end();

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

/* Return a Keybind with the same sequence at
 * str, being str a correct formatted textual
 * keybind. If some error happened return an
 * invalid Keybind. It can be tested using
 * kh_valid_kb(Keybind).
 * See STR Keybind format before. */
Keybind kh_bind_parse(const char *str);

/******************************************************
 * ---| STR Keybind format                       |--- *
 ******************************************************/

/* the max keypresses are stored in KEYBINDLEN
 * Modifiers:
 *  - Ctrl: represented as ^. Example: ctrl+a -> ^a
 *  - Shift: just use an Upercase character
 * Characters: alphas from a-z
 *
 * Symbols and numbers are not valid chars */


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

int kb_is_equal(Keybind, Keybind);
int kp_is_equal(Keypress, Keypress);


/***************************************************
 * ---| Array (dynarray.c)                    |--- *
 ***************************************************/

/* Dynamic array for store keybinds. DATA array entries
 * can be both valid or invalid keybinds. Invalid keybinds
 * could be overwritten. If array is full, it would increment
 * by ARRINC. */
typedef struct
{
    int length;     /* Number of elements in buffer. */
    int alloc_size; /* Size of data, max number of elements
                     * that can be stored in data without
                     * overflow */
    Keybind *data;  /* Elements in the buffer */
} Array;

/* Array grow as
 * new size = old size + ARRINC */
#define ARRINC 4

/* As this array is not going to be used more than
 * once, Im going to use kp_array as the
 * buffer on which every function call applied. */
extern Array kb_array;

/* Initialize the array*/
void array_new();

/* Add a keybind to the buffer and
 * return added keybind. */
Keybind array_add(Keybind kb);

/* Remove an keybind from the array
 * and return it */
Keybind array_pop(Keybind);

/* Get the function if keyfind is found or NULL.*/
BindFunc array_find(Keybind);

/* Destroy the buffer, all data
 * in the buffer are lost */
void array_destroy();


/***************************************************
 * ---| Buffer (buffer.c)                     |--- *
 ***************************************************/

/* As mentioned above, I share this info to allow
 * user modify and customize the behaviour of the
 * handler.
 * BUT if nothing is changed here all would work as
 * expected, so please dont touch anything if you are
 * not sure about what are you doing. Thanks. */

/* Buffer is a circular buffer of dynamic size.
 * It aims to be easy to use and flexible, as well
 * as fast.
 * As this implementation is done to store keypresses
 * the types and other stuff are based on author
 * knowledge about future usage. */
typedef struct
{
    int length;           /* Number of elements in buffer. */
    int alloc_size;       /* Size of data, max number of elements
                           * that can be stored in data without
                           * overflow */
    Keypress *data;       /* Elements in the buffer */
    Keypress *next_ptr;   /* Pointer to next element that have
                           * to be processed */
    Keypress *insert_ptr; /* Pointer to the next position
                           * where a new element have to
                           * be stored */
} Buffer;

/* As this buffer is not going to be used more than
 * once Im going to use kp_buffer as the
 * buffer on which every function call applied. */
static Buffer kp_buffer;

/* Initialize the buffer */
void buffer_new();

/* Add a keypress to the buffer and
 * return added keypress. */
Keypress buffer_add(Keypress);

/* Remove an element from the buffer
 * and return it */
Keypress buffer_pop();

/* Get the top of the buffer */
Keypress buffer_top();

/* Destroy the buffer, all data
 * in the buffer are lost */
void buffer_destroy();


#endif
