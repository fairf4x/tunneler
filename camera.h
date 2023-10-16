#ifndef _CAMERA_H
#define _CAMERA_H


#define	TILE_SIZE	5	/* velikost "dlazdicek" mapy */

#include "tmap.h"
#include "SDL/SDL.h"
#include "collisions.h" 

/* aktualni herni mapa */
extern tmap * landscape;

class tank;
class structure;
class game;
class player;

class camera
{
	public:
	camera(SDL_Surface * can,SDL_Rect * win);
	~camera();
	void set(coord position);	/* zameri stred kamery na dane souradnice */
	void shot(void);		/* "vyfoti" mapu na aktualnich souradnicich */
	int target_x(void);		/* vrati x-ovou souradnici dlazdicky uprostred zaberu */
	int target_y(void);		/* vrati y-ovou souradnici dlazdicky uprostred zaberu */
	bool is_on_screen(int inx,int iny);	/* zjisti zda je bod na danych souradnicich v zaberu */
	
	/* dosadi do "spot" souradnice bodu (mx,my) vzhledem k obrazovce a vrati 0 
	 * pokud bod se zadanymi souradnicemi neni v zaberu kamery vrati -1 */
	int get_screen_coords(int mx, int my, SDL_Rect & spot);
	
	area visible_area;	/* prostor ktery kamera zabira na mape */
	SDL_Rect * get_window_rect();	/* vrati obdelnik pro vykreslovani */

	friend int show_status_bar(camera*,tank*);
	friend int show_lives(camera*,player*,int);
	friend int show_final_screen(camera*);
	friend class tank;
	friend class structure;
	friend class game;
	/* pro ladici ucely */
	void print_coords(void);
	private:
	SDL_Surface * canvas;	/* kam se ma vykreslit "obrazek" ktery kamera sejme */
	SDL_Rect * window;	/* ctverec do ktereho se ma kreslit */
	
};

#endif
