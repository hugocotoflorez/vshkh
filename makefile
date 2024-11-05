debug:
	gcc -Wall -Wextra -fsanitize=address,null buffer.c dynarray.c keybinds.c keyboard.c test.c utils.c -o test -g -O0

final:
	gcc buffer.c dynarray.c keybinds.c keyboard.c test.c utils.c -o test -O3


example:
	gcc buffer.c dynarray.c keybinds.c keyboard.c example.c utils.c -o example -O3
	./example
