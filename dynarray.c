#include "vshkh.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

extern Array kb_array;

/* Value used to increment the buffer.
 * new size = old size + BUFINC */
#define BUFINC 4;

/* Get the adress of the first occurence of kb or NULL */
Keybind              *__array_get(Keybind kb);
/* Get the adress of the first occurence of INVALID_KB or NULL */
static Keybind       *__get_invalid();
/* Get the first empty adress or used by an invalid kb */
static Keybind       *__get_empty();
/* Grow the array */
static void           __grow();
/* Insert an element, dont change any variable or check anything */
static inline Keybind __insert(Keybind *kbptr, Keybind kb);

static inline Keybind
__insert(Keybind *kbptr, // Pointer to a data-array position
                         // where KB is going to be inserted
         Keybind kb      // Keybind that is going to be inserted
)
{
    /* This function dont increment array size */
    return (*kbptr = kb);
}

// grow kb_array by BUFINC
static void
__grow()
{
    kb_array.alloc_size += BUFINC;
    kb_array.data = realloc(kb_array.data, kb_array.alloc_size);
}

static Keybind *
__get_invalid()
{
    return __array_get(INVALID_KB);
}

static Keybind *
__get_empty()
{
    if (kb_array.alloc_size == kb_array.length)
        /* return the first empty keybind from data array. It returns NULL
         * if no invalid entry were found */
        return __get_invalid();
    else
        /* Return the last empty position and increment length */
        return kb_array.data + kb_array.length++ - 1;
    return NULL;
}

/* Check if keypresses of both keybinds are the same,
 * return 0 is they are not equal, != 0 otherwise */
int
kb_is_equal(Keybind kb1, Keybind kb2)
{
    Keypress *kb1kp = kb1.kp;
    Keypress *kb2kp = kb2.kp;

    while (kb1kp && kb1kp)
    {
        if (!kp_is_equal(*kb1kp, *kb2kp))
            return 0;
        ++kb1kp;
        ++kb2kp;
    }

    return kb1kp == kb2kp;
}

/* Get the first adress of kp if it is in the array */
Keybind *
__array_get(Keybind kb)
{
    for (int i = 0; i < kb_array.length; i++)
        /* Return the address of the keybind if its invalid */
        if (kb_is_equal(kb, kb_array.data[i]))
            return kb_array.data + i;
    return NULL;
}

/* Initialize the array*/
void
array_new()
{
    kb_array = (Array) {
        .data       = NULL,
        .length     = 0,
        .alloc_size = 0,
    };
}

/* Add a keybind to the buffer and
 * return added keybind. */
Keybind
array_add(Keybind kb)
{
    Keybind *cell = __get_empty();
    if (cell)
        return __insert(cell, kb);

    __grow();

    return array_add(kb);
}

/* Remove an keybind from the array
 * and return it */
Keybind
array_pop(Keybind)
{
}

/* Get the function if keyfind is found or NULL.*/
BindFunc array_find(Keybind);

/* Destroy the buffer, all data
 * in the buffer are lost */
void array_destroy();
