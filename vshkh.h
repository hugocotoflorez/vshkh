#ifndef VSHKH_H
#define VSHKH_H


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

#define INVALID_KEYPRESS \
    (Keypress) { .c = '\0', .mods = IS_INVALID, }

/*****************************/
/* ---| VSHHK interface |--- */
/*****************************/

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
 * if passed to kh_is_valid. */
Keypress kh_get();

/* Wait for keyboard input and return the first
 * keypress */
Keypress kh_wait();

/* Ignore buffered keypresses and empty the
 * buffer. Removed keypressed cant be accesed
 * after calling this funcion. */
Keypress kh_flush();


/*****************************/
/* ---| Misc tools      |--- */
/*****************************/

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
int kh_is_valid(Keypress);

/*****************************/
/* ---| Buffer          |--- */
/*****************************/
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
 * once Im going to use keypress_buffer as the
 * buffer on which every function call applied. */
static Buffer keypress_buffer;

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
