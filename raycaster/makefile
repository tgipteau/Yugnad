CC = clang
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined

RAYLIB_CFLAGS = -I/opt/homebrew/Cellar/raylib/6.0/include
RAYLIB_LIBS = -L/opt/homebrew/Cellar/raylib/6.0/lib -lraylib 

main: main.o map.o
	$(CC) $(CFLAGS) -o main main.o map.o $(RAYLIB_LIBS)

map.o: map.c map.h
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) -c map.c

main.o: main.c map.h
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) -c main.c

clean:
	rm -f main main.o map.o
