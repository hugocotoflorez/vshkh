#define ASCII_TABLE_REPR
#include "vshkh.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define DEBUG

extern Array kb_array;

/* Add a keybind. If same sequences of keypresses are
 * yet stored, it only modified func. Note
 * that previous one would be overwritten. */
void
kh_bind_add(Keybind kb)
{
    if (array_exist(kb))
        array_modify(kb);

    else
        array_add(kb);
}

/* Remove a keybind if exists */
void
kh_bind_remove(Keybind kb)
{
    if (array_exist(kb))
        array_pop(kb);
}

/* Get the function from keybind that share KB
 * keypresses or NULL if not found */
BindFunc
kh_bind_get(Keybind kb)
{
    printf("Getting %c with mods %d\n", kb.kp[0].c, kb.kp[0].mods);
    if (array_exist(kb))
    {
        printf("Func = %p\n", array_exec(kb));
        return array_exec(kb);
    }
    printf("No in array!\n");
    return NULL;
}

Keybind
kh_bind_new()
{
    return (Keybind) { 0 };
}

/* Set the func to a keybind. This call dont
 * modify yet stored keybinds, so it is needed
 * to be called BEFORE add */
void
kh_bind_set_func(Keybind *kb, BindFunc func)
{
    kb->func = func;
}

/* Append a keypress to the keybind */
Keybind
kh_bind_append(Keybind *kb, Keypress kp)
{
    Keypress *kpptr = kb->kp;
    while (kpptr->c)
        ++kpptr;

    *kpptr = kp;

    return *kb;
}

/* Arrows are not implemented */
/* Return a Keybind with the same sequence at
 * str, being str a correct formatted textual
 * keybind. If some error happened return an
 * invalid Keybind. It can be tested using
 * kh_valid_kb(Keybind).
 * See STR Keybind format before. */
Keybind
kh_bind_parse(const char *str)
{
    Keybind  kb;
    Keypress kp;

    kb = kh_bind_new();
    kp = (Keypress) { 0 };

    while (*str)
    {
        switch (*str)
        {
            case '\\':
                ++str;
                goto __add__;

            case '^':
                kp.mods |= CTRL_MOD;
                break;

            case 'A' ... 'Z':
                kp.mods |= SHIFT_MOD;

            default:
            __add__:
                kp.c = *str;
                kh_bind_append(&kb, kp);
                kp = (Keypress) { 0 };
                break;
        }
        ++str;
    }
    return kb;
}

/******************************************************
 * ---| STR Keybind format                       |--- *
 ******************************************************/

/* the max keypresses are stored in KEYBINDLEN
 * Modifiers:
 *  - Ctrl: represented as ^. Example: ctrl+a -> ^a
 *  - Shift: just use an Upercase character
 * Characters: alphas from a-z
 * Special symbols: \^ -> ^ (for any char)
 *
 * Symbols and numbers are not valid chars */
