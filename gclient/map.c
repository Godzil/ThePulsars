#include "client.h"

static void destroy_player(player_t *player)
{
    if ( player == NULL )
    {
        return;
    }
    destroy_picture(player->akx);
    destroy_picture(player->r4d2);
    g_free(player->gdkcol);
    g_free(player);
}

void destroy_map(conf_t *conf)
{
    int i;

    conf->ready = FALSE;
    if ( conf->map == NULL )
    {
        return;
    }
    if ( conf->map->players != NULL )
    {
        for ( i = 0 ; i <= conf->map->nb_players ; i++ )
        {
            destroy_player(conf->map->players[i]);
        }
        g_free(conf->map->players);
    }
    if ( conf->map->objects != NULL )
    {
        for ( i = 0 ; i < conf->map->nb_objects ; i++ )
        {
            if ( conf->map->objects[i] != NULL )
            {
                g_free(conf->map->objects[i]);
            }
        }
        g_free(conf->map->objects);
    }
    g_free(conf->map->sand_file);
    destroy_picture(conf->map->background);
    g_free(conf->map);
    conf->map = NULL;
}

static void update_players(conf_t *conf)
{
    int id, i;

    while (( i = net_chk_str_list(conf->socket, "UPL", "END", NULL)) == 0 )
    {
        id = net_get_int(conf->socket);
        conf->map->players[id]->score = net_get_float(conf->socket);
    }
    if ( i < 0 )
    {
        protocol_error(conf, "UPL or END expected");
    }
    net_send_ack(conf->file);
    player_list_refresh(conf);
    stats_refresh(conf);
}

static void update_objects(conf_t *conf)
{
    int id, i;

    while (( i = net_chk_str_list(conf->socket, "UOB", "END", NULL)) == 0 )
    {
        id = net_get_int(conf->socket);
        get_object_param(conf->socket, conf->map->objects[id], conf);
        conf->map->objects[id]->changed = TRUE;
    }
    if ( i < 0 )
    {
        protocol_error(conf, "UOB or END expected");
    }
    net_send_ack(conf->file);
}

static void update_map(conf_t *conf)
{
    int i = 0;

    conf->ready = FALSE;
    i = net_chk_str_list(conf->socket, "TRN", "BYE", NULL);
    if ( i == -1 )
    {
        protocol_error(conf, "TRN expected");
        return;
    }
    if ( i == 1 )
    {
        net_send_ack(conf->file);
        close_socket(conf);
        set_status(conf, "Match termin�.");
        return;
    }
    i = net_get_int(conf->socket);
    conf->map->turn_num = CLAMP(i, 0, conf->map->nb_turns);
    net_send_ack(conf->file);
    update_players(conf);
    for ( i = 0 ; i < conf->map->nb_objects ; i++ )
    {
        conf->map->objects[i]->changed = FALSE;
    }
    update_objects(conf);
    conf->ready = TRUE;
    gamearea_refresh(conf->main, conf);
    minimap_refresh(conf->mini, conf);
    if (( conf->debug ) && ( conf->turn.mode == tt_skip ))
    {
        conf->turn.st_turns--;
        if ( !conf->turn.st_turns )
        {
            conf->turn.mode = tt_pause;
        }
        allow_new_turn(conf);
    }
    if (( conf->debug ) && ( conf->turn.mode == tt_fwd ))
    {
        conf->turn.gtkto = gtk_timeout_add(conf->turn.spd, tb_new_turn, conf);
    }
}

void update_map_callback(void *conf, gint src, GdkInputCondition cond)
{
    update_map(conf);
}
