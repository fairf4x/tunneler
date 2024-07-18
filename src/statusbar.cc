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

#define GAME_OVER_PIC "./img/game_over.png"

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

/* barva ramecku status baru */
Color status_color_key = {.r=30, .g=10, .b=175, .a=ALPHA_FRA};

/* obrazek napisu GAME OVER*/
SDL_Texture * final_pic;
int final_pic_w;
int final_pic_h;

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
Color armor_1 = {.r=0, .g=100, .b=0, .a=ALPHA_BAR};
Color armor_2 = {.r=0, .g=0, .b=100, .a=ALPHA_BAR};
Color armor_3 = {.r=100, .g=0, .b=0, .a=ALPHA_BAR};

Color energy_1 = {.r=0, .g=100, .b=0, .a=ALPHA_BAR};
Color energy_2 = {.r=0, .g=0, .b=100, .a=ALPHA_BAR};
Color energy_3 = {.r=100, .g=0, .b=0, .a=ALPHA_BAR};

Color lives_col = {.r=0, .g=0, .b=255, .a=ALPHA_BAR};

Uint32 max_width;

int init_status_bar(Uint32 width)
{
	max_width = width - 2 * SB_MARGIN;

	return (0);
}

int show_status_bar(camera * cam,tank * machine)
{
	SDL_Rect * cam_rect = cam->get_window_rect();
	SDL_Rect rect;

	rect.x = cam_rect->x + SB_MARGIN;
	rect.y = cam_rect->y + cam_rect->h - ( SB_MARGIN + SB_HEIGHT );
	rect.w = cam_rect->w - 2*SB_MARGIN;
	rect.h = SB_HEIGHT;

	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
	/* vyplneni cele plochy vychozi barvou */
	SDL_SetRenderDrawColor(renderer, status_color_key.r, status_color_key.g, status_color_key.b, status_color_key.a);
	SDL_RenderFillRect(renderer,&rect);

	/* armor */
	float armor = (float)machine->armor / MAX_ARMOR;
	
	rect.x = rect.x + SB_PADDING;
	rect.y = rect.y + SB_PADDING;
	rect.w = (max_width*machine->armor)/MAX_ARMOR;
	rect.h = SB_STRIPE_H;
	
	if( bottom1 < armor ){
		SDL_SetRenderDrawColor(renderer, armor_1.r, armor_1.g, armor_1.b, armor_1.a);
		SDL_RenderFillRect(renderer,&rect);
	}
	else
	{
		if( bottom2 < armor ){
			SDL_SetRenderDrawColor(renderer, armor_2.r, armor_2.g, armor_2.b, armor_2.a);
			SDL_RenderFillRect(renderer,&rect);
		}
		else{
			SDL_SetRenderDrawColor(renderer, armor_3.r, armor_3.g, armor_3.b, armor_3.a);
			SDL_RenderFillRect(renderer,&rect);
		}
	}

	/* energy */
	float energy = (float)machine->energy / MAX_ENERGY;
	rect.y = rect.y + SB_STRIPE_H + SB_SPACE;
	rect.w = (max_width*machine->energy)/MAX_ENERGY;

	if( bottom1 < energy ){
		SDL_SetRenderDrawColor(renderer, energy_1.r, energy_1.g, energy_1.b, energy_1.a);
		SDL_RenderFillRect(renderer,&rect);
	}
	else
	{
		if( bottom2 < energy ){
			SDL_SetRenderDrawColor(renderer, energy_2.r, energy_2.g, energy_2.b, energy_2.a);
			SDL_RenderFillRect(renderer,&rect);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, energy_3.r, energy_3.g, energy_3.b, energy_3.a);
			SDL_RenderFillRect(renderer,&rect);
		}
	}

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

	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, lives_col.r, lives_col.g, lives_col.b, lives_col.a);

	for(i=0; i < pla->life; ++i)
	{
		if(i > (max_lives-1))
			return (1);

		SDL_RenderFillRect(renderer,&scratch_rect);
		scratch_rect.x += LIVES_WIDTH + LIVES_SPACE;
	}

	return (0);
}

int init_final_screen(SDL_Surface * screen)
{
	SDL_Surface * final_pic_img = IMG_Load(GAME_OVER_PIC);
	final_pic_w = final_pic_img->w;
	final_pic_h = final_pic_img->h;
	final_pic = SDL_CreateTextureFromSurface(renderer,final_pic_img);
	SDL_FreeSurface(final_pic_img);

	if( final_pic == NULL )
	{
		fprintf(stderr,"init_final_screen: can't load bitmap\n");
		exit(1);
	}

	blink_index = 0;

	return (0);
}

int show_final_screen(camera * cam)
{
	if( (blink_index % BLINK_MODULO) < BLINK_VAL )
	{
		SDL_Rect rect;
		rect.w = final_pic_w;
		rect.h = final_pic_h;

		rect.x = cam->window->x + (cam->window->w / 2) - (final_pic_w / 2); 
		rect.y = cam->window->y + (cam->window->h / 2) - (final_pic_h / 2); 
		
		SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
		SDL_RenderCopy(renderer,final_pic,NULL,&rect);
	}
		
	++blink_index;

	return (0);
}
