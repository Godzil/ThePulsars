/* $Id: server.h,v 1.22 2001/04/14 16:50:48 kilobug Exp $ */

#ifndef __PROLO_SERVER_H__
#define __PROLO_SERVER_H__

#define WANT_PROTOCOL_VERSION 1
#include "../general.h"
#include "../netlib/net.h"

#include <glib.h>
#include <gmodule.h>

#include "public.h"
#include "action.h"
#include "private.h"
#include "network.h"
#include "server.h"
#include "conf.h"
#include "game.h"
#include "plugins.h"
#include "exec.h"
#include "parser.h"
#include "erreur.h"
#include "ipc.h"
#include "client.h"
#include "message.h"

extern player_t	*gl_player;

/* Initialization functions */
conf_t	*init(int argc, char **argv);
void	launch_game(conf_t *conf);
void	load_map(conf_t *conf);
void start_client(player_t *player, int turn_num);

/* Time functions */
void	start_timer();
void	stop_timer();
int	time_left();

void abort();

#endif
