#ifndef _TANK_H
#define _TANK_H

#include <map>
#include "camera.h"
#include "painting.h"

/* v ramci camera.h: 		*/
/*	#include "collisions.h"
	#include "tmap.h" 	*/

#define MAX_ENERGY 5000
#define MAX_ARMOR 300

#define ENERGY_RECOVERY_HOME 40
#define ENERGY_RECOVERY_ENEMY 15
#define ARMOR_RECOVERY 5

#define TILE_DIG_ENERGY 4
#define MOVE_ENERGY 2
#define FIRE_ENERGY 10

#define NOWHERE		0x0	/* 0000 0000 */
#define UP		0x1	/* 0000 0001 */
#define DOWN		0x2	/* 0000 0010 */
#define LEFT		0x4	/* 0000 0100 */
#define RIGHT		0x8	/* 0000 1000 */

#define UP_LEFT		0x5	/* 0000 0101 */
#define UP_RIGHT	0x9	/* 0000 1001 */
#define DOWN_LEFT	0x6	/* 0000 0110 */
#define DOWN_RIGHT	0xA	/* 0000 1010 */


#define STEP 1			/* o kolik se ma tank posunout */

#define TANK_WIDTH 7
#define TANK_HEIGHT 7
#define TANK_SIZE 7
			/* 0000000
			 * 0011100
			 * 0110110
			 * 0100010
			 * 0110110
			 * 0011100
			 * 0000000 */

#define TANK_SHAPE "0000000001110001101100100010011011000111000000000"

extern tmap * landscape;
extern SDL_Surface * screen;


class tank
{
	public:
	tank(unsigned char init_id);
	void set_position(int x,int y);
	bool move(char dir);	/* pohne tankem v zadanem smeru */
	bool show(camera * cam);/* ukaze tank v okne dane kamery */
	int get_hit(int damage);		/* odecte armor pri zasahu */
	void regenerate(char bunkr_id);	/* obnoveni energy a armor - doma rychleji nez jinde */
	void fire();		/* odecte energii za odpaleni strely */
	void reset();		/* vrati tank na misto kde zacinal (souradnice zjisti z "domaciho" bunkru) */
	coord get_position();
	coord canon_position();	/* souradnice na kterych by se mela ojevit vypalena strela */
	collision_shape * get_shape();
	char dir();
	unsigned char get_id();

	int armor;		/* pancir tanku */
	int energy;		/* energie tanku */

	/* kvuli ladicim vypisum */
	friend class game;

	private:
	char facing;		/* smer kterym je tank zrovna natocen */
	tank_sprites * sprites;	/* obrazky pro jednotliva natoceni */

	unsigned char id;

	int prevX;			/* predchozi souradnice */
	int prevY;
	collision_shape * shape;	/* prostor, ktery zabira na mape */
};

using namespace std;
typedef map <char,tank> tank_map;
typedef tank_map::iterator tank_map_it;

extern tank_map * tanks;

bool need_resurection(unsigned char bitmask);
bool is_alive(unsigned char bitmask);
bool is_firing(unsigned char bitmask);
unsigned char is_heading(unsigned char bitmask);

#endif
