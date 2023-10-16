#ifndef _STATUSBAR_H
#define _STATUSBAR_H

#include "SDL/SDL.h"
#include "camera.h"
#include "tank.h"
#include "player.h"

int init_status_bar(Uint32 width);

int show_status_bar(camera * cam, tank * machine);

void destroy_status_bar();

int init_lives(SDL_Surface * screen);

int show_lives(camera * cam, player * pla, int max_lives);

int init_final_screen(SDL_Surface * screen);

int show_final_screen(camera * cam);

#endif
