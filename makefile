all:
	gcc -Wall buffer.c dynarray.c keybinds.c keyboard.c test.c utils.c -o test -g
