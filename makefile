debug:
	gcc -Wall -Wextra -fsanitize=address,null buffer.c dynarray.c keybinds.c keyboard.c test.c utils.c -o test -g -O0

final:
	gcc buffer.c dynarray.c keybinds.c keyboard.c test.c utils.c -o test -O3


