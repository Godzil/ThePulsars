/* $Id: network.h,v 1.6 2001/04/15 14:57:19 kilobug Exp $ */

#ifndef __PROLO_SERVER_NETWORK_H__
#define __PROLO_SERVER_NETWORK_H__

typedef struct _gfx_client_t
{
  gboolean alive;
  int sock;
  FILE *wsock;
} gfx_client_t;

typedef struct _gfx_t
{
  int sock;
  int master;
  FILE *wmaster;
  GSList *clients;
} gfx_t;

gboolean gfx_get_ack(gfx_client_t *cl);
gboolean gfx_new_client(int socket, gboolean master);
void gfx_client_new_turn(gboolean end);
void gfx_init();
void gfx_write_object(FILE *file, item_t *obj);

#endif
