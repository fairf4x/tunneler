#include "structure.h"
#include "commondef.h"	/* kvuli konstante MAX_PLAYER_CNT */

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
	SDL_Rect corner_spot;
	size_t xd;
	size_t yd;
	int cam_x;
	int cam_y;
	
	/* pokud area koliduje se zaberem kamery */
	if( ground % cam->visible_area)
	{
		/* vypocet souradnic obrazku na screen pro SDL_BlitSurface */
		cam_x = cam->visible_area.x;
		cam_y = cam->visible_area.y;
		xd = AbsClockwise(ground.x,cam_x,HORIZONTAL,landscape);
		yd = AbsClockwise(ground.y,cam_y,VERTICAL,landscape);
		
		switch(decide_location(xd,yd,STRUCTURE_WIDTH,STRUCTURE_HEIGHT))
		{
			case BOTTOM_RIGHT: /* pripad "X" */
				cam->get_screen_coords(ground.x,ground.y,corner_spot);
				picture->rect.x = corner_spot.x;
				picture->rect.y = corner_spot.y;
			break;
			case BOTTOM_LEFT: /* pripad "|" */
				picture->rect.x = cam->window->x - (TILE_SIZE * xd);
				picture->rect.y = cam->window->y + TILE_SIZE*AbsClockwise(cam_y,ground.y,VERTICAL,landscape);
			break;
			case TOP_RIGHT: /* pripad "-" */
				picture->rect.x = cam->window->x + TILE_SIZE*AbsClockwise(cam_x,ground.x,HORIZONTAL,landscape);
				picture->rect.y = cam->window->y - (TILE_SIZE * yd);
			break;
			case TOP_LEFT: /* pripad "#" */
				picture->rect.x = cam->window->x - (TILE_SIZE * xd);
				picture->rect.y = cam->window->y - (TILE_SIZE * yd);
			break;
			default:
				fprintf(stderr,"tank::show decide_location ERROR\n");
		}

		SDL_SetClipRect(cam->canvas,cam->window);
		SDL_BlitSurface(picture->structure_img,NULL,cam->canvas,&(picture->rect));
		SDL_SetClipRect(cam->canvas,NULL);
	}
	else
		return (false);
}

area * structure::get_area()
{
	return (&zone);
}
