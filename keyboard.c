#include "vshkh.h"

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

