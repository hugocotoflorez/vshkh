#include "vshkh.h"

/* Return one of the values above.
 * Arrowkey can be used as a boolean
 * value, so if return value is false,
 * keypress is not an arrow key. */
Arrowkey
kh_is_arrow(Keypress kp)
{
    // it only control have to be set
    if (kp.mods != CTRL_MOD)
        return NO_ARROW;

    switch (kp.c)
    {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        default:
            return NO_ARROW;
    }
}

/* Following functions return >0 if keypress has
 * the modifier in the function name pressed or
 * 0 otherwise */
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

int
kh_valid_kp(Keypress kp)
{
    return !kp_is_equal(kp, INVALID_KP);
}
