#include "server.h"

void	init_fd_set(fd_set *fds)
{
  GSList	*l;
  player_t	*player;

  FD_ZERO(fds);
  FD_SET(0, fds);
  for (l = gl_config->players; l != NULL; l = l->next)
    {
      player = l->data;
      FD_SET(player->pipe_read, fds);
    } 
}

int	get_max_player_fd(fd_set *fds, FILE **tab_fds)
{
  GSList	*l;
  player_t	*player;
  int		max = 0;

  fcntl(0, F_SETFL, O_NONBLOCK); 
  tab_fds[0] = stdin;

  for (l = gl_config->players; l != NULL; l = l->next)
    {
      player = l->data;
      if (player->pipe_read > max)
	max = player->pipe_read;
      fcntl(player->pipe_read, F_SETFL, O_NONBLOCK); 
      tab_fds[player->team_id] = fdopen(player->pipe_read, "r");
    } 
  return max;
}

void	test_file_fd(fd_set *fds, FILE **tab_fds)
{
  GSList	*l;
  player_t	*player;
  char		buffer[MESSAGE_BUFFER_SIZE];

  for (l = gl_config->players; l != NULL; l = l->next)
    {
      player = l->data;
      if (FD_ISSET(player->pipe_read, fds))
	{
	  while (fgets(buffer, MESSAGE_BUFFER_SIZE, 
			       tab_fds[player->team_id]) != NULL)
	    {
	      if (!gl_config->quiet)
		printf("<%s(%d)> %s", player->name, player->team_id, buffer);
	    }
	}
    } 
  if (FD_ISSET(0, fds))
    {
      while (fgets(buffer, MESSAGE_BUFFER_SIZE, tab_fds[0]) != NULL)
	printf("<server> %s", buffer);
    }
  fflush(stdout);
}

static gboolean gl_stop = FALSE;

void	messager_exit(int foo)
{
  gl_stop = TRUE;
}

void	launch_messager()
{
  fd_set		fds;
  int			fd_max;
  FILE			**tab_fds;

  sigprocmask(SIG_SETMASK, gl_config->mask, NULL);
  signal(SIGUSR1, messager_exit);
  tab_fds = malloc(1+gl_config->nb_players * sizeof(FILE*));
  fd_max = get_max_player_fd(&fds, tab_fds); 
  while (!gl_stop)
    {
      init_fd_set(&fds);
      select(fd_max + 1, &fds, NULL, NULL, NULL);
      test_file_fd(&fds, tab_fds);
    }
  write(1, "\nBye!\n", 6);
  exit(0);
}

