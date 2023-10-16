#include "player.h"
#include "commondef.h"

player::player(int socket,char init_id,int init_life,struct controls & settings )
{
	sock = socket;
	
	id = init_id;
	life = init_life;

	tank_state = ALIVE;

	/* inicializace ovladani */
	user_input.up = settings.up;
	user_input.down = settings.down;
	user_input.left = settings.left;
	user_input.right = settings.right;
	user_input.fire = settings.fire;
}

void player::confirm_ok()
{
	char zn = WAITING_FOR_START;
	if( send(sock,&zn,1,0) == -1)
	{
		fprintf(stderr,"player::confirm_ok: ");
		perror("send");
	}
}

bool player::wait_for_start()
{
	char zn;
	if( recv(sock,&zn,1,MSG_WAITALL) == -1)
	{
		fprintf(stderr,"player::wait_for_start: ");
		perror("recv");
	}

	if( zn == START_GAME)
		return (true);
	else
		return (false);
}

SDLKey player::process_keypress(SDL_KeyboardEvent & ke,bool press)
{
	/* jedna-li se o stisk klavesy, je treba zkontrolovat zda je uz bit nastaven aby se nevynuloval */
	/* pokud se jedna o uvolneni klavesy, je treba overit, ze bit je nastaven */
	if(press)
	{
		if(ke.keysym.sym == user_input.up && !is_bit_set(UP))
			tank_state ^= UP;
		else
		if(ke.keysym.sym == user_input.down && !is_bit_set(DOWN))
			tank_state ^= DOWN;
		else
		if(ke.keysym.sym == user_input.left && !is_bit_set(LEFT))
			tank_state ^= LEFT;
		else
		if(ke.keysym.sym == user_input.right && !is_bit_set(RIGHT))
			tank_state ^= RIGHT;
		else
		if(ke.keysym.sym == user_input.fire && !is_bit_set(FIRE))
		{
			tank_state ^= FIRE;
		}
	}
	else
	{
		if(ke.keysym.sym == user_input.up && is_bit_set(UP))
			tank_state ^= UP;
		else
		if(ke.keysym.sym == user_input.down && is_bit_set(DOWN))
			tank_state ^= DOWN;
		else
		if(ke.keysym.sym == user_input.left && is_bit_set(LEFT))
			tank_state ^= LEFT;
		else
		if(ke.keysym.sym == user_input.right && is_bit_set(RIGHT))
			tank_state ^= RIGHT;
		else
		if(ke.keysym.sym == user_input.fire && is_bit_set(FIRE))
		{
			tank_state ^= FIRE;
		}
	}

	return (ke.keysym.sym);
}

void player::send_msg()
{
	if(life <= 0)
	{
		fprintf(stderr,"Player #%d : Not enough lives(send_msg)\n",id);
		return;
	}
	
	unsigned char msg[CLIENT_MSG_LEN];

	tank_map::iterator tmit;

	tmit = tanks->find(id);

	if( tmit != tanks->end())
	{

		if(life > 1)
		{
			/* pokud ma hrac vic nez jeden zivot, posila se v pripade zasahu pozadavek na resurekci */
			if( ((*tmit).second.armor <= 0) || ((*tmit).second.energy <= 0) )
				tank_state = RESURECT | ALIVE;
		}
		else
		{
			/* pokud ma hrac uz jen posledni zivot, posila se v pripade zasahu parte */
			if( ((*tmit).second.armor <= 0) || ((*tmit).second.energy <= 0) )
				tank_state = DEAD;
		}
		
		fill_char_buff(msg,"cc",id,tank_state);
		
		/* odeslani vlastniho stavu */
		if( send(sock,msg,CLIENT_MSG_LEN,MSG_DONTWAIT) == -1 )
		{
			fprintf(stderr,"player::send_msg: ");
			perror("send");
		}
	}
}

bool player::kill()
{
	if(life <= 0)
	{
		fprintf(stderr,"#%d GAME OVER\n",id);
		return (false);
	}

	unsigned char msg[CLIENT_MSG_LEN];
	
	/* nastaveni stavu tanku */
	tank_state = DEAD;
	
	tank_map::iterator tmit;

	tmit = tanks->find(id);

	if( tmit != tanks->end())
	{
		fill_char_buff(msg,"cc",id,tank_state);

		/* odeslani stavu */
		if( send(sock,msg,CLIENT_MSG_LEN,0) == -1 )
		{
			fprintf(stderr,"player::kill: ");
			perror("send");
		}

		fprintf(stderr,"Player #%d - leaving game\n",id);
		return (true);
	}

	return (false);
}

unsigned char * player::recv_answer(unsigned char * ret)
{
	if(life <= 0)
	{
		fprintf(stderr,"Player #%d : Not enough lives (recv_answer)\n",id);
		return (NULL);
	}

	/* prijmuti odpovedi od serveru */
	if( recv(sock,ret,SERVER_MSG_LEN,MSG_WAITALL) == -1 )
	{
		fprintf(stderr,"player::recv_answer: ");
		perror("recv");
		return (NULL);
	}
	
	return ret;
}

void player::reset_tank_state()
{
	tank_state = ALIVE;
	/* odecteni zivota */
	life -= 1;
	fprintf(stderr,"ID %d has %d lives left\n",id,life);
}

bool player::is_bit_set(char mask)
{
	return (mask & tank_state);
}
