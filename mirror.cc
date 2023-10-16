#include "mirror.h"
#include "commondef.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <set>

using namespace std;

/* mnozina id od kterych byla prijmuta zprava (a kterym se ma poslat) */
set<char>	id_set;
set<char>	dead_set;
typedef pair<set<char>::iterator,bool> retval;
typedef set<char>::iterator set_it;

/* buffer na ulozeni prijmutych zprav */
unsigned char answer[SERVER_MSG_LEN];

/* globalni promenna pro pocet hracu (pouze v mirror.cc) */
int plac;

bool fill_answer(int active_cnt,struct pollfd * players)
{
	retval insert_retval;
	set_it it;
	int result;
	unsigned char * act_pos = answer;

	int i;

	while(id_set.size() != active_cnt)
	{
		assert(id_set.size() < active_cnt);

		result = poll(players,plac,0);

		assert(result >= 0);
		
		if(result > 0)
		{
			for(i=0; i<plac; ++i)
			{
				/* mrtve hrace neobsluhujeme */
				it = dead_set.find(i);
				if(it != dead_set.end())
					continue;

				if(players[i].revents == POLLIN)
				{
					/* zkus vlozit id */
					insert_retval = id_set.insert(i);
					if(insert_retval.second)
					{
						if(recv(players[i].fd,act_pos,CLIENT_MSG_LEN,0) == -1)
						{
							fprintf(stderr,"fill_answer: ");
							perror("recv");
						}

						players[i].events = POLLOUT;
						players[i].revents = 0;
						act_pos += CLIENT_MSG_LEN;
						assert(act_pos - answer < SERVER_MSG_LEN);
					}
				}
			}
		}
	}

	return (true);
}

bool send_answer(struct pollfd * players)
{
	char i = 0;
	int result;

	result = poll(players,plac,0);
	
	if(result == -1)
	{
		fprintf(stderr,"send_answer:");
		perror("poll");
	}

	/* dokud neni poslana odpoved vsem kdo poslali zpravu */
	while(id_set.size() > 0)
	{
		if(players[i].revents == POLLOUT)
		{
			/* posli zpravu */
			if(send(players[i].fd,answer,SERVER_MSG_LEN,0) == -1)
			{
				fprintf(stderr,"send_answer:");
				perror("send");
			}
			
			/* vymaz id z mnoziny */
			if(id_set.erase(i) == 0)
				fprintf(stderr,"send_answer: id not erased\n");

			/* zmen udalost filedescriptoru */
			players[i].events = POLLIN;
			players[i].revents = 0;
		}

		++i;
		if(i >= plac)
		{
			i = 0;

			result = poll(players,plac,0);
			
			if(result == -1)
			{
				fprintf(stderr,"send_answer:");
				perror("poll");
			}
		}
	}
	
	return (true);
}

bool is_alive(unsigned char bitmask)
{
	return (bitmask & ALIVE);
}

int count_dead()
{
	int res = 0;

	size_t i;

	for(i = 0; i < plac; ++i)
	{
		if( !is_alive(answer[i*2+1]) )
		{
			++res;
			dead_set.insert(answer[i*2]);
		}
	}

	return (res);
}

int mirror_loop(int player_cnt, struct pollfd * players)
{
	int dead_cnt = 0;
	plac = player_cnt;

	while(player_cnt > dead_cnt)
	{
		/* vynulovani bufferu */
		memset(answer,MAX_PLAYER_CNT,SERVER_MSG_LEN);

		/* naplneni bufferu zpravami od jednotlivych hracu */
		if(!fill_answer(player_cnt-dead_cnt,players))
			return (PROBLEM_RECIEVING);

		/* odeslani odpovedi vsem zucastnenym */
		if(!send_answer(players))
			return (PROBLEM_SENDING);

		print_answer(answer);

		/* spocitani vyrazenych hracu na zaklade odeslane odpovedi */
		dead_cnt = count_dead();
	}

	return (ALL_RIGHT);
}
