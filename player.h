#ifndef _PLAYER_H
#define _PLAYER_H

#include "tank.h"
#include "structure.h"
#include "commondef.h"

extern char player_cnt;
extern tank_map * tanks;

class camera;

struct controls
{
	SDLKey up;
	SDLKey down;
	SDLKey left;
	SDLKey right;
	SDLKey fire;
};

class player
{
	public:
	player(int socket,char init_id,int init_life,struct controls & settings);
	void confirm_ok();			/* posle na server potvrzeni WAITING_FOR_START */
	bool wait_for_start();			/* ceka na signal ke startu vraci true pri jeho obdrzeni */
	SDLKey process_keypress(SDL_KeyboardEvent & ke, bool press); /* zmeni stav sveho tanku podle stisknute klavesy */
	void send_msg();			/* odeslani stavu vlastniho tanku na server */
	bool kill();				/* ukonceni hry uzivatelem */
	unsigned char * recv_answer(unsigned char * ret);		/* prijmuti odpovedi od serveru */
	void reset_tank_state();		/* vyresetuje stav tanku po resurekci */
	char id;				/* id vlastniho tanku */

	friend class game;
	friend int show_lives(camera*,player*,int);

	private:
	unsigned char tank_state;		/* stav tanku zakodovany do unsigned charu 
				 		 * obsahuje 0RAFDDDD
				 		 * kde: R je bit urcujici zda je treba tank resetovat (a ubrat zivot playera)
						 * 	A je bit urcujici zda je tank zivy (alive bit)
				 		 * 	F je bit bit indikujici strelbu daneho tanku (fire bit)
				 		 * 	DDDD jsou bity urcujici smer kterym se tank pohybuje */
	
	bool is_bit_set(char mask);		/* zjisti zda je bit specifikovany maskou nastaven 
						 * (maska musi obsahovat pouze jeden bit) */
	
	int life;				/* pocet zivotu hrace */
	int sock;				/* socket pres ktery probiha komunikace se serverem */
	controls user_input;			/* definice ovladani */
};

#endif
