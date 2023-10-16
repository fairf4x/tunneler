#include "tank.h"
#include "structure.h"
#include "commondef.h"

extern tank_map * tanks;
extern structure_map * structures;

tank::tank(unsigned char init_id)
{
	id = init_id;
	energy = MAX_ENERGY;
	armor = MAX_ARMOR;
	prevX = 0;
	prevY = 0;
	shape = new collision_shape(0,0,TANK_WIDTH,TANK_HEIGHT,TANK_SHAPE);
	sprites = new tank_sprites(TANK_WIDTH*TILE_SIZE,TANK_HEIGHT*TILE_SIZE,id);
	facing = UP;
}

void tank::set_position(int x,int y)
{
	/* opraveni souradnic vzhledem k mape */
	landscape->correct_coord(x,y);
	/* nastaveni predchozich souradnic */
	prevX = x;
	prevY = y;
	/* nastaveni aktualnich souradnic */
	shape->x = x;
	shape->y = y;

	/* vykopani mapy */
	landscape->set_tiles(x,y,(get_shape())->shape,PATH);
	return;
}

bool tank::move(char dir)
{
	int dig_tiles_cnt = 0;	/* pocet vykopanych dlazdic */

	if(dir == NOWHERE)
		return (false);

	switch(dir)
	{
		case RIGHT:
			shape->x += STEP;
			facing = RIGHT;
		break;
		case LEFT:
			shape->x -= STEP;
			facing = LEFT;
		break;
		case UP:
			shape->y -= STEP;
			facing = UP;
		break;
		case DOWN:
			shape->y += STEP;
			facing = DOWN;
		break;
		case UP_RIGHT:
			shape->x += STEP;
			shape->y -= STEP;
			facing = UP_RIGHT;
		break;
		case UP_LEFT:
			shape->x -= STEP;
			shape->y -= STEP;
			facing = UP_LEFT;
		break;
		case DOWN_RIGHT:
			shape->x += STEP;
			shape->y += STEP;
			facing = DOWN_RIGHT;
		break;
		case DOWN_LEFT:
			shape->x -= STEP;
			shape->y += STEP;
			facing = DOWN_LEFT;
		break;
		default:break;
	}

	landscape->correct_coord(shape->x,shape->y);

	/* detekce kolize s mapou */
	if(shape->map_collision(ROCK) || shape->map_collision(STRUCTURE))
	{
		/* doslo ke kolizi - pohyb se nekona */
		/* nastaveni puvodnich souradnic */
		shape->x = prevX;
		shape->y = prevY;
		return (false);
	}
	
	/* detekce kolize s ostatnimi objekty */
	
	tank_map_it tmit;
	for(tmit = tanks->begin(); tmit != tanks->end(); ++tmit)
	{
		/* jestlize se nejedna o tento tank */
		if( &((*tmit).second) != this )
		{
			/* overuje se hruba kolize */
			if( (*shape) % (*(*tmit).second.get_shape()) )	
			{
				/* pokud k hrube kolizi doslo overuje se jemna kolize */
				if( (*shape) * (*(*tmit).second.get_shape()) )	
				{
					/* doslo ke kolizi - pohyb se nekona */
					/* nastaveni puvodnich souradnic */
					shape->x = prevX;
					shape->y = prevY;
					return (false);
				}

			}
		}
	}
	
	/* kolize neprobehla muzeme zustat posunuti */
	prevX = shape->x;
	prevY = shape->y;

	/* odecteni energie za pohyb */
	energy -= MOVE_ENERGY;

	/* vykopani mapy na nove pozici */
	dig_tiles_cnt = landscape->set_tiles(prevX,prevY,(get_shape())->shape,PATH);
	
	/* odecteni energie za kopani hliny */
	energy -= dig_tiles_cnt * TILE_DIG_ENERGY;

	return (true);
}

bool tank::show(camera * cam)
{
	SDL_Rect corner_spot;
	size_t xd;
	size_t yd;
	int cam_x;
	int cam_y;

	/* pokud tank "koliduje" se zaberem kamery */
	if( (*shape) % (cam->visible_area) )
	{	
		/* vypocet souradnic obrazku na screen pro SDL_BlitSurface */
		cam_x = cam->visible_area.x;
		cam_y = cam->visible_area.y;
		xd = AbsClockwise(shape->x,cam_x,HORIZONTAL,landscape);
		yd = AbsClockwise(shape->y,cam_y,VERTICAL,landscape);
		
		switch(decide_location(xd,yd,TANK_WIDTH,TANK_HEIGHT))
		{
			case BOTTOM_RIGHT: /* pripad "X" */
				cam->get_screen_coords(shape->x,shape->y,corner_spot);
				sprites->rect.x = corner_spot.x;
				sprites->rect.y = corner_spot.y;
			break;
			case BOTTOM_LEFT: /* pripad "|" */
				sprites->rect.x = cam->window->x - (TILE_SIZE * xd);
				sprites->rect.y = cam->window->y + TILE_SIZE*AbsClockwise(cam_y,shape->y,VERTICAL,landscape);
			break;
			case TOP_RIGHT: /* pripad "-" */
				sprites->rect.x = cam->window->x + TILE_SIZE*AbsClockwise(cam_x,shape->x,HORIZONTAL,landscape);
				sprites->rect.y = cam->window->y - (TILE_SIZE * yd);
			break;
			case TOP_LEFT: /* pripad "#" */
				sprites->rect.x = cam->window->x - (TILE_SIZE * xd);
				sprites->rect.y = cam->window->y - (TILE_SIZE * yd);
			break;
			default:
				fprintf(stderr,"tank::show decide_location ERROR\n");
		}

		SDL_SetClipRect(cam->canvas,cam->window);
		switch(facing)
		{
			case UP:
				SDL_SetColorKey(sprites->tank_up,SDL_SRCCOLORKEY,sprites->color_key);
				SDL_BlitSurface(sprites->tank_up,NULL,cam->canvas,&(sprites->rect));
			break;
			case DOWN:
				SDL_SetColorKey(sprites->tank_down,SDL_SRCCOLORKEY,sprites->color_key);
				SDL_BlitSurface(sprites->tank_down,NULL,cam->canvas,&(sprites->rect));
			break;
			case LEFT:
				SDL_SetColorKey(sprites->tank_left,SDL_SRCCOLORKEY,sprites->color_key);
				SDL_BlitSurface(sprites->tank_left,NULL,cam->canvas,&(sprites->rect));
			break;
			case RIGHT:
				SDL_SetColorKey(sprites->tank_right,SDL_SRCCOLORKEY,sprites->color_key);
				SDL_BlitSurface(sprites->tank_right,NULL,cam->canvas,&(sprites->rect));
			break;
			case UP_LEFT:
				SDL_SetColorKey(sprites->tank_up_left,SDL_SRCCOLORKEY,sprites->color_key);
				SDL_BlitSurface(sprites->tank_up_left,NULL,cam->canvas,&(sprites->rect));
			break;
			case UP_RIGHT:
				SDL_SetColorKey(sprites->tank_up_right,SDL_SRCCOLORKEY,sprites->color_key);
				SDL_BlitSurface(sprites->tank_up_right,NULL,cam->canvas,&(sprites->rect));
			break;
			case DOWN_LEFT:
				SDL_SetColorKey(sprites->tank_down_left,SDL_SRCCOLORKEY,sprites->color_key);
				SDL_BlitSurface(sprites->tank_down_left,NULL,cam->canvas,&(sprites->rect));
			break;
			case DOWN_RIGHT:
				SDL_SetColorKey(sprites->tank_down_right,SDL_SRCCOLORKEY,sprites->color_key);
				SDL_BlitSurface(sprites->tank_down_right,NULL,cam->canvas,&(sprites->rect));
			break;
			default:
			break;
		}

		SDL_SetClipRect(cam->canvas,NULL);
	
		return (true);
	}
	else
		return (false);

}

int tank::get_hit(int damage)
{
	armor -= damage;
	return (armor);
}

void tank::regenerate(char bunkr_id)
{
	if( bunkr_id == (id+MAX_PLAYER_CNT))
	{
		energy += ENERGY_RECOVERY_HOME;
		armor += ARMOR_RECOVERY;
		
		if(armor > MAX_ARMOR)
			armor = MAX_ARMOR;
	}
	else
		energy += ENERGY_RECOVERY_ENEMY;

	if(energy > MAX_ENERGY)
		energy = MAX_ENERGY;
	return;
}

void tank::fire()
{
	energy -= FIRE_ENERGY;
}

void tank::reset()
{
	/* return to base & resurection */
	structure_map_it smit = structures->find(MAX_PLAYER_CNT+id);
	area * home_area = smit->second.get_area();

	set_position(home_area->x + TANK_REL_X,home_area->y + TANK_REL_Y);

	energy = MAX_ENERGY;
	armor = MAX_ARMOR;
	prevX = 0;
	prevY = 0;
}

coord tank::get_position()
{
	coord res(shape->x,shape->y);
	return (res);
}

/* X   X   X	-> +x
 *  #000000	|
 *  0000000	V +y
 *  0000000
 * X0000000X
 *  0000000
 *  0000000
 *  0000000
 * X   X   X */
coord tank::canon_position()
{
	switch(facing)
	{
		case UP:
			return coord(prevX+TANK_SIZE/2,prevY-1);		
		break;
		case DOWN:
			return coord(prevX+TANK_SIZE/2,prevY+TANK_SIZE);		
		break;
		case LEFT:
			return coord(prevX-1,prevY+TANK_SIZE/2);		
		break;
		case RIGHT:
			return coord(prevX+TANK_SIZE,prevY+TANK_SIZE/2);		
		break;
		case UP_LEFT:
			return coord(prevX-1,prevY-1);		
		break;
		case UP_RIGHT:
			return coord(prevX+TANK_SIZE,prevY-1);		
		break;
		case DOWN_LEFT:
			return coord(prevX-1,prevY+TANK_SIZE);		
		break;
		case DOWN_RIGHT:
			return coord(prevX+TANK_SIZE,prevY+TANK_SIZE);		
		break;
		default:
			fprintf(stderr,"tank::canon_position : wrong direction\n");
	}
	/* k tomuhle NIKDY nedojde */
	return coord(0,0);
}

collision_shape * tank::get_shape()
{
	return (shape);
}

char tank::dir()
{
	return (facing);
}

unsigned char tank::get_id()
{
	return (id);
}

bool need_resurection(unsigned char bitmask)
{
	return (bitmask >> 6);
}

bool is_alive(unsigned char bitmask)
{
	return ((bitmask >> 5) % 2);
}

bool is_firing(unsigned char bitmask)
{
	return ((bitmask >> 4) % 2);
}

unsigned char is_heading(unsigned char bitmask)
{
	/* pouze posledni 4 bity */
	return (bitmask % 16);
}


