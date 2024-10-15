#include "vshkh.h"
#include <assert.h>
#include <stddef.h>

extern Buffer keypress_buffer;

/* Initialize the buffer */
void buffer_new();

/* Add a keypress to the buffer and
 * return added keypress. */
Keypress buffer_add(Keypress kp);

/* Remove an element from the buffer
 * and return it */
Keypress buffer_pop();

/* Get the top of the buffer */
Keypress buffer_top();

/* Destroy the buffer, all data
 * in the buffer are lost */
void buffer_destroy();
