/* $Id: network.c,v 1.11 2001/04/28 15:29:25 kilobug Exp $ */

#include "server.h"

void gfx_trash_client(net_type_t get, net_type_t wanted, void *data)
{
  gfx_client_t *cl;

  cl = data;
  fprintf(stderr, "Network warning: reading %s, wanted %s.\n",
	  net_type_name(get), net_type_name(wanted));  
  close(cl->sock);
  cl->alive = FALSE;
}

gboolean gfx_get_ack(gfx_client_t *cl)
{
  gfx_t *gfx;

  fflush(cl->wsock);
  gfx = gl_config->gfx_clients;
  if (!net_wait_ack(cl->sock))
    {
      gfx_trash_client(net_type_none, net_type_int, cl);
      return FALSE;
    }
  return TRUE;
}

void gfx_write_object(FILE *file, item_t *obj)
{
  net_wr_float(file, obj->obj.x);
  net_wr_float(file, obj->obj.y);
  net_wr_int(file, obj->obj.team_id);
  if (obj->obj.type == obj_akx)
    {
      net_wr_int(file, obj->akx.action.type);
      switch (obj->akx.action.type)
	{
	case act_akx_move:
	  break;
	case act_akx_pulse:
	  net_wr_float(file, obj->akx.action.act.pulse.angle);
	  net_wr_float(file, obj->akx.action.act.pulse.x);
	  net_wr_float(file, obj->akx.action.act.pulse.y);
	  break;
	case act_akx_link:
	  net_wr_int(file, obj->akx.action.act.link.target_id);
	  break;
	}
    }
}

static gboolean gfx_do_send_updates(void *data, void *foo)
{
  GSList *l;
  player_t *play;
  item_t *obj;
  int i;
  gfx_client_t *cl;

  cl = data;
  if (cl == NULL)
    return TRUE;
  if (!cl->alive)
    return TRUE;
  net_set_error_handler(gfx_trash_client, cl);
  net_wr_string(cl->wsock, "TRN");
  net_wr_int(cl->wsock, gl_config->turn_num);
  if (!gfx_get_ack(cl))
    return FALSE;
  for (l = gl_config->players; l != NULL; l = l->next)
    {
      net_wr_string(cl->wsock, "UPL");
      play = l->data;
      net_wr_int(cl->wsock, play->team_id);
      net_wr_float(cl->wsock, play->score);
    }
  net_wr_string(cl->wsock, "END");
  if (!gfx_get_ack(cl))
    return FALSE;
  for (i = 0; i < gl_config->nb_objects; i++)
    if (gl_objects[i].obj.change)
      {
	net_wr_string(cl->wsock, "UOB");
	obj = gl_objects + i;
	net_wr_int(cl->wsock, obj->obj.id);
	gfx_write_object(cl->wsock, obj);
      }
  net_wr_string(cl->wsock, "END");
  if (!gfx_get_ack(cl))
    return FALSE;
  return TRUE;
}

static void gfx_send_updates(void *data, void *foo)
{
  gfx_client_t *cl;

  if (!gfx_do_send_updates(data, foo))
    {
      cl = data;
      cl->alive = FALSE;
    }
}

static void gfx_send_goodbye(void *data, void *foo)
{
  gfx_client_t *cl;

  cl = data;
  net_wr_string(cl->wsock, "BYE");
  fflush(cl->wsock);
  net_wait_ack(cl->sock);
}

void gfx_client_new_turn(gboolean last)
{
  gfx_t *gfx;
  int i;

  if (!gl_config->gfx)
    return;
  gfx = gl_config->gfx_clients;
  i = net_chk_str_list(gfx->master, "GO", "END", NULL);
  if (i == -1)
    {
      fprintf(stderr, "Protocol error.\n");
      exit(0);
    }
  net_send_ack(gfx->wmaster);  
  net_set_error_handler(NULL, NULL);
  if ((i == 0) && (!last))
    {
      while (gfx_new_client(gfx->sock, FALSE));
      g_slist_foreach(gfx->clients, gfx_send_updates, NULL);
    }
  else
    {
      g_slist_foreach(gfx->clients, gfx_send_goodbye, NULL);
      exit(0);
    }
}

static void gfx_clean()
{
  close(gl_config->gfx_clients->sock);
}

void gfx_init()
{
  if (!gl_config->gfx)
    {
      return;
      gl_config->gfx_clients = NULL;
    }
  gl_config->gfx_clients = g_malloc(sizeof(*(gl_config->gfx_clients)));
  gl_config->gfx_clients->master = 0;
  gl_config->gfx_clients->wmaster = NULL;
  gl_config->gfx_clients->clients = NULL;
  gl_config->gfx_clients->sock = net_listen_at(gl_config->port, 15,
					       NULL, 0);
  atexit(gfx_clean);
  while (!gfx_new_client(gl_config->gfx_clients->sock, TRUE));
}
