/* $Id: plugins.h,v 1.12 2001/05/05 17:58:10 kilobug Exp $ */

#ifndef __PROLO_PLUGINS_H__
#define __PROLO_PLUGINS_H__

#include "conf.h"

typedef void (prolo_callback_t)(int);

typedef void (prolo_callback2_t)(int, int);

typedef struct _player_t
{
    const char *name;
    prolo_callback2_t *init;
    prolo_callback_t *new_turn, *turn_akx, *turn_r4d2;
    float score;
    int team_id;
    int time_turn;
    int last_time;
    boolean_t on;
    GModule *handle;
    int error;
    int new;
    pid_t pid;
    int pipe_read;
    int pipe_write;
    boolean_t alive;
} player_t;

player_t *new_player(const char *libname, const char *name, int id,
                     conf_t *conf);

void destroy_player(player_t *player);

void player_init(player_t *p, int team_id);

void player_new_turn(player_t *p, int turn_num);

void player_turn_akx(player_t *p, int akx_id);

void player_turn_r4d2(player_t *p, int r4d2_id);

#endif
