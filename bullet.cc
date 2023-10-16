#include "bullet.h"

extern Uint32 fire_color;

bullet::bullet(coord where, char dir)
{
	direction = dir;
	ttl = RANGE;
	to_move = 0;	/* v cyklu kdy vznikne se strela jeste nepohybuje */
	pos.x = where.x;
	pos.y = where.y;
}

void bullet::explode()
{
	landscape->change_tiles(pos.x-2,pos.y-2,(*dig_hole),MUD,PATH);
	ttl = 0;
	to_move = 0;
}

void bullet::disapear()
{
	ttl = 0;	/* zajisti vymazani strely ze seznamu hned po vykresleni */
	to_move = 0;
}

int bullet::get_ttl()
{
	return (ttl);
}

bool bullet::move()
{
	if(to_move > 0)
	{
		switch(direction)
		{
			case UP:
				pos.y -= 1;
			break;
			case DOWN:
				pos.y += 1;
			break;
			case LEFT:
				pos.x -= 1;
			break;
			case RIGHT:
				pos.x += 1;
			break;
			case UP_LEFT:
				pos.x -= 1;
				pos.y -= 1;
			break;
			case UP_RIGHT:
				pos.x += 1;
				pos.y -= 1;
			break;
			case DOWN_LEFT:
				pos.x -= 1;
				pos.y += 1;
			break;
			case DOWN_RIGHT:
				pos.x += 1;
				pos.y += 1;
			break;
			default:
			break;
		}

		--to_move;

		return (true);
	}
	else
	{
		switch(direction)
		{
			case UP:
			case DOWN:
			case LEFT:
			case RIGHT:
				to_move = BULLET_STEP_HV;
			break;
			default:
				to_move = BULLET_STEP_D;
			break;
		}

		--ttl;
		landscape->correct_coord(pos.x,pos.y);
		return (false);
	}
}

void bullet::show(camera * cam)
{
	if( cam->visible_area % pos )
	{
		SDL_Rect ctv;
		if(cam->get_screen_coords((int)pos.x,(int)pos.y,ctv) == 0)
			SDL_FillRect(screen,&ctv,fire_color);
	}
	return;
}

unsigned char bullet::which_tank()
{
	tank_map_it tmit;
	collision_shape * tank_shape;

	for(tmit = tanks->begin(); tmit != tanks->end(); ++tmit)
	{
		tank_shape = (*tmit).second.get_shape();
		if( (*tank_shape) % pos )
		{
			if( (*tank_shape) * pos )
				return ((*tmit).second.get_id());
		}
	}

	/* tohle by se nemelo stavat */
	return BULLET_ID;
}

coord bullet::get_pos()
{
	return coord(pos);
}

void init_dig_hole()
{
	dig_hole = new coord_set;
	const char sh[] = FIRE_DIG;
	size_t i;
	size_t j;
	size_t pos = 0;
	for(i=0; i < FIRE_DIG_SIZE; ++i)
	{
		for(j=0; j < FIRE_DIG_SIZE; ++j)
		{
			if(sh[pos] == '1')
				dig_hole->insert(coord(i,j));
			++pos;
		}
	}
}
