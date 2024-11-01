#include "vshkh.h"

/* Return one of the values above.
 * Arrowkey can be used as a boolean
 * value, so if return value is false,
 * keypress is not an arrow key. */
Arrowkey
kh_is_arrow(Keypress kp)
{
    if (kp.mods != IS_ARROW)
        return NO_ARROW;

    return kp.c;
}

/* Following functions return >0 if keypress has
 * the modifier in thekeypress or 0 otherwise */
inline int
kh_has_ctrl(Keypress kp)
{
    return kp.mods & CTRL_MOD;
}

inline int
kh_has_shift(Keypress kp)
{
    return kp.mods & SHIFT_MOD;
}

inline int
kh_has_alt(Keypress kp)
{
    return kp.mods & ALT_MOD;
}

inline int
kp_is_equal(Keypress kp1, Keypress kp2)
{
    return kp1.c == kp2.c && kp1.mods == kp2.mods;
}

/* Check if keypresses of both keybinds are the same,
 * return 0 is they are not equal, != 0 otherwise */
int
kb_is_equal(Keybind kb1, Keybind kb2)
{
    for (int i = 0; i < KEYBINDLEN; i++)
    {
        if (!kp_is_equal(kb1.kp[i], kb2.kp[i]))
            return 0;
    }
    return 1;
}

/* return >0 if keybind is valid or 0 otherwise */
inline int
kh_valid_kp(Keypress kp)
{
    return !kp_is_equal(kp, INVALID_KP);
}

/* return >0 if keybind is valid or 0 otherwise */
inline int
kh_valid_kb(Keybind kb)
{
    return !kb_is_equal(kb, INVALID_KB);
}
