#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "game.h" 
/* globalni promenne */
SDL_Surface * screen;
tmap * landscape;

tank_map	* tanks;			/* mapa tanku - klicem je id */ 
structure_map 	* structures;			/* mapa struktur - klicem je id */
char 		  player_cnt;			/* pocet hracu */

char * prog_name;

void print_help(FILE * stream, int exit_code)
{
	fprintf(stream,"Usage: %s -a IP_addr -m map_file\n",prog_name);
	fprintf(stream,"  -h --help		Display this help\n");
	fprintf(stream,"  -m map_file		use map_file as game map\n");
	fprintf(stream,"  -s --splitscreen	run in splitscreen mode\n");
	fprintf(stream,"  -a IP			connect to server on IP\n");
	exit(exit_code);
}

int main(int argc, char ** argv)
{
	prog_name = argv[0];

	bool splitscreen = false;
	struct in_addr * server_ip = NULL;
	int map_file = -1;

	/* parsovani parametru */
	const char * short_opt = "hsa:m:";
	
	const struct option long_opt[] = {
		{"help",0,NULL,'h'},
		{"splitscreen",0,NULL,'s'},
		{"addr",1,NULL,'a'},
		{"map",1,NULL,'m'},
		{NULL,0,NULL,0}
	};

	int next_opt;

	do {
		next_opt = getopt_long(argc,argv,short_opt,long_opt,NULL);

		switch(next_opt)
		{
			case 'h':
				print_help(stdout,0);
			break;
			case 's':
				splitscreen = true;
				fprintf(stderr,"Splitscreen mode.\n");
			break;
			case 'a':
				server_ip = new struct in_addr;
				inet_aton(optarg,server_ip);
			break;
			case 'm':
				map_file = open(optarg,O_RDONLY);
				if(map_file == -1)
				{
					fprintf(stderr,"Fatal error: Can't open %s\n",optarg);
					exit(1);
				}
			break;
			case '?':
				print_help(stderr,1);
			break;
			case -1: /* konec */
			break;
			default:
				abort();
		}
	} while(next_opt != -1);

	if( server_ip == NULL )
	{
		fprintf(stderr,"Server IP address must be specified.\n");
		exit(1);
	}

	if( map_file == -1 )
	{
		fprintf(stderr,"Map file must be specified.\n");
		exit(1);
	}

	fprintf(stderr,"server addr: %s\n",inet_ntoa(*server_ip));

	/* pripojeni k serveru */

	int sock1;
	int sock2;
	
	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = server_ip->s_addr;
	memset(server_addr.sin_zero,'\0',sizeof(server_addr.sin_zero));

	/* vytvoreni a pripojeni socketu pro prvniho hrace */
	if( (sock1 = socket(PF_INET,SOCK_STREAM,0)) == -1)
		{
			fprintf(stderr,"First player socket:\n");
			perror("socket");
			exit(1);
		}
	
	if( (connect(sock1,(struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)
		{
			fprintf(stderr,"First player socket:\n");
			perror("connect");
			exit(1);
		}

	if (splitscreen)
	{
		/* vytvoreni a pripojeni socketu pro druheho hrace */
		if( (sock2 = socket(PF_INET,SOCK_STREAM,0)) == -1 )
			{
				fprintf(stderr,"Second player socket:\n");
				perror("socket");
				exit(1);
			}
		
		if( (connect(sock2,(struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)
			{
				fprintf(stderr,"Second player socket:\n");
				perror("connect");
				exit(1);
			}
	}

	/* vytvoreni  hry */
	game act_game(splitscreen,map_file);
	
	if (splitscreen)
		act_game.start(sock1,sock2);
	else
		act_game.start(sock1);

	close(sock1);
	close(sock2);
	return (0);
}
