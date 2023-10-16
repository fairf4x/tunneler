#ifndef _BULLET_H
#define _BULLET_H

#define RANGE 40
#define DAMAGE 20

#define BULLET_ID 16	/* 0-7 vyhrazeno pro tanky 8-15 pro bunkry */
#define MAP_ID 17	/* navratova hodnota pro pripad zasahu mapy */

#define EXPLODE 0
#define NO_EXPOLODE -1

/* strely ktere se pohybuji uhlopricne jsou na pohled "rychlejsi" nez strely ve 
 * vodorovnem nebo svislem smeru (diagonala ctverce je delsi nez jeho strana)
 * diagonalni strely je tedy treba "zpomalit"
 * konkretne: BULLET_STEP_D = ceil(sqrt(0.5)*BULLET_STEP_HV)
 ***********************************************************************************/
#define BULLET_STEP_HV 5	/* pohyb strely v horizontalnim a vertikalnim smeru */
#define BULLET_STEP_D 4		/* pohyb strely v diagonalnim smeru */

#define FIRE_DIG_SIZE 5				/* ctverec 5x5 udava tvar vybuchu */
#define FIRE_DIG "0010001110111110111000100"

#include "tank.h"
/* v ramci tank.h provedeno take:	*/
/*	#include "collisions.h"
	#include "camera.h"
	#include "tmap.h"		*/

extern coord_set * dig_hole;		/* promenna uchovavajici tvar vybuchu */

extern tmap * landscape;
extern SDL_Surface * screen;
extern tank_map * tanks;

class bullet
{
	public:
		bullet(coord where, char dir);
		void explode();		/* exploduje na aktualnich souradnicich */
		void disapear();	/* ukonci existenci strely bez vybuchu */
		int get_ttl();		/* kolik cyklu ma jeste strela existovat */
		bool move();		/* posune strelu o jeden ctverecek 
					 *(behem jednoho cyklu to lze provest pouze to_move krat) 
					 * vraci false pokud uz nemuze posunovat */
		void show(camera * cam);
		unsigned char which_tank();	/* zjisti, ktery tank je zasazen (momentalne koliduje se strelou) */
		coord get_pos();		/* vrati souradnice strely */
		
	private:
	coord pos;			/* souradnice strely */
	int ttl;			/* time to live - kolik cyklu jeste zbyva */
	int to_move;			/* o kolik se jeste musi pohnout v tomto kole */
	char direction;
};

void init_dig_hole();		/* inicializace tvaru vybuchu */

using namespace std;
typedef list <class bullet> bullet_list;
typedef bullet_list::iterator bullet_list_iter;

#endif
