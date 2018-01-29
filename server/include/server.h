/* $Id: server.h,v 1.22 2001/04/14 16:50:48 kilobug Exp $ */

#ifndef __PROLO_SERVER_H__
#define __PROLO_SERVER_H__

#include <plugins.h>
#include <conf.h>

extern player_t *gl_player;

/* Initialization functions */
conf_t *init(int argc, char **argv);
void launch_game(conf_t *conf);
void load_map(conf_t *conf);
void start_client(player_t *player, int turn_num);

/* Time functions */
void start_timer();
void stop_timer();
int time_left();
void abort();

#endif
