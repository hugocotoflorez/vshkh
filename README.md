# vshkh

Very Simple Hugo's Keyboard Handler

## About

Description should be written here.

## User Interface

Function descriptions and those stuff are written in vshkh.h

## Binds formatting

### Mods parsing format

Using 'a' as example char

| Mods | format |
| :---: | :---: |
| **Normal** | "a" |
| **Shift** | "A" |
| **ctrl** | "^A" |
| **alt** | "&a" |
| **Supr** | "@a" |
| **Shift + ctrl** | Not allowed |
| **Shift + alt** | "&A" |
| **Shift + supr** | "@A" |
| **Shift + ctrl + alt** | "&^A" |
| **Shift + ctrl + supr** | "^@A" |
| **Shift + alt + supr** | &@A" |
| **Shift + ctrl + alt +supr** | "^&@A" |
| **ctrl + alt** | "^&a" |
| **ctrl + supr** | "^@a" |
| **ctrl + alt +supr** | "^&@a" |
| **alt + supr** | "&@a" |

> [!CAUTION]
> Ctrl without supr dont allow lowercase chars

## Potential issues + todos

- **Potential issue: CTRL + lowercase is read as ctrl + upercase and ctrl + shift cant be read**
- CTRL is too limited
- CTRL + SHIFT + KEY: crash
- CTRL + 1,9,0 dont exists
- ALT + CTRL set shift
- ALT + CTRL + SHIFT uset shift
- ^&A only maps to C+Alt+a, if shift is used dont call func
- right + left + down at the same time breaks the handler. with -O3 it doesnt happened xd

