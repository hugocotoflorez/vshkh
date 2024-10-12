#include "vshkh.h"

/* return >0 if keybind is valid or 0 otherwise */
int kh_valid_kb(Keybind);

/* Add a keybind. If same sequences of keypresses are
 * yet stored, it only modified func. Note
 * that previous one would be overwritten. */
void kh_bind_add(Keybind);

/* Remove a keybind if exists */
void kh_bind_remove(Keybind);

/* Return a Keybind with the same sequence at
 * str, being str a correct formatted textual
 * keybind. If some error happened return an
 * invalid Keybind. It can be tested using
 * kh_valid_kb(Keybind).
 * See STR Keybind format before. */
Keybind kh_bind_parse(const char *str);

/******************************************************
 * ---| STR Keybind format                       |--- *
 ******************************************************/

/* the max keypresses are stored in KEYBINDLEN
 * Modifiers:
 *  - Ctrl: represented as ^. Example: ctrl+a -> ^a
 *  - Shift: just use an Upercase character
 * Characters: alphas from a-z
 *
 * Symbols and numbers are not valid chars */

