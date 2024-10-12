#include "vshkh.h"

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
int kh_valid_kp(Keypress);

