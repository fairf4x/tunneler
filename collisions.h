#ifndef _COLLISIONS_H
#define _COLLISIONS_H

#include "tmap.h"

#include <set>
#include <list>
extern tmap * landscape;

struct coord
{
	coord(int xc, int yc) 	/* konstruktor upravuje souradnice */
	{
		landscape->correct_coord(xc,yc);
		x = xc;
		y = yc;
	}
	coord(){}
	int x;		/* podle width a height globalni promenne map * landscape; */
	int y;
};

/* funktor pro porovnavani dvou struktur typu coord (na nerovnost) */
struct coord_comp
{
	bool operator()(const coord & A, const coord & B) const
	{
		if(A.x < B.x)
			return (true);
		else
			{
				if(A.x == B.x)
					return (A.y < B.y);
			}
		return (false);
	};
};

typedef	std::set < struct coord, coord_comp > coord_set;
typedef coord_set::iterator coord_set_iter;
typedef std::pair < coord_set_iter,bool > retval;

typedef std::list < struct coord > coord_list;
typedef coord_list::iterator coord_list_iter;

struct area
{
	area(int xc, int yc, size_t width, size_t height);
	area(){};	/* defaultni konstruktor - kvuli konstruktoru collision_shape */
	int x; 		/* je treba pouzit int - pri zmene souradnic je nutna moznost uchovat */
	int y;		/* docasne zapornou hodnotu (v zapeti nutno opravit pomoci map::correct_coord) */
	size_t w;
	size_t h;
};

struct collision_shape: public area
{
	collision_shape(int xc, int yc, size_t width, size_t height,char * bitmap);
	bool map_collision(char type); /* zjisti kolizi s mapou (pouze policka zadaneho typu) */
	coord_set shape; /* mnozina souradnic vzhledem k (0,0) */
};

/* operator "hrube" kolize (zjisti zda koliduji ctverce vymezene danymi objekty) */
bool operator%(const area & A, const area & B);

/* operator "hrube" kolize pro area a bod zadany pomoci coord */
bool operator%(const area & A, const coord & C);

/* operator "jemne" kolize (zkouma kolizi bitmap danych objektu) */
bool operator*(const collision_shape & A, const collision_shape & B);

/* operator "jemne" kolize pro collision_shape a bod */
bool operator*(const collision_shape & A, const coord & C);

#endif
