#include "tank.h"
#include "structure.h"
#include "commondef.h"


const char* TANK_SHAPE = "0000000001110001101100100010011011000111000000000";

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
	sprites = new tank_sprites(renderer,TANK_WIDTH*TILE_SIZE,TANK_HEIGHT*TILE_SIZE,id);
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

void tank::show(camera * cam)
{
	area spriteArea;

	// TODO: spocitat flek zabrany na mape podle pozice tanku (je to ctverec 7x7)
	coord pos = get_position();

	spriteArea.x = pos.x;
	spriteArea.y = pos.y;
	spriteArea.w = TANK_WIDTH;
	spriteArea.h = TANK_HEIGHT;

	landscape->correct_coord(spriteArea.x,spriteArea.y);

	/* pokud tank "koliduje" se zaberem kamery */
	if( (*shape) % (cam->visible_area) )
	{	
		switch(facing)
		{
			case UP:
				cam->render(sprites->tank_up,spriteArea);
			break;
			case DOWN:
				cam->render(sprites->tank_down,spriteArea);
			break;
			case LEFT:
				cam->render(sprites->tank_left,spriteArea);
			break;
			case RIGHT:
				cam->render(sprites->tank_right,spriteArea);
			break;
			case UP_LEFT:
				cam->render(sprites->tank_up_left,spriteArea);
			break;
			case UP_RIGHT:
				cam->render(sprites->tank_up_right,spriteArea);
			break;
			case DOWN_LEFT:
				cam->render(sprites->tank_down_left,spriteArea);
			break;
			case DOWN_RIGHT:
				cam->render(sprites->tank_down_right,spriteArea);
			break;
			default:
			break;
		}
	}
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
