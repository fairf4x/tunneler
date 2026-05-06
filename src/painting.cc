#include "painting.h"
#include <string.h>

#define SPRITE_UP 		"./img/tank_up.png"
#define SPRITE_DOWN 		"./img/tank_down.png"
#define SPRITE_LEFT 		"./img/tank_left.png"
#define SPRITE_RIGHT 		"./img/tank_right.png"
#define SPRITE_UP_LEFT 		"./img/tank_up_left.png"
#define SPRITE_UP_RIGHT 	"./img/tank_up_right.png"
#define SPRITE_DOWN_LEFT 	"./img/tank_down_left.png"
#define SPRITE_DOWN_RIGHT 	"./img/tank_down_right.png"

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

tank_sprites::tank_sprites(SDL_Renderer * renderer, Uint32 pixw, Uint32 pixh, char id)
{
	SDL_Surface * color_dot_img; /* barevne rozliseni tanku */
	char * filename = dot_file(id);
	rect.x = 0;
	rect.y = 0;
	rect.w = pixw;
	rect.h = pixh;

	color_dot_img = IMG_Load(filename);

	SDL_SetColorKey(color_dot_img,SDL_TRUE,SDL_MapRGB(color_dot_img->format,255,255,255));

	SDL_Surface * tank_up_img = IMG_Load(SPRITE_UP);
	SDL_BlitSurface(color_dot_img,NULL,tank_up_img,NULL);
	tank_up = SDL_CreateTextureFromSurface(renderer,tank_up_img);
	SDL_FreeSurface(tank_up_img);

	SDL_Surface * tank_down_img = IMG_Load(SPRITE_DOWN);
	SDL_BlitSurface(color_dot_img,NULL,tank_down_img,NULL);
	tank_down = SDL_CreateTextureFromSurface(renderer,tank_down_img); 
	SDL_FreeSurface(tank_down_img);

	SDL_Surface * tank_left_img = IMG_Load(SPRITE_LEFT);
	SDL_BlitSurface(color_dot_img,NULL,tank_left_img,NULL);
	tank_left = SDL_CreateTextureFromSurface(renderer,tank_left_img);
	SDL_FreeSurface(tank_left_img);

	SDL_Surface * tank_right_img = IMG_Load(SPRITE_RIGHT);
	SDL_BlitSurface(color_dot_img,NULL,tank_right_img,NULL);
	tank_right = SDL_CreateTextureFromSurface(renderer,tank_right_img);
	SDL_FreeSurface(tank_right_img);

	SDL_Surface * tank_up_left_img = IMG_Load(SPRITE_UP_LEFT);
	SDL_BlitSurface(color_dot_img,NULL,tank_up_left_img,NULL);
	tank_up_left = SDL_CreateTextureFromSurface(renderer,tank_up_left_img);
	SDL_FreeSurface(tank_up_left_img);

	SDL_Surface * tank_up_right_img = IMG_Load(SPRITE_UP_RIGHT);
	SDL_BlitSurface(color_dot_img,NULL,tank_up_right_img,NULL);
	tank_up_right = SDL_CreateTextureFromSurface(renderer,tank_up_right_img);
	SDL_FreeSurface(tank_up_right_img);

	SDL_Surface * tank_down_left_img = IMG_Load(SPRITE_DOWN_LEFT);
	SDL_BlitSurface(color_dot_img,NULL,tank_down_left_img,NULL);
	tank_down_left = SDL_CreateTextureFromSurface(renderer,tank_down_left_img);
	SDL_FreeSurface(tank_down_left_img);

	SDL_Surface * tank_down_right_img = IMG_Load(SPRITE_DOWN_RIGHT);
	SDL_BlitSurface(color_dot_img,NULL,tank_down_right_img,NULL);
	tank_down_right = SDL_CreateTextureFromSurface(renderer,tank_down_right_img);
	SDL_FreeSurface(tank_down_right_img);

	SDL_FreeSurface(color_dot_img);

}

tank_sprites::~tank_sprites()
{
	SDL_DestroyTexture(color_dot);
	SDL_DestroyTexture(tank_up);
	SDL_DestroyTexture(tank_down);
	SDL_DestroyTexture(tank_left);
	SDL_DestroyTexture(tank_right);
	SDL_DestroyTexture(tank_up_left);
	SDL_DestroyTexture(tank_up_right);
	SDL_DestroyTexture(tank_down_left);
	SDL_DestroyTexture(tank_down_right);
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
	SDL_Surface * wall_img = IMG_Load(filename);
	SDL_Surface * floor_img = IMG_Load(FLOOR_FILE);

	if( wall_img == NULL || floor_img == NULL)
	{
		fprintf(stderr,"structure_sprite: can't load structure sprites\n");
		exit(1);
	}
		
	SDL_SetColorKey(wall_img,SDL_TRUE,SDL_MapRGB(wall_img->format,255,255,255));
	SDL_BlitSurface(wall_img,NULL,floor_img,NULL);
	structure_img = SDL_CreateTextureFromSurface(renderer,floor_img);

	SDL_FreeSurface(wall_img);
	SDL_FreeSurface(floor_img);
}

structure_sprite::~structure_sprite()
{
	SDL_DestroyTexture(structure_img);
}

/* komentar k pomocne funkci decide_location
 *
 * +-----
 * |XXXXX
 * |XXXXX
 *
 * X : dlazdicky v zaberu kamery
 * +,-,| : dlazdicky mimo zaber kamery, jejichz souradnice jsou treba jako pocatecni bod pro 
 * SDL_RenderCopy pri vykreslovani obrazku tanku
 */

EdgeCase decide_location( size_t x, size_t y, size_t w, size_t h, area * rect)
{
	if( (x >= w ) && (y >= h ) )
		return BOTTOM_RIGHT;
	
	if( (x >= w ) && (y < h ) )
		return TOP_RIGHT;
	
	if( (x < w ) && (y >= h ) )
		return BOTTOM_LEFT;
	
	if( (x < w ) && (y < h ) )
		return TOP_LEFT;

	return CENTER;
}

