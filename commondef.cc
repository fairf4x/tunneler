#include "commondef.h"
/*
int my_read(int file,unsigned char * buff,size_t nbyte)
{
	int i;
	size_t readed;
	for(i=0;i<nbyte;i += readed)
	{
		if((readed = read(file,buff,nbyte-i)) == -1)
			perror("read");
		if(readed == 0)
			return i;
	}

	return nbyte;
}

int my_write(int file,unsigned char * buff,size_t nbyte)
{
	int i;
	size_t writed;
	for(i=0;i<nbyte;i += writed)
	{
		if((writed = write(file,buff,nbyte-i))== -1)
			perror("write");
		if(writed == 0)
			return i;
	}

	return nbyte;
}
*/
/* ulozi 32 bitovy int do bufferu */
void packi32(unsigned char *buf, unsigned long i)
{
	    *buf++ = i>>24; *buf++ = i>>16;
	        *buf++ = i>>8;  *buf++ = i;
}

/* vybali 32 bitovy int z bufferu */
unsigned long unpacki32(unsigned char *buf)
{
	return ((buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3]);
}

/* format:
 * 'i' - 32bit integer
 * 'c' - 8bit char
 * */
size_t fill_char_buff(unsigned char * buff,const char * format,...)
{
	size_t size = 0;
	unsigned long i_var;
	char c_var;
	va_list ap;

	va_start(ap,format);

	while(*format != '\0')
	{
		switch(*format)
		{
			case 'i':
				size += sizeof(unsigned long);
				i_var = htonl(va_arg(ap, unsigned long));
				packi32(buff,i_var);
				buff += sizeof(unsigned long);
			break;
			case 'c':
				size += sizeof(char);
				c_var = va_arg(ap,int);
				*buff = c_var;
				buff += sizeof(char);
			break;
			default:
				fprintf(stderr,"Unknown format character");
			break;
		}

		++format;
	}

	va_end(ap);

	return size;
}

void read_char_buff(unsigned char * buff,const char * format,...)
{
	unsigned long * i_var;
	char * c_var;

	va_list ap;

	va_start(ap,format);

	while( *format != '\0' )
	{
		switch(*format)
		{
			case 'i':
				i_var = va_arg(ap,unsigned long *);	
				*i_var = ntohl(unpacki32(buff));
				buff += sizeof(unsigned long);
			break;
			case 'c':
				c_var =(char*)va_arg(ap,int*);
				*c_var = ntohs((*buff)<<8);
				buff += sizeof(char);
			break;
			default:
				fprintf(stderr,"Unknown format character");
			break;
		}

		++format;
	}
	
	va_end(ap);
}

void print_answer(unsigned char * buff)
{
	int i;
	for(i=0; i < MAX_PLAYER_CNT*2; ++i)
		printf(":%x",buff[i]);
	
	printf(" #\n");
	return;
}

