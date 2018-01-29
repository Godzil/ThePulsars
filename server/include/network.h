/* $Id: network.h,v 1.6 2001/04/15 14:57:19 kilobug Exp $ */

#ifndef __PROLO_SERVER_NETWORK_H__
#define __PROLO_SERVER_NETWORK_H__

#include <stdio.h>
#include <general.h>
#include "objects.h"

typedef struct _gfx_client_t
{
    boolean_t alive;
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

boolean_t gfx_get_ack(gfx_client_t *cl);

boolean_t gfx_new_client(int socket, boolean_t master);

void gfx_client_new_turn(boolean_t end);

void gfx_init();

void gfx_write_object(FILE *file, item_t *obj);

#endif
