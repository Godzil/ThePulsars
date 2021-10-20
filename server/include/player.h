#ifndef THE_PULSAR_PLUGINS_H
#define THE_PULSAR_PLUGINS_H

#include "conf.h"

typedef struct player_t
{
    const char *name;

    /* Function exported by the player module */
    void (*init)(int team, int nbPlayers);

    void (*new_turn)(int turn);

    void (*akx_turn)(int id);

    void (*r4d2_turn)(int id);

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

extern player_t *glbPlayer;

player_t *player_new(const char *libname, const char *name, int id, conf_t *conf);

void player_destroy(player_t *player);

void player_init(player_t *p, int team_id);

void player_new_turn(player_t *p, int turn_num);

void player_turn_akx(player_t *p, int akx_id);

void player_turn_r4d2(player_t *p, int r4d2_id);

#endif
