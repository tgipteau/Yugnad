#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>


#define DUNGEON_HEIGHT 30
#define DUNGEON_WIDTH 30
#define MAX_ROOM_NB 8
#define MIN_ROOM_SIDE 5
#define ROOM_MARGIN 1

#define NAT_GROUND	0
#define NAT_VOID	1
#define NAT_WALL	2
#define NAT_EXIT	3

//UTILS
#define RAND_RANGE(x) ((x) == 0 ? 0 : rand() % ((x) + 1))
#define SECONDS(x)	((x)*1000000)

typedef struct Tile {
		int occupied;
		int nature;
} Tile;


typedef struct Dungeon{
	int height;
	int width;
	int start_x;
	int start_y;
	int idx;
	int discovered;

	struct Dungeon *child1;
	struct Dungeon *child2;
	
	Tile network[DUNGEON_HEIGHT][DUNGEON_WIDTH];
	
	int connect_point_x, connect_point_y;
	int exit_x, exit_y;
 
} Dungeon;
	

//////////////////////
/* GENERATOR HEADER */

	Dungeon generate_bonfire_room(void);
	void split_dungeon(Dungeon *parent);
	void generate_room(Dungeon *div);
	void generate_corridor(Dungeon *parent);
	Dungeon generate_map(void);
	void free_dungeon_entities(Dungeon* dungeon);

#endif

