#include "vshkh.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

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
    if (array_exist(kb))
        return array_exec(kb);

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
            case '\\': // escape symbols
                ++str;
                kp.mods |= SHIFT_MOD;
                /* all ^&@ uses shift mod */
                /* if *str is \0 it can crash */
                goto __add__;

            case '^': // CTRL
                kp.mods |= CTRL_MOD;
                /* If the modifier is ctrl and dont have supr it
                 * set shift mod (ctrl requires shift is not used
                 * with super */
                if (!(kp.mods & SUPR_MOD))
                    kp.mods |= SHIFT_MOD;
                break;

            case '&': // ALT
                kp.mods |= ALT_MOD;
                break;

            case '@': // SUPR
                kp.mods |= SUPR_MOD;
                /* Disable shift if ctrl had set it, because shift is
                 * set by the char that is the last part of the bind */
                kp.mods &= ~SHIFT_MOD;
                break;

                // clang-format off
            case 'A' ... 'Z': // Upercase
            case '<': case '_': case '>': case '?': // Chars that uses shift modifier
            case ')': case '!': case '#': case '$':
            case '%': case '*': case '(': case ':':
            case '{': case '|': case '}': case '~': // clang-format on
                kp.mods |= SHIFT_MOD;
                goto __add__; // avoid implicit-fallthought

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
