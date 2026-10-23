#include "map.h"

// BSP functions
void split_dungeon(Dungeon *parent){
	/* splits parent in two child dungeons. */
	 
	 int vertical_split_ok = 1;
	 int horizontal_split_ok = 1;
	 
    // leaf if too small for split
    if (parent->width < (MIN_ROOM_SIDE + 2*ROOM_MARGIN) *2)
		vertical_split_ok = 0;
    if (parent->height < (MIN_ROOM_SIDE + 2*ROOM_MARGIN) *2)
		horizontal_split_ok = 0;
	
	// stop if MAX_NB_ROOM attained 
	if (pow(2, parent->idx+1) > MAX_ROOM_NB)
		return; 
    
    Dungeon *child1 = malloc(sizeof(Dungeon));
    Dungeon *child2 = malloc(sizeof(Dungeon));

    if (child1 == NULL || child2 == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    
    if (rand() % 2 == 0) { // vertical split

		if (vertical_split_ok == 0)
			return;
		int min_split = MIN_ROOM_SIDE + 2 * ROOM_MARGIN;
		int max_split = parent->width - min_split;
		int split_x = min_split + RAND_RANGE(max_split - min_split);

		// child 1 as left div
        child1->width = split_x;
        child1->height = parent->height;
        child1->start_x = parent->start_x;
        child1->start_y = parent->start_y;

		// child 2 as right div
        child2->width = parent->width - split_x;
        child2->height = parent->height;
        child2->start_x = parent->start_x + split_x;
        child2->start_y = parent->start_y;

    } else { // horizontal split

        if (horizontal_split_ok == 0)
			return;

        int min_split = MIN_ROOM_SIDE + 2 * ROOM_MARGIN;
		int max_split = parent->height - min_split;
		int split_y = min_split + RAND_RANGE(max_split - min_split);

		// child 1 as top div
        child1->width = parent->width;
        child1->height = split_y;
        child1->start_x = parent->start_x;
        child1->start_y = parent->start_y;

		// child 2 as bottom div
        child2->width = parent->width;
        child2->height = parent->height - split_y;
        child2->start_x = parent->start_x;
        child2->start_y = parent->start_y + split_y;
    }

	// recognize children
    parent->child1 = child1;
    parent->child2 = child2;

	// set children idx to parent+1
    child1->idx = parent->idx + 1;
    child2->idx = parent->idx + 1;

	// children have no children yet
    child1->child1 = NULL;
    child1->child2 = NULL;
    child2->child1 = NULL;
    child2->child2 = NULL;
    
    //children networks are VOID
    for (int x=0 ; x<DUNGEON_WIDTH ; x++){
		for (int y=0 ; y<DUNGEON_HEIGHT ; y++){
			child1->network[y][x].nature = NAT_VOID;
			child2->network[y][x].nature = NAT_VOID;
	}}

    // recursively split the two children.
    split_dungeon(child1);
    split_dungeon(child2);
}

void generate_corridor(Dungeon *parent){
	/* creates L corridor between children connect points. 
	 * Corridor is added to parent network. */
	 
    Dungeon *child1 = parent->child1;
    Dungeon *child2 = parent->child2;

    int x1 = child1->connect_point_x;
    int y1 = child1->connect_point_y;

    int x2 = child2->connect_point_x;
    int y2 = child2->connect_point_y;

    // Horizontal
    int step_x = (x2 > x1) ? 1 : -1;

    for (int x = x1; x != x2; x += step_x){
        parent->network[y1][x].nature = NAT_GROUND;
        parent->network[y1][x].occupied = 0;
	}

    // Vertical
    int step_y = (y2 > y1) ? 1 : -1;

    for (int y = y1; y != y2; y += step_y){
        parent->network[y][x2].nature = NAT_GROUND;
        parent->network[y][x2].occupied = 0;
    }

    // Point d'arrivée
    parent->network[y2][x2].nature = NAT_GROUND;

}

void generate_room(Dungeon *div){
	
	// it is a parent (has children dungeons)
	if (div->child1 != NULL) {
		generate_room(div->child1);
		generate_room(div->child2);
		// generate corridors between children
		generate_corridor(div);
		// copy children rooms to parent network
		
		int nb_grounds = 0; // reservoir sampling for connect point
		for (int x=0 ; x<DUNGEON_WIDTH ; x++){
			for (int y=0 ; y<DUNGEON_HEIGHT ; y++){
				if (div->child1->network[y][x].nature == NAT_GROUND ||
					div->child2->network[y][x].nature == NAT_GROUND) {
					div->network[y][x].nature = NAT_GROUND;
					div->network[y][x].occupied = 0;
					nb_grounds++;
					
					// determine new connect point in network (reservoir sampling)
					if (rand() % nb_grounds == 0) {
					div->connect_point_x = x;
					div->connect_point_y = y;
					}
				}
			}
		}
		
		
	} 
	
	else { // it is a leaf (no children dungeon)
		// define room span
			int room_width = MIN_ROOM_SIDE 
					+ RAND_RANGE(div->width - MIN_ROOM_SIDE - 2*ROOM_MARGIN);	

			int room_height = MIN_ROOM_SIDE
					+ RAND_RANGE(div->height - MIN_ROOM_SIDE - 2*ROOM_MARGIN);

			int min_x = div->start_x + ROOM_MARGIN;
			int max_x = div->start_x + div->width
					- ROOM_MARGIN - room_width;
			int room_start_x = min_x + RAND_RANGE(max_x - min_x);

			int min_y = div->start_y + ROOM_MARGIN;
			int max_y = div->start_y + div->height
					- ROOM_MARGIN - room_height;

			int room_start_y = min_y + RAND_RANGE(max_y - min_y);

		// update network
			for (int x=room_start_x ; x<room_width+room_start_x ; x++){
				for (int y=room_start_y ; y<room_height+room_start_y ; y++){
					div->network[y][x].nature = NAT_GROUND;
					div->network[y][x].occupied = 0;
					}
				}
				
		// get random connect point 
			div->connect_point_x = room_start_x + rand() % room_width ;
			div->connect_point_y = room_start_y + rand() % room_height ;

	}
		
	return;
}

void free_dungeon_entities(Dungeon* dungeon){
	/* Recursively free mallocs of dungeons used for construction.*/
	if (dungeon->child1 != NULL){ // not a leaf
		free_dungeon_entities(dungeon->child1);
		free_dungeon_entities(dungeon->child2);
	}
	free(dungeon);
}

// generate empty map
Dungeon generate_map(void){
    srand(time(NULL));

    Dungeon main_dungeon = {
        .height = DUNGEON_HEIGHT,
        .width = DUNGEON_WIDTH,
        .start_x = 0,
        .start_y = 0,
        .idx = 0,
        .child1 = NULL,
        .child2 = NULL,
    };
    
    // Empty main dungeon network
    for (int x=0 ; x<DUNGEON_WIDTH ; x++){
		for (int y=0 ; y<DUNGEON_HEIGHT ; y++){
			main_dungeon.network[y][x].nature = NAT_VOID;
			main_dungeon.network[y][x].occupied = 1;
	}}

    split_dungeon(&main_dungeon);
    generate_room(&main_dungeon);
    
    // get starting point in dungeon (stored in connect_point)
    // and exit point
    int nb_grounds = 0; // reservoir sampling for starting point
		for (int x=0 ; x<DUNGEON_WIDTH ; x++){
			for (int y=0 ; y<DUNGEON_HEIGHT ; y++){
				if (main_dungeon.network[y][x].nature == NAT_GROUND){
					nb_grounds++;
					// determine starting point (reservoir sampling)
					if (rand() % nb_grounds == 0) {
					main_dungeon.connect_point_x = x;
					main_dungeon.connect_point_y = y;
					}
					//determine exit point
					if (rand() % nb_grounds == 0) {
					main_dungeon.exit_x = x;
					main_dungeon.exit_y = y;
					}
				
				}
			}
		}

	main_dungeon.network[main_dungeon.exit_y][main_dungeon.exit_x]
		.nature = NAT_EXIT;
		
	main_dungeon.discovered = 1;
	
    free_dungeon_entities(main_dungeon.child1);
	free_dungeon_entities(main_dungeon.child2);
    
    return main_dungeon;
   
}
// ..
Dungeon generate_bonfire_room(void){
	Dungeon bonfire_room = {
        .height = DUNGEON_HEIGHT,
        .width = DUNGEON_WIDTH,
        .start_x = 0,
        .start_y = 0,
        .idx = 0,
        .child1 = NULL,
        .child2 = NULL,
        .discovered = 1
    };
    
    // Empty dungeon network
    for (int x=0 ; x<DUNGEON_WIDTH ; x++){
		for (int y=0 ; y<DUNGEON_HEIGHT ; y++){
			bonfire_room.network[y][x].nature = NAT_VOID;
			bonfire_room.network[y][x].occupied = 1;
	}}
    
    // bonfire room is 8 by 8
    // generate vertical corridor and central room
    for (int y=DUNGEON_HEIGHT/4 ; y<=3*DUNGEON_HEIGHT/4 ; y++){
		bonfire_room.network[y][DUNGEON_WIDTH/2].nature = NAT_GROUND;
		bonfire_room.network[y][DUNGEON_WIDTH/2].occupied = 0;
	}
	
	for (int y=(DUNGEON_HEIGHT/2)-4 ; y<(DUNGEON_HEIGHT/2)+4 ; y++){
		for (int x=(DUNGEON_WIDTH/2)-4 ; x<=(DUNGEON_WIDTH/2)+4 ; x++){
			bonfire_room.network[y][x].nature = NAT_GROUND;
			bonfire_room.network[y][x].occupied = 0;
		}
	}
	
	bonfire_room.connect_point_x = DUNGEON_WIDTH/2;
	bonfire_room.connect_point_y = 3*DUNGEON_HEIGHT/4;
	
	bonfire_room.exit_x = DUNGEON_WIDTH/2;
	bonfire_room.exit_y = DUNGEON_HEIGHT/4;
	bonfire_room.network[bonfire_room.exit_y][bonfire_room.exit_x]
		.nature = NAT_EXIT;
	
	
	
	return bonfire_room;
}
