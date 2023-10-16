#ifndef _GAME_H
#define _GAME_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "player.h"
/* v ramci bullet.h provedeno take:	*/
/*	#include "tmap.h"
	#include "camera.h"
	#include "SDL/SDL.h"
	#include "SDL/SDL_gfxPrimitives.h"
	#include "collisions.h"		*/
#include "bullet.h"
#include "commondef.h"

/* globalni promenne */
extern SDL_Surface * screen;
extern tmap * landscape;

extern tank_map	* tanks;		/* mapa tanku - klicem je id */ 
extern structure_map * structures;	/* mapa struktur - klicem je id */
extern char player_cnt;			/* pocet hracu ziskava se ze serveru */

class game
{
	public:
	game(bool split, int map_input);
	~game();
	void execute();				/* vykona povely od serveru */
	void start(int sock);			/* zahaji hru (spusti casovac, zapne zpracovani udalosti 
			 			 * vstoupi do herni smycky) */
	void start(int sock1, int sock2);
	
	void DrawScreen_split();
	void DrawScreen();
	private:
	bool ProcessEvents();
	void process_timer_tick();
	void move_fire();
	char get_char_answer(int sock,char request);
	unsigned char * get_answer();

	coord_set * make_pool();

	void create_objects(int sock); /* 	vytvori tanky a bunkry
				        *	inicializuje obstacles, tanks a bases */

	void recover_tanks();		/* uzdravi tanky ktere jsou v bunkru */
	void show_status(camera * cam, tank & machine);

	bullet_list fire;	/* seznam vypalenych strel */

	bool splitscreen;

	camera * cam1;		/* vyuzije se vzdy */
	camera * cam2;		/* vyuzije se pro splitscreen */

	player * pla1;
	player * pla2; 

	SDL_TimerID timer;	/* herni timer */
	Uint32 delay;		/* interval herniho timeru */
};

Uint32 callback(Uint32 interval, void * param);	/* callback pro herni timer */

#endif
