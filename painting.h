#ifndef _PAINTING_H
#define _PAINTING_H

#include "SDL/SDL.h"

/* definice navratovych hodnot pomocne funkce decide_location */
#define TOP_LEFT 	'#'
#define BOTTOM_LEFT 	'|'
#define TOP_RIGHT 	'-'
#define BOTTOM_RIGHT 	'X'
#define ERROR 		'E'

struct tank_sprites{
	tank_sprites(Uint32 pixw, Uint32 pixh,char id);
	~tank_sprites();
	Uint32 color_key;
	SDL_Rect rect;
	SDL_Surface * tank_up;
	SDL_Surface * tank_down;
	SDL_Surface * tank_left;
	SDL_Surface * tank_right;
	SDL_Surface * tank_up_left;
	SDL_Surface * tank_up_right;
	SDL_Surface * tank_down_left;
	SDL_Surface * tank_down_right;
};

struct structure_sprite{
	structure_sprite(Uint32 pixw, Uint32 pixh, char id);
	~structure_sprite();
	SDL_Rect rect;
	SDL_Surface * structure_img;
};

char decide_location( size_t x, size_t y, size_t w, size_t h);

#endif
