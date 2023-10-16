#include "collisions.h"
#include <string.h>

collision_shape::collision_shape(int xc, int yc, size_t width, size_t height,char * bitmap)
{
	landscape->correct_coord(xc,yc);
	x = xc;
	y = yc;

	w = width;
	h = height;

	if( (width*height) != strlen(bitmap) )
		throw "collision_shape::collision_shape : WRONG BITMAP";
	
	/* inicializace mnoziny shape
	 * dle retezce znaku tvoreneho 0 a 1 */
	size_t i;
	size_t j;
	size_t pos = 0;

	for(i = 0; i < height; ++i)
	{
		for(j = 0; j < width; ++j)
		{
			if(bitmap[pos] == '1')
				shape.insert(coord(j,i));
			++pos;
		}
	}
}

bool collision_shape::map_collision(char type)
{
	char zn;
	coord_set_iter it;
	for(it = shape.begin(); it != shape.end(); ++it)
	{
		zn = landscape->get_xy((*it).x + x, (*it).y + y);
		if(zn == type)
			return (true);
	}
	return (false);
}

bool operator%(const area & A, const area & B)
{
	/* kontrola prekryti projekce na osu x */
	if ( 	(AbsClockwise(A.x, B.x, HORIZONTAL, landscape) < A.w ) || 
		(AbsClockwise(B.x, A.x, HORIZONTAL, landscape) < B.w )		)
	{
		/* doslo k prekryti -> je potreba zkontrolovat vuci ose y */
		if (	(AbsClockwise(A.y, B.y, VERTICAL, landscape) < A.h ) ||
			(AbsClockwise(B.y, A.y, VERTICAL, landscape) < B.h )	)
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

bool operator%(const area & A, const coord & C)
{
	/* kontrola prekryti projekce na osu x */
	if ( 	(AbsClockwise(A.x, C.x, HORIZONTAL, landscape) < A.w))
	{
		/* doslo k prekryti -> je potreba zkontrolovat vuci ose y */
		if (	(AbsClockwise(A.y, C.y, VERTICAL, landscape) < A.h))
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


/* operator "jemne" kolize (zkouma kolizi "shape mnozin" danych objektu)
 * algoritmus:
 * A,B - objekty jejichz kolize se zkouma
 * docasna_mnozina := TMP
 * 1. vloz prvky z A.shape do TMP
 * 2. vkladej prvky z B.shape do TMP - pokud uz tam vkladany prvek je, doslo ke kolizi
 *
 * pozn.: je treba prepocitavat hodnoty vkladane do TMP dle aktualnich souradnic */
bool operator*(const collision_shape & A, const collision_shape & B)
{
	/* docasna mnozina */
	coord_set temp_set;
	/* pomocny iterator */
	coord_set_iter it;
	/* promenna pro kontrolu navratove hodnoty coord_set.insert */
	retval	ret;
	
	/* vkladani prepocitanych souradnic z A.shape do temp_set
	 * pricitaji se aktualni souradnice, ktere se rovnou upravi
	 * "modulo" vzhledem k rozmerum aktualni mapy v konstruktoru coord */
	for(it = A.shape.begin(); it != A.shape.end(); ++it)
	{
		temp_set.insert(coord( ((*it).x + A.x),((*it).y + A.y) ));
	}

	/* vkladani prepocitanych souradnic z B.shape */
	/* pokud se nepovede vlozit doslo ke kolizi */
	for(it = B.shape.begin(); it != B.shape.end(); ++it)
	{
		ret = temp_set.insert(coord( (*it).x + B.x, (*it).y + B.y ));
		if( ret.second == false )
			return (true);
	}
	return (false);
}

bool operator*(const collision_shape & A, const coord & C)
{
	/* docasna mnozina */
	coord_set temp_set;
	/* pomocny iterator */
	coord_set_iter it;
	/* promenna pro kontrolu navratove hodnoty coord_set.insert */
	retval	ret;

	for(it = A.shape.begin(); it != A.shape.end(); ++it)
	{
		temp_set.insert( coord((*it).x + A.x, (*it).y + A.y) );
	}
	
	ret = temp_set.insert(C);

	if(ret.second == false)
		return (true);
	else
		return (false);
}

area::area(int xc, int yc, size_t width, size_t height)
{
	landscape->correct_coord(xc,yc);
	x = xc;
	y = yc;

	w = width;
	h = height;
}
