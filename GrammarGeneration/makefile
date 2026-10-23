CC = clang
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined

RAYLIB_CFLAGS = -I/opt/homebrew/Cellar/raylib/6.0/include
RAYLIB_LIBS = -L/opt/homebrew/Cellar/raylib/6.0/lib -lraylib

gramgen : gramgen.o
	clang $(CFLAGS) -o gramgen gramgen.o $(RAYLIB_LIBS)

gramgen.o : gramgen.c
	clang $(CFLAGS) $(RAYLIB_CFLAGS) -c gramgen.c -o gramgen.o
