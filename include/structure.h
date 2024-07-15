#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#define STRUCTURE_WIDTH 21
#define STRUCTURE_HEIGHT 21
#define TANK_REL_X 4
#define TANK_REL_Y 4

#define STRUCTURE_SHAPE "111111100000001111111111111100000001111111110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011110000000000000000011111111100000001111111111111100000001111111"

#define WALL_WIDTH 2	/* tloustka zdi - je treba nadefinovat podle STRUCTURE_SHAPE */

#include "camera.h"
#include "painting.h"
/* v ramci camera.h provedeno take: 	*/
/*	#include "tmap.h"
	#include "collisions.h"		*/
#include <map>

extern tmap * landscape;
extern SDL_Surface * screen;

class structure
{
	public:
	structure(char init_id, int xc, int yc, const char * bitmap);
	bool show(camera * cam);
	area * get_area();
	char id;

	private:
	structure_sprite * picture;	
	area ground;			/* vymezeni struktury pro potreby vykreslovani */
	area zone;			/* vymezeni pro potreby dobijeni a opravy tanku */
};

using namespace std;
typedef map <char,structure> structure_map;
typedef structure_map::iterator structure_map_it;

#endif
