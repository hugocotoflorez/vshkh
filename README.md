# vshkh

Very Simple Hugo's Keyboard Handler

## About

Description should be written here.

## User Interface

Function descriptions and those stuff are written in vshkh.h

## Stuff that work

Keypress read
- [x] alphas (a-z, a-z symbols)
- [x] shift + alphas
- [x] ctrl + '@', 'a'-'z', '[', '\', ']', '^', '_'
- [x] alt + alphas
- [x] supr + alphas

Binds
- [x] single key bind
- [x] ctrl + single key bind
- [x] alt + single key bind
- [x] ctrl + alt
- [x] supr + alt + shift + ctrl

## Potential issues + todos

TODO:
- CTRL is too limited
- CTRL + SHIFT + KEY: crah
- CTRL + 1,9,0 dont exists

- ALT + CTRL set shift
- ALT + CTRL + SHIFT uset shift
- ^&A only maps to C+Alt+a, if shift is used dont call func

- right + left + down at the same time breaks the handler. with -O3 it doesnt happened xd

## Binds formatting

### Mods

-**Normal**: lowerase or some symbols
-**Shift**: upercase or some symbols
-**Shift + ctrl**: ^ + A-Z or some symbols
-**Shift + alt**: & + A-Z or some symbols
-**Shift + supr**: All symbols that require physical shift
-**Shift + ctrl + alt**: ^& + A-Z or some symbols
-**Shift + ctrl + supr**: ^ + All symbols that require physical shift
-**Shift + alt + supr**: &@ + All symbols that require physical shift
-**Shift + ctrl + alt +supr** ^&@ + All symbols that require physical shift
-**ctrl**: Cant be mapped, just shift ctrl
-**ctrl + alt**: Cant be mapped, just shift ctrl alt
-**ctrl + supr**: ^@ + lowercase or symbols that dont need shift
-**ctrl + alt +supr**: ^&@ + lowercase or symbols that dont need shift
-**alt**: & + lowercase or some symbols
-**alt + supr**: &@ + lowercase or some symbols
-**supr**: @ + lowercase or some symbols

> [!CAUTION]
> Ctrl without supr dont allow lowercase chars

