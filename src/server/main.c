/* $Id: main.c,v 1.11 2001/05/02 11:04:27 kilobug Exp $ */

#include "server.h"

#include <caml/callback.h>

#ifdef ENABLE_GPC
void init_Gpc(int argc, char **argv, char **arge);
void init_Init();
#endif

int main(int argc, char **argv, char **arge)
{
  conf_t *conf = NULL;
  GSList	*l;  
  player_t	*player;

#ifdef ENABLE_GPC
  init_Gpc(argc, argv, arge);
  init_Init();
#endif

  conf = init(argc, argv);
  if (conf == NULL)
    abort();
  launch_game(conf);
  for (l = conf->players; l != NULL; l = l->next)
    {
      gl_player = l->data;
      fprintf(stdout, "Player %d : %f\n", gl_player->team_id, 
	      gl_player->score);
      fflush(stdout);
    }
  gfx_client_new_turn(TRUE);
  
  kill(gl_config->messager_pid, SIGUSR1);
  gl_config->messager_pid = 0;
  for (l = gl_config->players; l != NULL; l = l->next)
    {
      player = l->data;
      kill(player->pid, SIGKILL);
      player->pid = 0;
    }
  return 0;
}
