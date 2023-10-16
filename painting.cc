#include "painting.h"
#include <string.h>

#define SPRITE_UP 		"./img/tank_up.bmp"
#define SPRITE_DOWN 		"./img/tank_down.bmp"
#define SPRITE_LEFT 		"./img/tank_left.bmp"
#define SPRITE_RIGHT 		"./img/tank_right.bmp"
#define SPRITE_UP_LEFT 		"./img/tank_up_left.bmp"
#define SPRITE_UP_RIGHT 	"./img/tank_up_right.bmp"
#define SPRITE_DOWN_LEFT 	"./img/tank_down_left.bmp"
#define SPRITE_DOWN_RIGHT 	"./img/tank_down_right.bmp"

#define COLOR_DOT_PREFIX	"./img/dot"
#define COL_DOT_SLEN		15

#define FLOOR_FILE		"./img/structure_floor.bmp"
#define WALL_PREFIX		"./img/wall"
#define WALL_SLEN		16

/* pro zadane id vrati jmeno souboru s puntikem */
char * dot_file(char id)
{
	char * result = new char[COL_DOT_SLEN];
	char num[2];	/* ulozeni jednociferneho cisla */

	result[0] = '\0';
	/* zapis prefixu */
	result = strcat(result,COLOR_DOT_PREFIX);

	/* pripojeni id */
	num[0] = '0' + id;
	num[1] = '\0';

	result = strcat(result,num);

	/* pripojeni pripony */
	result = strcat(result,".bmp");
	return result;
}

tank_sprites::tank_sprites(Uint32 pixw, Uint32 pixh, char id)
{
	SDL_Surface * color_dot;	/* barevne rozliseni tanku */
	char * filename = dot_file(id);
	rect.x = 0;
	rect.y = 0;
	rect.w = pixw;
	rect.h = pixh;


	/* obarveni tanku */
	color_dot = SDL_LoadBMP(filename);
	
	if( color_dot == NULL )
	{
		fprintf(stderr,"tank_sprites: can't load dot bitmap");
		exit(1);
	}


	tank_up = SDL_LoadBMP(SPRITE_UP);

	/* bila je pruhledna */
	color_key = SDL_MapRGB(tank_up->format,255,255,255);
	SDL_SetColorKey(color_dot,SDL_SRCCOLORKEY,color_key);

	SDL_BlitSurface(color_dot,NULL,tank_up,NULL);
	tank_down = SDL_LoadBMP(SPRITE_DOWN);
	SDL_BlitSurface(color_dot,NULL,tank_down,NULL);
	tank_left = SDL_LoadBMP(SPRITE_LEFT);
	SDL_BlitSurface(color_dot,NULL,tank_left,NULL);
	tank_right = SDL_LoadBMP(SPRITE_RIGHT);
	SDL_BlitSurface(color_dot,NULL,tank_right,NULL);
	tank_up_left = SDL_LoadBMP(SPRITE_UP_LEFT);
	SDL_BlitSurface(color_dot,NULL,tank_up_left,NULL);
	tank_up_right = SDL_LoadBMP(SPRITE_UP_RIGHT);
	SDL_BlitSurface(color_dot,NULL,tank_up_right,NULL);
	tank_down_left = SDL_LoadBMP(SPRITE_DOWN_LEFT);
	SDL_BlitSurface(color_dot,NULL,tank_down_left,NULL);
	tank_down_right = SDL_LoadBMP(SPRITE_DOWN_RIGHT);
	SDL_BlitSurface(color_dot,NULL,tank_down_right,NULL);
}

tank_sprites::~tank_sprites()
{
	SDL_FreeSurface(tank_up);
	SDL_FreeSurface(tank_down);
	SDL_FreeSurface(tank_left);
	SDL_FreeSurface(tank_right);
	SDL_FreeSurface(tank_up_left);
	SDL_FreeSurface(tank_up_right);
	SDL_FreeSurface(tank_down_left);
	SDL_FreeSurface(tank_down_right);
}

/* pro zadane id vrati jmeno souboru se zdi */
char * wall_file(char id)
{
	char * result = new char[WALL_SLEN];
	char num[2];	/* ulozeni jednociferneho cisla */

	result[0] = '\0';
	
	/* zapis prefixu */
	result = strcat(result,WALL_PREFIX);

	/* pripojeni id */
	num[0] = '0' + id;
	num[1] = '\0';

	result = strcat(result,num);

	/* pripojeni pripony */
	result = strcat(result,".bmp");
	return result;
}

structure_sprite::structure_sprite(Uint32 pixw, Uint32 pixh, char id)
{
	rect.x = 0;
	rect.y = 0;
	rect.w = pixw;
	rect.h = pixh;

	char * filename = wall_file(id); 
	SDL_Surface * wall = SDL_LoadBMP(filename);
	
	structure_img = SDL_LoadBMP(FLOOR_FILE);
	if( structure_img == NULL )
	{
		fprintf(stderr,"structure_sprite: can't load floor bitmap\n");
		exit(1);
	}
	
	SDL_SetColorKey(wall,SDL_SRCCOLORKEY,SDL_MapRGB(wall->format,255,255,255));

	SDL_BlitSurface(wall,NULL,structure_img,NULL);
	SDL_FreeSurface(wall);
}

structure_sprite::~structure_sprite()
{
	SDL_FreeSurface(structure_img);
}

/* komentar k pomocne funkci decide_location
 *
 * #-----
 * |XXXXX
 * |XXXXX
 *
 * X : dlazdicky v zaberu kamery
 * #,-,| : dlazdicky mimo zaber kamery, jejichz souradnice jsou treba jako pocatecni bod pro 
 * SDL_BlitSurface pri vykreslovani obrazku tanku
 */

char decide_location( size_t x, size_t y, size_t w, size_t h)
{
	if( (x >= w ) && (y >= h ) )
		return BOTTOM_RIGHT;
	
	if( (x >= w ) && (y < h ) )
		return TOP_RIGHT;
	
	if( (x < w ) && (y >= h ) )
		return BOTTOM_LEFT;
	
	if( (x < w ) && (y < h ) )
		return TOP_LEFT;
	
	return ERROR;
}

