/* $Id: load_map.c,v 1.19 2001/04/29 17:35:16 kilobug Exp $ */

#include "client.h"

void get_object_param(int fd, obj_t *obj, conf_t *conf)
{
  obj->posx = net_get_float(conf->socket);
  obj->posy = net_get_float(conf->socket);
  obj->team_id = net_get_int(conf->socket);
  if (obj->type == obj_akx)
    {
      obj->action = net_get_int(fd);
      switch (obj->action)
	{
	case act_akx_move:
	  break;
	case act_akx_pulse:
	  obj->angle = net_get_float(fd);
	  obj->tx = net_get_float(fd);
	  obj->ty = net_get_float(fd);
	  break;
	case act_akx_link:
	  obj->target = net_get_int(fd);
	  break;
	default:
	  protocol_error(conf, "invalid action");
	  return;
	}
    } 
}

static player_t *get_player(conf_t *conf)
{
  player_t *res;

  if (!net_chk_string(conf->socket, "PLR"))
    {
      protocol_error(conf, "PLR exepected");
      return NULL;      
    }
  res = g_malloc(sizeof(*res));
  res->id = net_get_int(conf->socket);
  if ((res->id > conf->map->nb_players) || (res->id < 1))
    {
      protocol_error(conf, "player id invalid");
      g_free(res);
      return NULL;      
    } 
  res->name = net_get_string(conf->socket);
  res->score = net_get_float(conf->socket);
  gen_colors(res->id, conf->map->nb_players + 1, res);  
  res->akx = tint_picture(conf->akx, res->colors);
  res->r4d2 = tint_picture(conf->r4d2, res->colors);
  return res;
}

static gboolean get_players(conf_t *conf)
{  
  int i;
  player_t *player;

  conf->map->players = g_malloc(sizeof(*(conf->map->players)) * 
				(conf->map->nb_players + 1));
  for (i = 0; i < conf->map->nb_players; i++)
    conf->map->players[i + 1] = NULL;
  for (i = 0; i < conf->map->nb_players; i++)
    {
      player = get_player(conf);
      if (player == NULL)
	return FALSE;
      conf->map->players[player->id] = player;
    }
  net_send_ack(conf->file);
  conf->map->players[0] = g_malloc(sizeof (*conf->map->players[0]));
  conf->map->players[0]->id = 0;
  conf->map->players[0]->name = "Sauvage";
  conf->map->players[0]->score = 0.0;
  conf->map->players[0]->akx = load_picture("akx_0.xpm", 
					    "akx_0_mask.xpm");
  conf->map->players[0]->r4d2 = load_picture("r4d2_0.xpm", 
					     "r4d2_0_mask.xpm");
  gen_colors(0, conf->map->nb_players + 1, conf->map->players[0]);
  player_list_make(conf);
  stats_make(conf);
  return TRUE;
}

static obj_t *get_object(conf_t *conf)
{
  obj_t *res;

  if (!net_chk_string(conf->socket, "OBJ"))
    {
      protocol_error(conf, "OBJ expected");
      return NULL;      
    }
  res = g_malloc(sizeof(*res));
  res->changed = TRUE;
  res->id = net_get_int(conf->socket);
  if ((res->id >= conf->map->nb_objects) || (res->id < 0))
    {
      protocol_error(conf, "object id invalid");
      g_free(res);
      return NULL;
    }
  res->type = net_get_int(conf->socket);
  if ((res->type != obj_r4d2) && (res->type != obj_akx))
    {
      protocol_error(conf, "object type invalid");
      g_free(res);
      return NULL;
    }
  get_object_param(conf->socket, res, conf);
  return res;
}

static gboolean get_objects(conf_t *conf)
{
  int i;
  obj_t *obj;

  conf->map->objects = g_malloc(sizeof(*(conf->map->objects)) * 
				conf->map->nb_objects);
  for (i = 0; i < conf->map->nb_objects; i++)
    conf->map->objects[i] = NULL;
  for (i = 0; i < conf->map->nb_objects; i++)
    {
      obj = get_object(conf);
      if (obj == NULL)
	return FALSE;
      conf->map->objects[obj->id] = obj;
    }
  net_send_ack(conf->file);
  return TRUE;
}

void load_map(conf_t *conf)
{
  map_t *map;

  conf->ready = FALSE;
  destroy_map(conf);

  map = g_malloc(sizeof(*map));
  conf->map = map;
  map->sizex = net_get_float(conf->socket);
  map->sizey = net_get_float(conf->socket);
  map->nb_turns = net_get_int(conf->socket);
  map->turn_num = net_get_int(conf->socket);
  map->nb_players = net_get_int(conf->socket);
  map->nb_r4d2 = net_get_int(conf->socket);
  map->nb_akx = net_get_int(conf->socket);
  map->sand_file = net_get_string(conf->socket);
  if (map->sand_file[0] == 0)
    map->background = NULL;
  else
    map->background = load_picture(map->sand_file, NULL);
  map->nb_objects = map->nb_r4d2 + map->nb_akx;
  map->players = NULL;
  map->objects = NULL;
  net_send_ack(conf->file);

  if (!get_players(conf))
    {
      destroy_map(conf);
      return;
    }
  if (!get_objects(conf))
    {
      destroy_map(conf);
      return;
    }
  conf->ready = TRUE;  
  scroll_bigmap(conf, -4242, -4242);
  stats_make(conf);
}
