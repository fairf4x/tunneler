#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <sys/types.h>
#include <unistd.h>
#include "commondef.h"

#define PLACE_WIDTH	51
#define PLACE_HEIGHT	51

#define OFFSET_X	15
#define OFFSET_Y	15

/* definice os pro funkci AbsClockwise */
#define VERTICAL	'V'
#define HORIZONTAL	'H'

struct area{
	area(int xc, int yc, size_t wd, size_t hd, size_t xmax, size_t ymax)
	{
		x = xc % xmax;
		y = yc % ymax;
		w = wd;
		h = hd;
	}
	int x;
	int y;
	size_t w;
	size_t h;
};

using namespace std;
typedef list <area*> place_list;
typedef place_list::iterator place_list_it;

/* pomocne funkce */
char my_getc(int file)
{
	char zn;
	if(read(file,&zn,1) > 0)	/* cte se jeden znak */
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
/********************************************************/


size_t	AbsClockwise(int p1, int p2, char axis, size_t xbound, size_t ybound)
{
	if (p1 == p2)
		return (0);
	
	if (p1 < p2)
		return ( p2 - p1 );
	
	size_t c;
	
	switch (axis)
	{
		case VERTICAL: c = ybound;
		break;
		case HORIZONTAL: c = xbound;
		break;
		default: fprintf(stderr,"camera::AbsClockwise: unknown axis\n");
	}

	if (p1 > p2)
		return ( p2 - p1 + c);
	
	fprintf(stderr,"camera::AbsClockwise: can't determine value\n");
	return (0); 
}

area * generate_place(int map_x,int map_y)
{
	int xc = random() % map_x;
	int yc = random() % map_y;
	area * result = new area(xc,yc,PLACE_WIDTH,PLACE_HEIGHT,(size_t)map_x,(size_t)map_y);

	return result;
}

bool collision(area * A,area * B, size_t xbound,size_t ybound)
{
	/* kontrola prekryti projekce na osu x */
	if ( 	(AbsClockwise(A->x, B->x, HORIZONTAL, xbound,ybound) < A->w ) || 
		(AbsClockwise(B->x, A->x, HORIZONTAL, xbound,ybound) < B->w )		)
	{
		/* doslo k prekryti -> je potreba zkontrolovat vuci ose y */
		if (	(AbsClockwise(A->y, B->y, VERTICAL, xbound,ybound) < A->h ) ||
			(AbsClockwise(B->y, A->y, VERTICAL, xbound,ybound) < B->h )	)
			/* doslo k prekryti */
			return (true);
		else
			/* nedoslo k prekryti */
			return (false);
	}
	else
		/* nedoslo k prekryti projekce na osu x -> neprekryvaji se */
		return (false);

}

bool fits(area * new_place, place_list * old_places,size_t xbound, size_t ybound)
{
	place_list_it plit;

	/* kontrola kolize s existujicimi misty */
	for(plit = old_places->begin(); plit != old_places->end(); ++plit)
	{
		if( collision(new_place,(*plit),xbound,ybound) )
			return (false);
	}
	
	return (true);
}

place_list * deploy_places(int cnt,int map_x, int map_y)
{
	place_list * places = new place_list;

	places->push_back(generate_place(map_x,map_y));

	int i;
	area * new_place;

	for(i=1; i < cnt; ++i)
	{
		new_place = generate_place(map_x,map_y);

		while(!fits(new_place,places,(size_t)map_x,(size_t)map_y))
		{
			delete new_place;
			new_place = generate_place(map_x,map_y);
		}

		places->push_back(new_place);
	}

	return places;
}


int * generate_init_vect(int mapfile,unsigned char player_cnt)
{
	/* precteni velikosti mapy */
	int map_x = my_getnum(mapfile,'X');
	int map_y = my_getnum(mapfile,'#');

	fprintf(stderr,"Map size: %dX%d\n",map_x,map_y);

	int * res = new int[MAX_PLAYER_CNT*2];
	int * act_pos = res;
	/* inicializace seedu */
	srandom(getpid());

	/* nagenerovani "mist" */
	place_list * places = deploy_places(player_cnt,map_x,map_y);
	place_list_it plit;
	
	/* zapsani souradnic */
	for( plit=places->begin(); plit != places->end(); ++plit)
	{
		(*act_pos) = (*plit)->x + OFFSET_X;
		++act_pos;
		(*act_pos) = (*plit)->y + OFFSET_Y;
		++act_pos;
	}
	
	/* vynulovani zbytku vektoru */
	while((act_pos - res) < MAX_PLAYER_CNT*2)
	{
		(*act_pos) = 0;	
		++act_pos;
	}
	
	delete places;
	
	return res;
}
