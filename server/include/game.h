#ifndef THE_PULSAR_GAME_H
#define THE_PULSAR_GAME_H

#include "conf.h"
#include "player.h"

void game_launch(conf_t *conf);

void start_client(player_t *player, int turn_num);

#endif
