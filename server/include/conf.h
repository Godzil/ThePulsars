#ifndef __PROLO_CONF_H__
#define __PROLO_CONF_H__

#include <sys/types.h>


#include "network.h"

typedef struct _conf_t
{
    int nb_turns;
    boolean_t verbose;
    boolean_t quiet;
    boolean_t gfx, pub;
    gfx_t *gfx_clients;
    const char *map_name;
    GSList *players;
    int nb_objects;
    int turn_num;
    pid_t messager_pid;
    int port;

    int nb_akx;
    int nb_r4d2;
    int turn_msec;
    int nb_players;

    int turn_take_akx;
    int turn_untake_akx;
    int turn_take_r4d2;
    int turn_untake_r4d2;
    float r4d2_speed;
    float akx_speed;
    float pulse_power;
    float see_power;
    float pulse_coef;
    float destroy_speed;
    float size_x;
    float size_y;
    char *sand_file;
    sigset_t *mask;
} conf_t;

extern conf_t *gl_config;

conf_t *conf_set_default(conf_t *res);

#endif
