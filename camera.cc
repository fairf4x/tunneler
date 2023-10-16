#include <math.h>
#include "camera.h"
#include "tank.h"

extern Uint32 mud_color;
extern Uint32 rock_color;
extern Uint32 path_color;
extern Uint32 stru_color;
extern Uint32 fire_color;

camera::camera(SDL_Surface * can,SDL_Rect * win)
{
	/* mapa nesmi byt mensi nez 
	 * okno kamery
	 * a platno nesmi byt NULL */	
	if( 	((landscape->width) >= (win->w / TILE_SIZE)) && 		
		((landscape->height) >= (win->h / TILE_SIZE)) &&
		(can != NULL) )
	{
		window = win;
		canvas = can;
	}
	else
	{
		window = NULL;			/* nezdarila se inicializace */
		canvas = NULL;			
	}

	visible_area.x = 0;
	visible_area.y = 0;
	
	visible_area.w = window->w / TILE_SIZE;	/* kolik dlazdicek se vejde do okna na sirku */
	visible_area.h = window->h / TILE_SIZE;	/* -//- na vysku */

}

camera::~camera()
{
	delete window;
}

void camera::set(coord position)		/* zameri stred kamery na dane souradnice */
{
	int nx = position.x;
	int ny = position.y;

	nx -= (visible_area.w / 2);
	ny -= (visible_area.h / 2);

	landscape->correct_coord(nx,ny);

	visible_area.x = nx;
	visible_area.y = ny;

	return;
}

void camera::shot(void)
{
	
	int i;
	int j;
	int s;			/* sirka souvisleho pasku stejnych dlazdic */

	SDL_Rect ctv;		/* aktualne vyplnovany ctverecek */
	ctv.w = 0;
	ctv.h = TILE_SIZE;
	ctv.x = window->x;	/* nastaveni pocatecni pozice v okne */
	ctv.y = window->y;

	char act;

	/* optimalizace vykreslovani - snaha snizit pocet volani SDL_FillRect
	 * misto vykreslovani kazde dlazdicky zvlast
	 * se napocita sirka souvisleho pasu stejnych dlazdicek (promenna s)
	 * a provede se vykresleni 1 obdelniku namisto s ctverecku
	 * v nejlepsim pripade se vykresli na jedno volani SDL_FillRect cely radek */

	for(j = 0; j < visible_area.h; ++j)
	{
		i = 0;
		while(i < visible_area.w)
		{
			act = landscape->get_xy(visible_area.x+i,visible_area.y+j);
			s = 1; /*minimalne jeden ctverecek se vykresli vzdy */
			while(	(i+s) < visible_area.w &&
				landscape->get_xy(visible_area.x+i+s,visible_area.y+j) == act )
				++s;

			ctv.w += s*TILE_SIZE;
			switch(act)
			{
				case ROCK:
					SDL_FillRect(canvas,&ctv,rock_color);
				break;
				case MUD:
					SDL_FillRect(canvas,&ctv,mud_color);
				break;
				case PATH:
					SDL_FillRect(canvas,&ctv,path_color);
				break;
				case STRUCTURE:
					SDL_FillRect(canvas,&ctv,stru_color);
				break;
				default:
				break;
			}
			i += s;
			ctv.x += ctv.w;
			ctv.w = 0;
		}
		ctv.x = window->x;	/* novy radek ctverecku */
		ctv.y += TILE_SIZE;
	}

	return;
}


int camera::target_x(void)		/* vrati x-ovou souradnici dlazdicky uprostred zaberu */
{
	return visible_area.x + (visible_area.w / 2);
}

int camera::target_y(void)		/* vrati y-ovou souradnici dlazdicky uprostred zaberu */
{
	return visible_area.y + (visible_area.h / 2);
}

bool camera::is_on_screen(int inx,int iny)	/* zjisti zda je bod na danych souradnicich v zaberu*/
{
	landscape->correct_coord(inx,iny);

	/* inx musi byt mezi x a x+h_tiles a iny musi byt mezi y a y+v_tiles */ 
	if ( 	(AbsClockwise(visible_area.x,inx,HORIZONTAL,landscape) < visible_area.w) &&
		(AbsClockwise(visible_area.y,iny,VERTICAL,landscape) < visible_area.h)	)
		return true;
	else
		return false;
}

int camera::get_screen_coords(int mx, int my, SDL_Rect & spot)
{
	if(is_on_screen(mx,my))
	{
		spot.x = window->x + TILE_SIZE * AbsClockwise(visible_area.x,mx,HORIZONTAL,landscape);
		spot.y = window->y + TILE_SIZE * AbsClockwise(visible_area.y,my,VERTICAL,landscape);
		spot.w = TILE_SIZE;
		spot.h = TILE_SIZE;

		return 0;
	}
	else
		return -1;
}

void camera::print_coords()
{
	printf("X: %d Y: %d\n------------\n",visible_area.x,visible_area.y);
	return;
}

SDL_Rect * camera::get_window_rect()
{
	return (window);
}
