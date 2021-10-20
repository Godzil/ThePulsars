#ifndef __PROLO_GAME_H__
#define __PROLO_GAME_H__

#include "conf.h"
#include "player.h"

void game_launch(conf_t *conf);

void start_client(player_t *player, int turn_num);

#endif
