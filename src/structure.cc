#include "structure.h"
#include "tank.h" /* kvuli konstantam TANK_WIDTH a TANK_HEIGHT */
#include "commondef.h"	/* kvuli konstante MAX_PLAYER_CNT */
#include <bits/stdc++.h>

using namespace std;

extern Color fire_color;

structure::structure(char init_id, int xc, int yc,const char * bitmap)
{
	if( (MAX_PLAYER_CNT <= init_id) && (init_id < 2*MAX_PLAYER_CNT) )
		id = init_id;
	else
		fprintf(stderr,"structure: wrong ID\n");
	
	landscape->correct_coord(xc,yc);
	ground.x = xc;
	ground.y = yc;
	ground.w = STRUCTURE_WIDTH;
	ground.h = STRUCTURE_HEIGHT;

	int px = xc - TANK_WIDTH_FP;
	int py = yc - TANK_HEIGHT_FP;
	landscape->correct_coord(px,py);

	area perimeter = area(px,py,STRUCTURE_WIDTH+2*TANK_WIDTH_FP,STRUCTURE_HEIGHT+2*TANK_HEIGHT_FP);

	/* vykopani hliny */
	landscape->set_area(perimeter,PATH);

	/* stavba zdi */
	size_t i;
	size_t j;
	size_t pos = 0;

	for(j = 0; j < STRUCTURE_HEIGHT; ++j)
	{
		for(i = 0; i < STRUCTURE_WIDTH; ++i)
		{
			if(bitmap[pos] == '1')
				landscape->set_xy(ground.x+i,ground.y+j,STRUCTURE);
			++pos;
		}
	}
	
	zone.x = ground.x + WALL_WIDTH;
	zone.y = ground.y + WALL_WIDTH;
	landscape->correct_coord(zone.x,zone.y);
	zone.w = STRUCTURE_WIDTH - 2*WALL_WIDTH;
	zone.h = STRUCTURE_HEIGHT- 2*WALL_WIDTH;

	picture = new structure_sprite(STRUCTURE_WIDTH*TILE_SIZE,STRUCTURE_HEIGHT*TILE_SIZE,id-MAX_PLAYER_CNT);
}

void structure::show(camera * cam)
{
	cam->render(picture->structure_img,ground);
}

area * structure::get_area()
{
	return (&zone);
}
