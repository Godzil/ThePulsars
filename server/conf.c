//
// Created by mlt on 29/01/18.
//

#include <conf.h>
#include <ipc.h>

#define CONF_DEFAULT_PORT (4242)
#define CONF_DEFAULT_MAP "./default.map"
#define CONF_DEFAULT_NBTURNS (420)
#define CONF_DEFAULT_TURN_MSEC (200)
#define DEF_TURN_TAKE_AKX (5)
#define DEF_TURN_UNTAKE_AKX (5)
#define DEF_TURN_TAKE_R4D2 (5)
#define DEF_TURN_UNTAKE_R4D2 (5)
#define DEF_R4D2_SPEED (2.0F)
#define DEF_AKX_SPEED (0.1F)
#define DEF_PULSE_POWER (1000.0F)
#define DEF_SEE_POWER (1.0F)
#define DEF_PULSE_COEF (1.0F)
#define DEF_SIZE_X (1)
#define DEF_SIZE_Y (1)
#define DEF_DESTROY_SPEED (-1.0F)

conf_t *gl_config;

conf_t *conf_set_default(conf_t *res)
{
    res->verbose = False;
    res->quiet = False;
    res->gfx = False;
    res->map_name = CONF_DEFAULT_MAP;
    res->nb_turns = CONF_DEFAULT_NBTURNS;
    res->turn_msec = CONF_DEFAULT_TURN_MSEC;
    res->turn_take_akx = DEF_TURN_TAKE_AKX;
    res->turn_untake_akx = DEF_TURN_UNTAKE_AKX;
    res->turn_take_r4d2 = DEF_TURN_TAKE_R4D2;
    res->turn_untake_r4d2 = DEF_TURN_UNTAKE_R4D2;
    res->r4d2_speed = DEF_R4D2_SPEED;
    res->akx_speed = DEF_AKX_SPEED;
    res->pulse_power = DEF_PULSE_POWER;
    res->see_power = DEF_SEE_POWER;
    res->destroy_speed = DEF_DESTROY_SPEED;
    res->pulse_coef = DEF_PULSE_COEF;
    res->nb_akx = res->nb_r4d2 = res->nb_objects = 0;
    res->size_x = DEF_SIZE_X;
    res->size_y = DEF_SIZE_Y;
    res->players = NULL;
    res->nb_players = 0;
    res->turn_num = 0;
    res->sand_file = NULL;
    res->port = CONF_DEFAULT_PORT;
    res->pub = True;
    return res;
}