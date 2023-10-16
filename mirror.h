#ifndef _MIRROR_H
#define _MIRROR_H

#define ALL_RIGHT 0
#define PROBLEM_RECIEVING 1
#define PROBLEM_SENDING 2

#include <poll.h>

/* smycka ktera shromazduje zpravy od vsech aktivnich hracu a rozesila zkompletovanou odpoved */
int mirror_loop(int player_cnt, struct pollfd * players);

#endif
