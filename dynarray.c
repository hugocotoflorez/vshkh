#include "vshkh.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

extern Array kb_array;

/* Value used to increment the buffer.
 * new size = old size + BUFINC */
#define BUFINC 4;

/* Get the adress of the first occurence of kb or NULL */
Keybind *__array_get(Keybind kb);
/* Get the adress of the first occurence of INVALID_KB or NULL */
static Keybind *__get_invalid();
/* Get the first empty adress or used by an invalid kb */
static Keybind *__get_empty();
/* Grow the array */
static void __grow();
/* Pop the element at index I. Index I have to be valid.*/
static Keybind __pop(int i);
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

static Keybind
__pop(int i)
{
    /* Just mark element as invalid, dont move elements */
    Keybind ret      = kb_array.data[i];
    kb_array.data[i] = INVALID_KB;
    return ret;
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

/* Remove a keybind KB from the array
 * and return it */
Keybind
array_pop(Keybind kb)
{
    __pop(kb_array.data - __array_get(kb));
    return INVALID_KB;
}

/* Get the function if keybind is found or NULL.*/
BindFunc
array_exec(Keybind kb)
{
    return __array_get(kb)->func;
}

/* Destroy the buffer, all data
 * in the buffer are lost */
void
array_destroy()
{
    free(kb_array.data);
    // set all fields as usnet
    kb_array = (Array) { 0 };
}
