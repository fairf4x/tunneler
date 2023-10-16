#ifndef _COMMONDEF_H
#define _COMMONDEF_H

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>

/* definice delek zprav */
#define CLIENT_MSG_LEN 2
#define MAX_PLAYER_CNT 8	 
#define SERVER_MSG_LEN CLIENT_MSG_LEN*MAX_PLAYER_CNT

#define INITBUF 64	/* 16*int = 16*4 = 64B */

#define PORT 3100

/* signalni znaky pro komunikaci mezi serverem a klientem */
#define WAITING_FOR_START 'W'
#define START_GAME 'S'
#define ID_REQUEST 'I'
#define PLAYER_CNT_REQUEST 'C'
#define LIFE_CNT_REQUEST 'L'
#define VECTOR_REQUEST 'V'
#define REQUEST_ERROR 'E'
#define TERM_CHAR	(0xff)	/* vyplneni zpravy 1 */

/* definice stavu tanku */
#define RESURECT	0x40	/* 0100 0000 */
#define ALIVE		0x20	/* 0010 0000 */
#define FIRE		0x10	/* 0001 0000 */
#define DEAD		0x00	/* 0000 0000 */

/*int my_read(int file,unsigned char * buff,size_t nbyte);
int my_write(int file,unsigned char * buff,size_t nbyte); */

/* ulozi 32 bitovy int do bufferu */
void packi32(unsigned char *buf, unsigned long i);

/* vybali 32 bitovy int z bufferu */
unsigned long unpacki32(unsigned char *buf);

/* format:
 * 'i' - 32bit integer
 * 'c' - 8bit char
 * */
size_t fill_char_buff(unsigned char * buff,const char * format,...);

void read_char_buff(unsigned char * buff,const char * format,...);

void print_answer(unsigned char * buffer);

#endif
