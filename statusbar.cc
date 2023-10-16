#include "statusbar.h"
#include <stdio.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
Uint32  rmask = 0xff000000;
Uint32  gmask = 0x00ff0000;
Uint32  bmask = 0x0000ff00;
Uint32  amask = 0x000000ff;
#else
Uint32  rmask = 0x000000ff;
Uint32  gmask = 0x0000ff00;
Uint32  bmask = 0x00ff0000;
Uint32  amask = 0xff000000;
#endif

#define GAME_OVER_PIC "./img/game_over.bmp"

#define ALPHA_BAR 128
#define ALPHA_FRA 75

#define SB_MARGIN 10	/* mezera mezi okrajem kamery a status barem */
#define SB_PADDING 10	/* mezera mezi okrajem status baru a pruhem */
#define SB_SPACE 10	/* mezera mezi statusem armoru a energy */
#define SB_HEIGHT 50	/* vyska statusbaru */
#define SB_STRIPE_H 10	/* vyska status pruhu */

#define LIVES_MARGIN 10
#define LIVES_SPACE 5
#define LIVES_HEIGHT 20
#define LIVES_WIDTH 5

/* konstanty ovlivnujici rychlost blikani napisu GAME OVER */
#define BLINK_MODULO 8
#define BLINK_VAL 4
/* promenna zajistujici blikani napisu GAME OVER */
int blink_index;

/* surface ne kterem se bude sestavovat status bar */
SDL_Surface * status_bar;

/* barva ramecku status baru */
Uint32 status_color_key;

/* obrazek napisu GAME OVER*/
SDL_Surface * final_pic;

/* barva ktera bude transparentni v GAME OVER obrazku */
Uint32 transp_col_final;

/* stav armoru a energie se zobrazuje bro ruzna mnozstvi ruznou barvou
 * 75%-100%	xxxx_1
 * 33%-75%	xxxx_2
 * 0%-33%	xxxx_3 
 * kde xxxx = armor/energy 
 * meze maji vystihovat ruzne kriticke stavy: vse OK - zatim to jde - kriticky stav */

/* hranice odlisnych mezi */
const float bottom1 = 0.75;
const float bottom2 = 0.33;

/* barvy pro ruzne meze hodnot */
Uint32 armor_1;
Uint32 armor_2;
Uint32 armor_3;

Uint32 energy_1;
Uint32 energy_2;
Uint32 energy_3;

Uint32 lives_col;

Uint32 max_width;

int init_status_bar(Uint32 width)
{
	status_bar = SDL_CreateRGBSurface(SDL_SWSURFACE, width, SB_HEIGHT, 32,
	                                   rmask, gmask, bmask, amask);
	if( status_bar == NULL)
	{
		fprintf(stderr,"init_status_bar : SDL_CreateRGBSurface failed");
		return (1);
	}

	/* definice barev */
	status_color_key = SDL_MapRGBA(status_bar->format,30,10,175,ALPHA_FRA);
	
	armor_1 = SDL_MapRGBA(status_bar->format,0,100,0,ALPHA_BAR);
	armor_2 = SDL_MapRGBA(status_bar->format,0,0,100,ALPHA_BAR);
	armor_3 = SDL_MapRGBA(status_bar->format,100,0,0,ALPHA_BAR);

	energy_1 = SDL_MapRGBA(status_bar->format,0,100,0,ALPHA_BAR);
	energy_2 = SDL_MapRGBA(status_bar->format,0,0,100,ALPHA_BAR);
	energy_3 = SDL_MapRGBA(status_bar->format,100,0,0,ALPHA_BAR);

	max_width = width - 2 * SB_MARGIN;

	return (0);
}

int show_status_bar(camera * cam,tank * machine)
{
	if(status_bar == NULL)
	{
		fprintf(stderr,"show_status_bar : status_bar == NULL");
		return (1);
	}

	/* vyplneni cele plochy cernou */
	SDL_FillRect(status_bar,NULL,status_color_key);

	SDL_Rect * cam_rect = cam->get_window_rect();
	SDL_Rect rect;

	/* armor */
	float armor = (float)machine->armor / MAX_ARMOR;
	rect.x = SB_PADDING;
	rect.y = SB_PADDING;
	rect.w = (max_width*machine->armor)/MAX_ARMOR;
	rect.h = SB_STRIPE_H;
	
	if( bottom1 < armor )
		SDL_FillRect(status_bar,&rect,armor_1);
	else
	{
		if( bottom2 < armor )
			SDL_FillRect(status_bar,&rect,armor_2);
		else
			SDL_FillRect(status_bar,&rect,armor_3);
	}

	/* energy */
	float energy = (float)machine->energy / MAX_ENERGY;
	rect.y = SB_PADDING + SB_STRIPE_H + SB_SPACE;
	rect.w = (max_width*machine->energy)/MAX_ENERGY;

	if( bottom1 < energy )
		SDL_FillRect(status_bar,&rect,energy_1);
	else
	{
		if( bottom2 < energy )
			SDL_FillRect(status_bar,&rect,energy_2);
		else
			SDL_FillRect(status_bar,&rect,energy_3);
	}

	rect.x = cam_rect->x + SB_MARGIN;
	rect.y = cam_rect->y + cam_rect->h - ( SB_MARGIN + SB_HEIGHT );
	rect.w = status_bar->w;
	rect.h = status_bar->h;

	SDL_BlitSurface(status_bar,NULL,cam->canvas,&rect);

	return (0);
}

void destroy_status_bar()
{
	SDL_FreeSurface(status_bar);
}

int init_lives(SDL_Surface * screen)
{
	lives_col = SDL_MapRGB(screen->format,0,0,255);	
	return (0);
}
int show_lives(camera * cam, player * pla, int max_lives)
{
	int i;
	SDL_Rect scratch_rect;
	scratch_rect.w = LIVES_WIDTH;
	scratch_rect.h = LIVES_HEIGHT;
	scratch_rect.y = cam->window->y + LIVES_MARGIN;
	scratch_rect.x = cam->window->x + LIVES_MARGIN;

	for(i=0; i < pla->life; ++i)
	{
		if(i > (max_lives-1))
			return (1);

		SDL_FillRect(cam->canvas,&scratch_rect,lives_col);
		scratch_rect.x += LIVES_WIDTH + LIVES_SPACE;
	}
	return (0);
}

int init_final_screen(SDL_Surface * screen)
{
	final_pic = SDL_LoadBMP(GAME_OVER_PIC);
	if( final_pic == NULL )
	{
		fprintf(stderr,"init_final_screen: can't load bitmap\n");
		exit(1);
	}

	transp_col_final = SDL_MapRGB(final_pic->format,255,255,255);

	blink_index = 0;

	return (0);
}

int show_final_screen(camera * cam)
{
	if( (blink_index % BLINK_MODULO) < BLINK_VAL )
	{
		SDL_Rect rect;
		rect.w = final_pic->w;
		rect.h = final_pic->h;

		rect.x = cam->window->x + (cam->window->w / 2) - (final_pic->w / 2); 
		rect.y = cam->window->y + (cam->window->h / 2) - (final_pic->h / 2); 
		
		SDL_SetColorKey(final_pic,SDL_SRCCOLORKEY,transp_col_final);
		
		SDL_BlitSurface(final_pic,NULL,cam->canvas,&rect);
	}
		
	++blink_index;

	return (0);
}
