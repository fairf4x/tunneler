#include "structure.h"
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

	/* vykopani hliny */
	landscape->set_area(ground,PATH);

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

bool structure::show(camera * cam)
{
	// vyrez mapy ktery je videt urcuje area cam->visible_area
	area cutA;
	SDL_Rect structArea;
	SDL_Rect imgCut;
	imgCut.x = 0;
	imgCut.y = 0;
	
	size_t xd = 0;
	size_t yd = 0;

	bool visible = intersection(cam->visible_area,ground,cutA);
	if( visible )
	{
		cam->get_screen_coords(cutA.x,cutA.y,structArea);
		SDL_SetRenderDrawColor(renderer,fire_color.r,fire_color.g,fire_color.b,60);
		
		cam->print_coords();
		printf("cutA: %d,%d,%lu,%lu\n",cutA.x,cutA.y,cutA.w,cutA.h);

		if (cutA.x != ground.x)
			xd = DistMod(cutA.x,ground.x,HORIZONTAL,landscape);

		if (cutA.y != ground.y)
			yd = DistMod(cutA.y,ground.y,VERTICAL,landscape);

		printf("dist: %lu,%lu\n",xd,yd);

		imgCut.x = (int)xd * TILE_SIZE;
		imgCut.y = (int)yd * TILE_SIZE;
		imgCut.w = cutA.w * TILE_SIZE;
		imgCut.h = cutA.h * TILE_SIZE;
		
		printf("imgC: %d,%d,%d,%d\n",imgCut.x,imgCut.y,imgCut.w,imgCut.h);

		structArea.w = cutA.w * TILE_SIZE;
		structArea.h = cutA.h * TILE_SIZE;

		SDL_RenderCopy(renderer,picture->structure_img,&imgCut,&structArea);
	}

	return true;
}

area * structure::get_area()
{
	return (&zone);
}
