#ifndef _MAP_H
#define _MAP_H

#include <stdlib.h>
#include <set>

// definice "policek" mapy

#define ROCK	'#'
#define MUD	'+'
#define PATH	'.'

// definice os pro funkci AbsClockwise
#define VERTICAL 'V'
#define HORIZONTAL 'H'

// dopredna deklarace struktur z collision.h
// nutno includovat collisions.h v map.cc (kvuli definici)
struct area;
struct coord;
struct coord_comp;

typedef	std::set < coord, coord_comp > coord_set;

class map
{
	public:
	map(int input);
	char get_xy(int x, int y);		// vrati typ dlazdice na (x,y) => ROCK | MUD | PATH
	void set_xy(int x, int y, char c);	// nastavi dlazdici na pozici (x,y)
	void set_area(area & field, char c);	// nastavi dlazdice na d
	
	// nastavi dlazdice urcene mnozinou souradnic
	void set_tiles(int xc, int yc, coord_set & set, char c);	
	
	void correct_coord(int & x, int & y);	// "opravi" promenne vzhledem k rozmerum mapy
					
	private:
	void parse_first_line(int input);
	
	typedef char* row;
	row * ground;
	int width;
	int height;

friend class camera;
friend size_t AbsClockwise(int ,int , char , map *);
};


/* pro zjisteni absolutni vzdalenosti dvou bodu na mape je treba pocitat s tim
 * ze je "nekonecna" ( napr. po souradnici <width,height> nasleduje 
 * pri pohybu DOWN_RIGHT opet <0,0>)
 * je tedy treba pocitat vsechno "modulo" v zavislosti na width a height
 * vzdalenost se da interpretovat 2 zpusoby: po a proti smeru hodinovych rucicek
 * AbsClockwise je funkce pro zjisteni absolutni vzdalenosti dvou bodu
 */

size_t	AbsClockwise(int p1, int p2, char axis, map * landscape);

#endif
