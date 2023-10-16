#include <stdio.h>
#include <string.h>

#include "game.h"
#include "statusbar.h"

/* ---- definice pro SDL ---- */
#define SDL_INIT_FLAGS	SDL_INIT_VIDEO|SDL_INIT_TIMER	/* flagy pro funkci SDL_Init */
#define SDL_MODE_FLAGS	SDL_ANYFORMAT|SDL_DOUBLEBUF|SDL_SRCALPHA /* flagy pro funkci SDL_SetVideoMode */
#define VIDEO_WIDTH 1600		/* sirka obrazovky */
#define VIDEO_HEIGHT 900		/* vyska obrazovky */
#define BORDER 10

/* ---- definice pro timer ---- */
#define TIMER_EVENT 0
#define TIMER_DELAY 100

#define FIRST (i*2)
#define SECOND (i*2+1)

#define FRAME_SPLIT_IMG 	"./img/frame_split_1600x900.bmp"
#define FRAME_IMG		"./img/frame_1600x900.bmp"

/* maximum zivotu ktere budou videt */
#define VISIBLE_LIVES 10

/* tvar vybuchu strel */
coord_set * dig_hole;

/* promenne uchovavajici barvy pouzite k vykreslovani */
Uint32 mud_color;
Uint32 rock_color;
Uint32 path_color;
Uint32 stru_color;

Uint32 fire_color;

/* textura ramecku */
SDL_Surface * frame;
Uint32 frame_key_color;

/* retezec pro prijate zpravy */
unsigned char answer[SERVER_MSG_LEN];

game::game(bool split,int map_input)
{
	/* inicializace SDL */
	if(SDL_Init(SDL_INIT_FLAGS)==-1)
	{
		SDL_Quit();
		/* nepovedlo se inicializovat SDL */
		fprintf(stderr,"Fatal error: SDL_Init failed\n");
	}
	
	screen = SDL_SetVideoMode(VIDEO_WIDTH,VIDEO_HEIGHT,0,SDL_MODE_FLAGS);

	if(screen==NULL)
	{
		SDL_Quit();
		/* nepovedlo se nastavit pozadovane rozliseni */
		fprintf(stderr,"Fatal error: Can't initialize screen\n");
	}

	/* inicializace mapy */
	landscape = new tmap(map_input);

	/* inicializace kamer */

	SDL_Rect * camwin;

	if(split)
	{
		/* prvni kamera */
		camwin = new SDL_Rect;
	
		camwin->w = ( VIDEO_WIDTH - 4 * BORDER ) / 2 ;
		camwin->h = VIDEO_HEIGHT - 2 * BORDER;
		camwin->x = camwin->w + 3 * BORDER;
		camwin->y = BORDER;
	
		cam1 = new camera(screen,camwin);
		
		/* druha kamera */
		camwin = new SDL_Rect;
	
		camwin->w = ( VIDEO_WIDTH - 4 * BORDER ) / 2 ;
		camwin->h = VIDEO_HEIGHT - 2 * BORDER;
		camwin->x = BORDER;
		camwin->y = BORDER;
	
		cam2 = new camera(screen,camwin);
		
		/* status bar */
		init_status_bar(camwin->w - 2 * BORDER);

		/* pocet zivotu */
		init_lives(screen);

		/* final screen */
		init_final_screen(screen);

		frame = SDL_LoadBMP(FRAME_SPLIT_IMG);
	}
	else
	{
		camwin = new SDL_Rect;

		camwin->w = ( VIDEO_WIDTH - 2 * BORDER );
		camwin->h = VIDEO_HEIGHT - 2 * BORDER;
		camwin->x = BORDER;
		camwin->y = BORDER;

		cam1 = new camera(screen,camwin);
		
		/* status bar */
		init_status_bar(camwin->w - 2 * BORDER);

		/* pocet zivotu */
		init_lives(screen);

		/* final screen */
		init_final_screen(screen);

		frame = SDL_LoadBMP(FRAME_IMG);
	}

	/* ulozeni informace o splitscreenu (duvod: metoda Draw) */
	splitscreen = split;

	/* inicializace tvaru vybuchu */
	init_dig_hole();

	/* inicializace barev */
	mud_color = SDL_MapRGB(screen->format,168,92,15);
	rock_color = SDL_MapRGB(screen->format,77,77,77);
	path_color = SDL_MapRGB(screen->format,165,140,123);
	stru_color = SDL_MapRGB(screen->format,0,0,120);
	fire_color = SDL_MapRGB(screen->format,200,0,0);

	frame_key_color = SDL_MapRGB(frame->format,255,255,255);
}

game::~game()
{
	delete cam1;
	delete pla1;

	if(splitscreen)
	{
		delete cam2;
		delete pla2;
	}

	delete landscape;
	delete dig_hole;
	delete tanks;
	delete structures;

	SDL_FreeSurface(frame);

	destroy_status_bar();

	SDL_Quit();
}

void game::execute()
{
	char i=0;		/* iteracni promenna; FIRST == (i*2)  SECOND == (i*2+1) */
	char dead_cnt = 0;

	/* neplatne id uvozuje prazdnou zpravu
	 * vsechny nasledujici zpravy jsou prazdne 
	 * (za neplatne je v tomto kontextu povazovano ID >= MAX_PLAYER_CNT)
	 * id je obsazeno vzdy v prvnim znaku klientske zpravy answer[FIRST] */

/*	print_answer(answer); ladici vypis dosle zpravy */

	while( (i < player_cnt) && (answer[FIRST] < MAX_PLAYER_CNT) )
	{
		tank_map_it tmit;
		/* najdi prislusny tank */	
		tmit = tanks->find(answer[FIRST]);

		if( tmit != tanks->end() )
		{
			if(is_alive(answer[SECOND]))
			{	
				/* tank byl znicen, ale jeste ma dalsi zivoty */
				if(need_resurection(answer[SECOND]))
				{
					/* znovunasazeni tanku */
					(*tmit).second.reset();	
					if( answer[FIRST] == pla1->id )
						pla1->reset_tank_state();
					else
					{
						if( splitscreen && answer[FIRST] == pla2->id )
							pla2->reset_tank_state();
					}
					
					/* po resurekci se tank nehybe ani nestrili */
					break;
				}
				
				/* pohni s tankem - pokud se hybe */
				(*tmit).second.move(is_heading(answer[SECOND]));

				/* pokud je nastaven "fire" bit, vytvor strelu podle orientace tanku */
				if(is_firing(answer[SECOND]))
				{
					/* vytvoreni nove strely */
					fire.push_back(bullet((*tmit).second.canon_position(),(*tmit).second.dir()));

					/* odecteni energie za strelbu */
					(*tmit).second.fire();
				}
			}
			else
			{
				/* tank je mrtev */				
				tanks->erase(answer[FIRST]);
				fprintf(stderr,"ID #%d - GAME OVER\n",answer[FIRST]);
				
				if( answer[FIRST] == pla1->id )
					--(pla1->life);
				else
				{
					if( splitscreen && answer[FIRST] == pla2->id )
						--(pla2->life);
				}

				++dead_cnt;
			}
		}

		++i;
	}

	player_cnt -= dead_cnt;

	return;
}

void game::start(int sock)
{
	char life_cnt = 0;

	/* hra ve fullscreen modu */

	struct controls settings;
	
	/* zjisteni poctu hracu */
	player_cnt = get_char_answer(sock,PLAYER_CNT_REQUEST);

	/* zjisteni poctu zivotu */
	life_cnt = get_char_answer(sock,LIFE_CNT_REQUEST);

	/* vytvoreni objektu na mape - dle inicializacniho vektoru ze serveru */
	create_objects(sock);

	/* inicializace ovladani prvniho hrace */
	settings.up = SDLK_UP;
	settings.down = SDLK_DOWN;
	settings.left = SDLK_LEFT;
	settings.right = SDLK_RIGHT;
	settings.fire = SDLK_RCTRL;

	/* inicializace hrace */
	pla1 = new player(sock,get_char_answer(sock,ID_REQUEST),life_cnt,settings);

	/* zaslani potvrzeni serveru ze je vse OK a cekame na start */
	pla1->confirm_ok();

	/* cekani na start */
	if(pla1->wait_for_start())	
	{
		/* inicializace casovace */
		delay = TIMER_DELAY;
		timer = SDL_AddTimer(delay,callback,NULL);
		if (timer == NULL)
		{
			fprintf(stderr,"Fatal error: Can't initialize timer\n");
			return;
		}

		/* vstup do herni smycky */
		bool done = false;
		while(!done)
			done = ProcessEvents();
		
		}
	else
		fprintf(stderr,"game::start: can't start game\n");

	return;
}

void game::start(int sock1, int sock2)
{
	char life_cnt = 0;

	/* hra ve splitscreen modu */
	struct controls settings;
	
	/* zjisteni poctu hracu */
	player_cnt = get_char_answer(sock1,PLAYER_CNT_REQUEST);

	/* zjisteni poctu zivotu */
	life_cnt = get_char_answer(sock1,LIFE_CNT_REQUEST);

	/* vytvoreni objektu na mape - dle inicializacniho vektoru ze serveru */
	create_objects(sock1);

	/************************** PRVNI HRAC **************************/
	/* inicializace ovladani prvniho hrace */
	settings.up = SDLK_UP;
	settings.down = SDLK_DOWN;
	settings.left = SDLK_LEFT;
	settings.right = SDLK_RIGHT;
	settings.fire = SDLK_RCTRL;

	/* inicializace prvniho hrace */
	pla1 = new player(sock1,get_char_answer(sock1,ID_REQUEST),life_cnt,settings);
	fprintf(stderr,"pla1->id = %d\n",pla1->id);

	/* zaslani potvrzeni serveru ze je vse OK a cekame na start */
	pla1->confirm_ok();
	
	/************************** DRUHY HRAC **************************/
	/* inicializace ovladani druheho hrace */
	settings.up = SDLK_w;
	settings.down = SDLK_s;
	settings.left = SDLK_a;
	settings.right = SDLK_d;
	settings.fire = SDLK_LCTRL;

	/* inicializace druheho hrace */
	pla2 = new player(sock2,get_char_answer(sock2,ID_REQUEST),life_cnt,settings);
	fprintf(stderr,"pla2->id = %d\n",pla2->id);

	/* zaslani potvrzeni serveru ze je vse OK a cekame na start */
	pla2->confirm_ok();

	/*****************************************************************/

	/* cekani na start */
	bool ret1;
	bool ret2;

	if( (ret1 = pla1->wait_for_start()) && (ret2 = pla2->wait_for_start()))
	{
		/* inicializace casovace */
		delay = TIMER_DELAY;
		timer = SDL_AddTimer(delay,callback,NULL);
		if (timer == NULL)
		{
			fprintf(stderr,"Fatal error: Can't initialize timer\n");
			return;
		}

		/* vstup do herni smycky */
		bool done = false;
		while(!done)
			done = ProcessEvents();
	}
	else
		fprintf(stderr,"game::start: can't start game\n");
	
	return;
}

void game::DrawScreen_split()
{
	tank_map_it tmit;
	bool fin1,fin2;
	fin1 = pla1->life <= 0;
	fin2 = pla2->life <= 0;

	/* okno prvniho hrace */
	if( !fin1 )
	{
		tmit = tanks->find(pla1->id);

		if(tmit != tanks->end())
			cam1->set((*tmit).second.get_position());

		cam1->shot();
	}
	
	/* okno druheho hrace */
	if( !fin2 )
	{
		tmit = tanks->find(pla2->id);

		if(tmit != tanks->end())
			cam2->set((*tmit).second.get_position());

		cam2->shot();
	}
	
	if( fin1 && fin2 )
	{
		SDL_Flip(screen);
		return;
	}

	/* okna obou hracu */
	
	/* zobraz bunkry */
	structure_map::iterator smit;
	for(smit = structures->begin(); smit != structures->end(); ++smit)
	{
		(*smit).second.show(cam1); 
		(*smit).second.show(cam2);
	}

	/* tanky */
	if(tanks->size() >= 1)
	{
		for(tmit = tanks->begin(); tmit != tanks->end(); ++tmit)
		{
			(*tmit).second.show(cam1);
			(*tmit).second.show(cam2);
		}
	}

	/* ramecek okolo herniho pohledu */
	SDL_SetColorKey(frame,SDL_SRCCOLORKEY,frame_key_color);
	SDL_BlitSurface(frame,NULL,screen,NULL);

	/* strely */
	if(!fire.empty())
	{
		bullet_list_iter blit = fire.begin();
		while(blit != fire.end())
		{
			(*blit).show(cam1);
			(*blit).show(cam2);

			if((*blit).get_ttl() <= 0)
				blit = fire.erase(blit);
			else
				++blit;
		}
	}

	/* status bar a pocet zivotu */
	if( !fin1 )
	{
		tmit = tanks->find(pla1->id);
		show_status_bar(cam1,&((*tmit).second));
		show_lives(cam1,pla1,VISIBLE_LIVES);
	}
	else
	{
		SDL_FillRect(screen,cam1->window,rock_color);
		show_final_screen(cam1);
	}

	if( !fin2 )
	{
		tmit = tanks->find(pla2->id);
		show_status_bar(cam2,&((*tmit).second));
		show_lives(cam2,pla2,VISIBLE_LIVES);
	}
	else
	{
		SDL_FillRect(screen,cam2->window,rock_color);
		show_final_screen(cam2);
	}
	
	SDL_Flip(screen);

	return;
}

void game::DrawScreen()
{
	tank_map_it tmit;

	/* herni pohled na mapu */
	if( pla1->life > 0)
	{
		tmit = tanks->find(pla1->id);

		/* nakresli mapu */

		/*zamer kameru na "nas" tank (pokud zije) */
		if(tmit != tanks->end())
			cam1->set((*tmit).second.get_position());
		
		cam1->shot();
	}
	else
	{
		SDL_FillRect(screen,cam1->window,rock_color);
		show_final_screen(cam1);

		SDL_Flip(screen);
		return;
	}
	
	/* zobraz bunkry */
	structure_map::iterator smit;
	for(smit = structures->begin(); smit != structures->end(); ++smit)
		(*smit).second.show(cam1); 

	/* nakresli tanky ktere jsou v zaberu */
	for(tmit = tanks->begin(); tmit != tanks->end(); ++tmit)
		(*tmit).second.show(cam1);

	/* ramecek okolo herniho pohledu */
	SDL_SetColorKey(frame,SDL_SRCCOLORKEY,frame_key_color);
	SDL_BlitSurface(frame,NULL,screen,NULL);

	/* nakresli strely ktere jsou videt a odstran ty ktere uz doletely */
	if(!fire.empty())
	{
		bullet_list_iter blit = fire.begin();
		while(blit != fire.end())
		{
			(*blit).show(cam1);
			if((*blit).get_ttl() <= 0)
				blit = fire.erase(blit);
			else
				++blit;
		}
	}

	/* status bar */
	tmit = tanks->find(pla1->id);
	show_status_bar(cam1,&((*tmit).second));

	/* pocet zivotu */
	show_lives(cam1,pla1,VISIBLE_LIVES);

	SDL_Flip(screen);
	
	return;
}

bool game::ProcessEvents()
{
	SDL_Event ev;
	tank_map_it tmit;

	while(SDL_WaitEvent(&ev))
	{
		switch(ev.type)
		{
			case SDL_KEYDOWN:
				if(splitscreen)
				{
					/* zpracovani klaves prvniho hrace a specialnich klaves */
					switch(pla1->process_keypress(ev.key,true))
					{
						/* klavesy ktere nebyly zpracovany v process_keypress se zpracuji zde */
						case SDLK_ESCAPE:
						bool p1,p2;
							p1 = pla1->kill();
							p2 = pla2->kill();
							
							if(p1)
								pla1->recv_answer(answer);
							if(p2)
								pla2->recv_answer(answer);
							return (true);
						break;
						default:
						break;
					}
					/* zpracovani klaves druheho hrace */
					pla2->process_keypress(ev.key,true);
				}
				else
				{
					switch(pla1->process_keypress(ev.key,true))
					{
						/* klavesy ktere nebyly zpracovany v process_keypress se zpracuji zde */
						case SDLK_ESCAPE:
							if(pla1->kill())
								pla1->recv_answer(answer);
							return (true);
						break;
						default:
						break;
					}
				}
			break;
			case SDL_KEYUP:
				if(splitscreen)
				{
					pla1->process_keypress(ev.key,false);
					pla2->process_keypress(ev.key,false);
				}
				else
					pla1->process_keypress(ev.key,false);
			break;
			case SDL_USEREVENT:
				switch(ev.user.code)
				{
					case TIMER_EVENT:
						process_timer_tick();	
					break;
					default:
					break;
				}
			break;
			case SDL_QUIT:
				return (true);
			break;
			default:
			break;
		}
	}
	return (false);
}

void game::process_timer_tick()
{
	/*zastaveni timeru*/
	SDL_RemoveTimer(timer);

	/* pohyb strel */
	move_fire();
	
	/* pohyb tanku, strelba a vyrazeni */
	if(splitscreen)
	{
		if(pla1->life > 0)
			pla1->send_msg();
		
		if(pla2->life > 0)
			pla2->send_msg();
	}
	else
		if(pla1->life > 0)
			pla1->send_msg();
	
	/* obnova energie a armoru */
	recover_tanks();

	/* odpoved je pro vsechny hrace stejna */
	if(get_answer() != NULL )
		execute();
	
	/* vykresleni situace */
	if(splitscreen)
		DrawScreen_split();
	else
		DrawScreen();

	/* obnoveni timeru */
	timer = SDL_AddTimer(delay,callback,NULL);
}



void print_answer()
{
	int i;
	for(i=0; i < MAX_PLAYER_CNT*2; ++i)
		printf(":%x",answer[i]);
	
	printf(" #\n");
	return;
}

unsigned char * game::get_answer()
{
	/* vynulovani odpovedi */
	memset(answer,0,SERVER_MSG_LEN);

	unsigned char * retval = NULL;

	if(splitscreen)
	{
		/* zde se odpoved dvakrat prepisuje - je nutne precist data z obou socketu aby se nehromadila */
		if( pla1->life > 0)
			retval = pla1->recv_answer(answer);
		
		if( pla2->life > 0)
			retval = pla2->recv_answer(answer);
	}
	else
	{
		if( pla1->life > 0)
			retval = pla1->recv_answer(answer);
	}
	
	return (retval);
}

void game::create_objects(int sock)
{
	char i;		/* iteracni promenna */

	/* ziskani inicializacniho vektoru */
	char zn = VECTOR_REQUEST;
	if( send(sock,&zn,1,0) == -1 )
	{
		fprintf(stderr,"game::create_objects: ");
		perror("send");
		return;
	}

	unsigned char * buff = new unsigned char[INITBUF];
	unsigned char * buffer_to_delete = buff;
	
	if( recv(sock,buff,INITBUF,MSG_WAITALL) == -1 )
	{
		fprintf(stderr,"game::create_objects: ");
		perror("recv");
		return;
	}

	int * vector = new int[2*MAX_PLAYER_CNT]; 

	/* dekodovani inicializacniho vektoru */
	fprintf(stderr,"create_objects - init vector:");
	for(i=0; i < (2*MAX_PLAYER_CNT); ++i)
	{
		read_char_buff(buff,"i",(vector+i));
		fprintf(stderr," %d ",vector[i]);
		buff += sizeof(int);
	}
	fprintf(stderr,"\n");

	delete buffer_to_delete;

	/* alokace map */
	tanks = new tank_map;
	structures = new structure_map;
	
	/* naplneni map */

	tank_map::iterator tmit;
	structure_map::iterator smit;

	for ( i=0; i<player_cnt; ++i)
	{
		/* pridani tanku s id=i */
		tanks->insert( pair<char,tank>(i,tank(i)));
		/* nastaveni pozice prave pridaneho tanku */
		/* vector[FIRST] - x-ova souradnice zakladny
		 * vector[SECOND] - y-ova souradnice zakladny */ 
		
		tmit = tanks->find(i);
		(*tmit).second.set_position(vector[FIRST]+TANK_REL_X,vector[SECOND]+TANK_REL_Y);

		/* pridani zakladny */
		/* ma-li tank id==x, jeho zakladna ma id==MAX_PLAYER_CNT+x */
		structures->insert( pair<char,structure>(MAX_PLAYER_CNT+i,structure(MAX_PLAYER_CNT+i,vector[FIRST],vector[SECOND],STRUCTURE_SHAPE)));
	}

	delete vector;

	return;
}

/* vytvori mnozinu obsahujici vsechny souradnice ktere jsou aktualne "zabrany" nejakym tankem */
coord_set * game::make_pool()
{
	coord_set * res = new coord_set;
	tank_map_it tmit;
	coord_set_iter csit;
	collision_shape * ts;

	for(tmit = tanks->begin(); tmit != tanks->end(); ++tmit)
	{
		ts = (*tmit).second.get_shape();
		for(csit = ts->shape.begin(); csit != ts->shape.end(); ++csit)
			res->insert(coord(ts->x+(*csit).x,ts->y+(*csit).y));
	}

	return res;
}

void game::move_fire()
{
	if(!fire.empty())
	{
		coord_set * pool = make_pool();	/* aktualni souradnice vsech bodu, kde jsou tanky */
		retval control;	
		bullet_list_iter blit;

		tank_map_it tmit;	/* bude-li zasazen tank je treba zavolat get_hit() */

		blit = fire.begin();
		/* projdi vsechny strely */
		while( blit != fire.end() )
		{
			/* let jedne strely */
			do
			{
				/* kontrola kolize s mapou */
				switch(landscape->get_xy((*blit).get_pos()))
				{
					case STRUCTURE:
					case ROCK:
						(*blit).disapear();
					break;
					case MUD:
						(*blit).explode();
					default:
					break;
				}
				
				/* pokud doslo k narazu je ttl=0 */
				if((*blit).get_ttl() > 0)
				{
					/* pokus se pridat souradnice strely do pool
					 * pokud uz tam jsou, tak strela neco trefila */
					control = pool->insert((*blit).get_pos());
					if(control.second == false) /* false => trefa */
					{
						/* likvidace strely */
						(*blit).disapear();

						/* poniceni tanku */
						unsigned char zas = (*blit).which_tank();
						tmit = tanks->find(zas);
						(*tmit).second.get_hit(DAMAGE);
					}
					else	/* proti kolizi strel mezi sebou */
						pool->erase((*blit).get_pos());
				}	
			}
			while( (*blit).move() );

			++blit;
		}

		delete pool;
	}
}

/* prijme odpoved ve forme jednoho znaku na dotaz request*/
char game::get_char_answer(int sock,char request)
{
	char zn = request;

	if( send(sock,&zn,1,0) == -1)
	{
		fprintf(stderr,"get_char_answer (request: %c) :",request);
		perror("send");
	}
	else
	{
		if( recv(sock,&zn,1,MSG_WAITALL) == -1 )
		{
			fprintf(stderr,"get_char_answer (request: %c) :",request);
			perror("recv");
		}
	}

	return (zn);
}

void game::recover_tanks()
{
	tank_map_it tmit;
	structure_map_it smit;

	area * tank_area;
	area * stru_area;

	for(tmit = tanks->begin(); tmit != tanks->end(); ++tmit)
	{
		for(smit = structures->begin(); smit != structures->end(); ++smit)
		{
			tank_area = (*tmit).second.get_shape();
			stru_area = (*smit).second.get_area();
			
			/* kontrola pozice tanku vzhledem k bunkru */
			if( (*tank_area) % (*stru_area) )
				(*tmit).second.regenerate((*smit).second.id);
		}
	}
}

void game::show_status(camera * cam, tank & machine)
{
	SDL_Surface * status_bar;
}

/* callback pro timer */
Uint32 callback(Uint32 interval, void * param)
{
	SDL_Event ev;
	ev.type = SDL_USEREVENT;
	ev.user.code = TIMER_EVENT;
	ev.user.data1 = NULL;
	ev.user.data2 = NULL;
	
	SDL_PushEvent(&ev);
	return (interval);
}

