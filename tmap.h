#ifndef _TMAP_H
#define _TMAP_H

#include <stdlib.h>
#include <set>

// definice "policek" mapy

#define ROCK		'#'
#define MUD		'+'
#define PATH		'.'
#define STRUCTURE 	'*'

// definice os pro funkci AbsClockwise
#define VERTICAL 'V'
#define HORIZONTAL 'H'

// dopredna deklarace struktur z collision.h
// nutno includovat collisions.h v tmap.cc (kvuli definici)
struct area;
struct coord;
struct coord_comp;

typedef	std::set < coord, coord_comp > coord_set;

struct area;

class tmap
{
	public:
	tmap(int input);
	~tmap();
	char get_xy(int x, int y);		/* vrati typ dlazdice na (x,y) 
						 * ROCK | MUD | PATH | STRUCTURE */
	char get_xy(const coord tc);

	bool set_xy(int x, int y, char c);	/* nastavi dlazdici na pozici (x,y) */
	bool set_xy(const coord & tc, char c);

	void set_area(area & field, char c);	/* nastavi dlazdice na dane plose */
	
	/* nastavi dlazdice urcene mnozinou souradnic - vrati pocet tech, ktere pri tom zmenily stav */
	int set_tiles(int xc, int yc, coord_set & set, char c);	
	/* nastavi dane dlazdice pouze pokud jde o zmenu z "from" na "to" */
	void change_tiles(int xc, int yc,const coord_set set, char from, char to);	
	
	void correct_coord(int & x, int & y);	/* "opravi" promenne vzhledem k rozmerum mapy */
	
	friend class camera;
	friend size_t AbsClockwise(int ,int , char , tmap *);
	friend area * generate_place(tmap *);

	private:
	void parse_first_line(int input);	/* nacte informace o mape z prvni radky souboru */
	
	typedef char* row;
	row * ground;
	int width;
	int height;

};


/* pro zjisteni absolutni vzdalenosti dvou bodu na mape je treba pocitat s tim
 * ze je "nekonecna" ( napr. po souradnici <width,height> nasleduje 
 * pri pohybu DOWN_RIGHT opet <0,0>)
 * je tedy treba pocitat vsechno "modulo" v zavislosti na width a height
 * vzdalenost se da interpretovat 2 zpusoby: po a proti smeru hodinovych rucicek
 * AbsClockwise je funkce pro zjisteni absolutni vzdalenosti dvou bodu
 */

size_t	AbsClockwise(int p1, int p2, char axis, tmap * landscape);

#endif
