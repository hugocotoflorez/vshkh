all:
	gcc -Wall -Wextra -fsanitize=address,null buffer.c dynarray.c keybinds.c keyboard.c test.c utils.c -o test -g -O0
