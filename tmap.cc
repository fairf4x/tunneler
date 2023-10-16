#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "tmap.h"
#include "collisions.h"

/* pomocne funkce */
int my_read(int file,char * buff,size_t nbyte)
{
	int i;
	size_t readed;
	for(i=0;i<nbyte;i += readed)
	{
		if((readed = read(file,buff,nbyte-i)) == -1)
			perror("read");
		if(readed == 0)
			return i;
	}

	return nbyte;
}

char my_getc(int file)
{
	char zn;
	if(my_read(file,&zn,1) > 0)	//cte se jeden znak
		return zn;
	else
		printf("ERROR - nelze cist vstup\n");
	return 0;
}

int my_getnum(int file,char delim)
{
	char str[10];
	int i=0;
	for(i=0;(str[i]= my_getc(file))!=delim;++i);
	str[i]=0;
	return atoi(str);
}
/**********************************/

tmap::tmap(int input)
{
	/* nacteni udaju o mape (rozmery atd.) z prvni radky souboru */
	parse_first_line(input);

	/* alokace ukazatelu na jednotlive radky */
	ground = new row[height];
	
	int i;

	/* alokace a nacteni jednotlivych radku */
	for ( i = 0; i < height; ++i )
	{
		ground[i] = new char[width];

		my_read(input,ground[i],width);
	}
	
	close(input);	
}

tmap::~tmap()
{
	int i;
	for ( i = 0; i < height; ++i )
		delete ground[i];
}

char tmap::get_xy(int x, int y)		/* vrati znak z pozice (x,y) */
{
	correct_coord(x,y);
	return ground[y][x];
}

char tmap::get_xy(const coord tc)		/* vrati znak z pozice dane coord */
{
	int x = tc.x;
	int y = tc.y;
	correct_coord(x,y);
	return ground[y][x];
}

bool tmap::set_xy(int x, int y, char c)	/* nastavi znak na pozici (x,y) */
{
	correct_coord(x,y);
	
	if( ground[y][x] != c)
	{
		ground[y][x]=c;
		return (true);		/* doslo ke zmene */
	}
	else
		return (false);		/* nedoslo ke zmene */
}

bool tmap::set_xy(const coord & tc, char c)	/* nastavi znak na pozici urcene souradnicemi (x,y) */
{
	int x = tc.x;
	int y = tc.y;

	correct_coord(x,y);
	
	if( ground[y][x] != c)
	{
		ground[y][x]=c;
		return (true);		/* doslo ke zmene */
	}
	else
		return (false);		/* nedoslo ke zmene */
}

void tmap::set_area(area & field, char c)	/* nastavi dlazdice na dane plose */
{
	int i;
	int j;
	for(i=0; i < field.w; ++i)
	{
		for(j=0; j < field.h; ++j)
			set_xy(field.x + i, field.y + j, c);
	}
}

/* nastavi dlazdice dane mnozinou souradnic - vrati pocet tech, ktere pritom zmenily stav */
int tmap::set_tiles(int xc, int yc, coord_set & set, char c) 
{
	int tile_cnt = 0;	/* pocet zmenenych dlazdic */

	coord_set_iter it;
	for(it = set.begin(); it != set.end(); ++it)
		if( set_xy( xc + (*it).x, yc + (*it).y , c) )
			++tile_cnt;
	
	return (tile_cnt);
}

void tmap::change_tiles(int xc, int yc,const coord_set set, char from, char to) 
/* zmeni dlazdice urcene mnozinou souradnic - pouze v pripade ze jsou typu "from" */
{
	correct_coord(xc,yc);
	coord_set_iter it;
	int x;
	int y;

	for(it = set.begin(); it != set.end(); ++it)
	{
		x = (*it).x + xc;
		y = (*it).y + yc;

		if(get_xy(x,y) == from)
			set_xy(x,y,to);
	}
}

void tmap::correct_coord(int & x, int & y)	/* "opravi" promenne vzhledem k rozmerum mapy */
{
	x = x % width;
	if(x<0)
		x += width;
	y = y % height;
	if(y<0)
		y += height;
	return;
}

/* rozparsuje prvni radek souboru s mapou
 * - predpoklada filedescriptor otevreneho souboru s mapou */
void tmap::parse_first_line(int input)
{
	width = my_getnum(input,'X');
	height = my_getnum(input,'#');
	printf("map size: %dX%d\n",width,height);
	while(my_getc(input)!='\n');
}

size_t	AbsClockwise(int p1, int p2, char axis, tmap * landscape)
{
	if (p1 == p2)
		return (0);
	
	if (p1 < p2)
		return ( p2 - p1 );
	
	size_t c;
	
	switch (axis)
	{
		case VERTICAL: c = landscape->height;
		break;
		case HORIZONTAL: c = landscape->width;
		break;
		default: fprintf(stderr,"camera::AbsClockwise: unknown axis\n");
	}

	if (p1 > p2)
		return ( p2 - p1 + c);
	
	fprintf(stderr,"camera::AbsClockwise: can't determine value\n");
	return (0); 
}
