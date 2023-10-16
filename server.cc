#include "commondef.h"
#include "initvect.h"
#include "mirror.h"
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define QUEUE_SIZE 10

char * prog_name;

void print_help(FILE * stream, int exit_code)
{
	fprintf(stream,"Usage: %s -m mapfile [options]\n",prog_name);
	fprintf(stream,"mandatory argument:\n");
	fprintf(stream,"  -m mapfile		file containing game map\n");
	fprintf(stream,"list of available options:\n");
	fprintf(stream,"  -h --help		Display this help\n");
	fprintf(stream,"  -c player_cnt 	determine number of players ( number from [2..8] )\n");
	fprintf(stream,"  -l lives		starting amount of lives for each player ( positive number )\n");
	exit(exit_code);
}

/* generator "nahodnych" vektoru - ZJEDNODUSENA VERZE */
/*int * generate_init_vect(int mapfile)
{
	int * vect = new int[2*MAX_PLAYER_CNT];
	vect[0] = 1;
	vect[1] = 1;
	vect[2] = 22;
	vect[3] = 22;
	vect[4] = 43;
	vect[5] = 43;
	vect[6] = 64;
	vect[7] = 64;
	vect[8] = 85;
	vect[9] = 85;
	vect[10] = 106;
	vect[11] = 106;
	vect[12] = 127;
	vect[13] = 127;
	vect[14] = 0;
	vect[15] = 50;
	return (vect);
} */


char read_request(int sock,char id)
{
	char ret = REQUEST_ERROR;
	if( recv(sock,&ret,1,MSG_WAITALL) == -1 )
	{
		fprintf(stderr,"read_request from player %d: ",id);
		perror("recv");
	}

	return ret;
}

void respond_char(int sock, char respond,char request, char id)
{
	if(send(sock,&respond,1,0) == -1)
	{
		fprintf(stderr,"respond_char %d to player %d request %c: ",respond,id,request);
		perror("send");
	}
}

void respond_vector(int sock, int * vector, char id)
{
	unsigned char * buff = new unsigned char[INITBUF];
	unsigned char * original_buff = buff;
	
	int i;
	for(i=0; i < (2*MAX_PLAYER_CNT); ++i)
	{
		fill_char_buff(buff,"i",vector[i]);
		buff += sizeof(int);
	}

	buff = original_buff;
	if( send(sock,buff,INITBUF,MSG_WAITALL) == -1 )
	{
		fprintf(stderr,"respond_vector for player %d: ",id);
		perror("send");
	}

	delete buff;
}

void send_terminate_msg(int sock)
{
	unsigned char * buff = new unsigned char[SERVER_MSG_LEN];
	
	memset(buff,TERM_CHAR,SERVER_MSG_LEN);

	send(sock,buff,SERVER_MSG_LEN,MSG_WAITALL);

	delete buff;
}

int main(int argc, char ** argv)
{
	prog_name = argv[0];
	int mapfile = 0;

	/* default options */
	unsigned char player_cnt = 2;
	unsigned char life_cnt = 3;

	char 	i;			/* iteracni promenne */
	char	j;			/**/

	int loop_retval;		/* navratova hodnota hlavni smycky */

	int sock_in;			/* socket pro prijimani spojeni */
	int sock_new;			/* socket pro prijmuta spojeni */
	struct pollfd * players;	/* sockety pro pripojene hrace */
	struct sockaddr_in in_addr; 	/* adresa prijimaciho spojeni */
	struct sockaddr_in new_addr;	/* adresa prijmuteho spojeni */
	socklen_t new_size;		/* velikost prijmute adresy */
	
	/* parsovani parametru */
	const char * short_opt = "hc:l:m:";
	
	const struct option long_opt[] = {
		{"help",0,NULL,'h'},
		{"player-count",1,NULL,'c'},
		{"live-count",1,NULL,'l'},
		{"map-file",1,NULL,'m'},
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
			case 'c':
				player_cnt = atoi(optarg);
				fprintf(stderr,"Player count: %d\n",player_cnt);
			break;
			case 'l':
				life_cnt = atoi(optarg);
				fprintf(stderr,"Starting with %d lives.\n",life_cnt);
			break;
			case 'm':
				mapfile = open(optarg,O_RDONLY);
				if(mapfile == -1)
				{
					perror("open");
					print_help(stderr,1);
				}
				else
					fprintf(stderr,"Map file: %s\n",optarg);
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

	if( mapfile == 0 )
	{
		fprintf(stderr,"Map file must be specified!\n");
		print_help(stderr,1);
	}

	fprintf(stderr,"Starting server\n");

	/* alokace pole socketu */
	players = new struct pollfd[player_cnt];

	/* generovani init vektoru (rozmisteni hracu po mape) */
/*	int * initvect = generate_init_vect(0);  - ZJEDNODUSENA VERZE */
	int * initvect = generate_init_vect(mapfile,player_cnt);

	/* souboru s mapou uz neni treba */
	close(mapfile);

	fprintf(stderr,"Creating socket for incoming connections ..");
	if ( (sock_in = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		return (1);
	}
	fprintf(stderr,"OK\n");

	/* nastaveni adresy */
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(PORT);
	in_addr.sin_addr.s_addr = INADDR_ANY;
	memset(in_addr.sin_zero, '\0', sizeof in_addr.sin_zero);


	fprintf(stderr,"Binding socket..");
	if ( bind(sock_in,(struct sockaddr *)&(in_addr), sizeof in_addr) == -1 )
	{
		perror("bind");
		return (1);
	}
	fprintf(stderr,"OK\n");
	
	fprintf(stderr,"Trying to listen..");
	if ( listen(sock_in,QUEUE_SIZE) == -1)
	{
		perror("listen");
		return (1);
	}
	fprintf(stderr,"Listening on port %d\n",PORT);

	fprintf(stderr,"Catching incoming connections..\n");
	
	char zn;

	socklen_t * addr_len = new socklen_t;

	/* prijmuti spojeni */
	for ( i = 0; i < player_cnt; ++i )
	{
		*addr_len = sizeof(new_addr);

		fprintf(stderr,"#%d .. ",i);
		if ( (sock_new = accept(sock_in,(struct sockaddr*)&(new_addr),addr_len)) == -1 )
		{
			perror("accept");
			return (1);
		}
		fprintf(stderr,"accepted\n");

		/* ulozeni socketu */
		players[i].fd = sock_new;
		players[i].events = POLLIN;

		/* zodpovezeni dotazu klienta (ID,Init. vect.,player_cnt,life count) */
		zn = read_request(sock_new,i);

		while(zn != WAITING_FOR_START)
		{
			switch(zn)
			{
				case ID_REQUEST:
					respond_char(sock_new,i,ID_REQUEST,i);
				break;
				case PLAYER_CNT_REQUEST:
					respond_char(sock_new,player_cnt,PLAYER_CNT_REQUEST,i);
				break;
				case LIFE_CNT_REQUEST:
					respond_char(sock_new,life_cnt,LIFE_CNT_REQUEST,i);
				break;
				case VECTOR_REQUEST:
					respond_vector(sock_new,initvect,i);
				break;
				default:
					fprintf(stderr,"server: wrong signal caught\n");
			}

			zn = read_request(sock_new,i);
		}
	}

	/* initvect uz muzeme smazat */
	delete initvect;

	/* odeslani povelu ke startu vsem uspesne pripojenym hracum */
	unsigned char start_cmd = START_GAME;	

	for( j=0; j < player_cnt; ++j)
		if( send(players[j].fd,&start_cmd,1,0) == -1)
			fprintf(stderr,"Server error: .. send START to player %d.\n",j);
	
	/* hlavni smycka serveru */
	loop_retval = mirror_loop(player_cnt, players);
	switch(loop_retval)
	{
		case ALL_RIGHT:
			fprintf(stderr,"Game over!\n");
		break;
		case PROBLEM_RECIEVING:
			fprintf(stderr,"Problem with reading messages\n");
		break;
		case PROBLEM_SENDING:
			fprintf(stderr,"Problem with sending messages\n");
		break;
		default:
			fprintf(stderr,"UNKNOWN PROBLEM\n");
	}


	fprintf(stderr,"Closing sockets..\n");
	for(j=0;j<player_cnt; ++j)
		close(players[j].fd);
	
	fprintf(stderr,"Shutdown.\n");
	return (0);
}
