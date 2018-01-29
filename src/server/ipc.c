/* $Id: ipc.c,v 1.11 2001/04/29 22:07:44 kilobug Exp $ */

#include "server.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

static GSList *shm_memory_list = NULL;

/*
** SHM
*/

int get_shm_key(char c)
{
  int i;

  i = ftok(".", c);
  if (i != -1)
    return i;
  perror("ftok");
  abort();
}

int get_shm_conf_id()
{
  return get_shm_key('C');
}

int get_shm_world_id()
{
  return get_shm_key('W');
}

int get_shm_plr_id()
{
  return get_shm_key('p');
}


void *shm_alloc(int size, int id)
{
  int shmid;
  void *res = NULL;

  if ((shmid = shmget(id, size, IPC_CREAT | IPC_EXCL | 0600)) == -1) 
    {
      if((shmid = shmget(id, size, 0)) == -1) 
	{
	  perror("shmget");
	  abort();
	}
    }
  if((res = shmat(shmid, 0, 0)) == (void *) -1)
    {
      perror("shmat");
      abort();
    }
  shm_memory_list = g_slist_prepend(shm_memory_list, GINT_TO_POINTER(shmid));
  return res;
}

void clean_shm()
{
  GSList *l;  

  if (gl_config->messager_pid)
    kill(gl_config->messager_pid, SIGUSR1);
  for (l = gl_config->players; l != NULL; l = l->next)
    {
      gl_player = l->data;
      if (gl_player->pid)
	kill(gl_player->pid, SIGKILL);
    }
  for (l = shm_memory_list; l != NULL; l = l->next)
    shmctl(GPOINTER_TO_INT(l->data), IPC_RMID, NULL);
  g_slist_free(shm_memory_list);
  shm_memory_list = NULL;
}

