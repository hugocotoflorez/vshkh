#include "vshkh.h"
#include <stdio.h>

void array_new();

/* Add a keybind to the buffer and
 * return added keybind. */
Keybind array_add(Keybind kb);

/* Remove an keybind from the array
 * and return it */
Keybind array_pop(Keybind);

/* Get the function if keyfind is found or NULL.
 * It is posible to call it as array_exec(kb)(); */
BindFunc array_exec(Keybind);

/* Destroy the buffer, all data
 * in the buffer are lost */
void array_destroy();

void
action1()
{
    printf("Hello\n");
}

#define PRINTPASS
inline void
pass(void)
{
#ifdef PRINTPASS
    puts("PRINTPASS");
#endif
    return;
}

/* TODO: I cannot check anything because
 * keybinds are not yet implemented
int
main()
{
    Keybind kb1, kb2, kb3, kb4;
    array_new();
    array_add(kb1);
    array_add(kb2);
    array_add(kb3);
    array_add(kb4);
    (array_exec(array_pop(kb1)) ?: pass)();
}
 */
