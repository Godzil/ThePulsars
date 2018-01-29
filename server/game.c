/* $Id: game.c,v 1.60 2001/05/07 02:41:36 kilobug Exp $ */

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include <server.h>
#include <objects.h>
#include <public.h>
#include <client.h>
#include <stdlib.h>
#include <message.h>
#include <exec.h>
#include <time.h>

#include <general.h>

static void signal_usr1(int foo)
{
}

static void client_died(int foo)
{
   glbPlayer->alive = False;
   printf("%d: died.\n", glbPlayer->team_id);
   fflush(stdout);
   wait(NULL);
}

static void play_turn(int turn_num)
{
   int i;

   start_timer();
   if (glbPlayer->new)
   {
      player_init(glbPlayer, glbPlayer->team_id);
      glbPlayer->new = False;
   }
   else
   {
      player_new_turn(glbPlayer, turn_num);

      for (i = 0 ; i < gl_config->nb_objects ; i++)
      {
         if (glbObjects[i].obj.team_id == glbPlayer->team_id)
         {
            switch (glbObjects[i].obj.type)
            {
            case obj_akx:
               player_turn_akx(glbPlayer, glbObjects[i].obj.id);
               break;
            case obj_r4d2:
               player_turn_r4d2(glbPlayer, glbObjects[i].obj.id);
               break;
            }
         }
      }
   }
   stop_timer();
   fflush(stdout);
   kill(getppid(), SIGUSR1);
   sigsuspend(gl_config->mask);
}

void start_client(player_t *player, int turn_num)
{
   pid_t pid;
   struct rlimit rl;

   player->alive = True;
   signal(SIGUSR1, signal_usr1);
   if (gl_config->gfx)
   {
      fflush(gl_config->gfx_clients->wmaster);
   }
   if ((pid = fork()))
   {
      player->pid = pid;
   }
   else
   {
      close(1);
      if (gl_config->gfx)
      {
         fclose(gl_config->gfx_clients->wmaster);
         gl_config->gfx_clients->wmaster = NULL;
         gl_config->gfx_clients->master = -1;
         close(gl_config->gfx_clients->sock);
         gl_config->gfx_clients->sock = -1;
      }
      dup2(player->pipe_write, 1);
      close(player->pipe_write);
      close(player->pipe_read);
      player->new = True;
      glbPlayer = player;
      rl.rlim_cur = rl.rlim_max = 16 * 1024 * 1024;
      setrlimit(RLIMIT_DATA, &rl);
      while (True)
      {
         play_turn(turn_num++);
      }
   }
   printf("Client %d : pid = %d\n", player->team_id, pid);
   fflush(stdout);
}

void turn_client(player_t *play)
{
   if (play->alive)
   {
      kill(play->pid, SIGUSR1);
      sigsuspend(gl_config->mask);
   }
   else
   {
      start_client(glbPlayer, gl_config->turn_num);
   }
}

void genere_pipe()
{
   GSList *l;
   player_t *player;
   int fds[2];

   for (l = gl_config->players ; l != NULL ; l = l->next)
   {
      pipe(fds);
      player = l->data;
      player->pipe_read = fds[0];
      player->pipe_write = fds[1];
   }
}

int check_done()
{
   return (map_count_r4d2() == count_r4d2(0));
}

/* lanceur de partie */
void game_launch(conf_t *conf)
{
   GSList *l;
   int i;
   int fds[2];
   int pid;

   genere_pipe();
   signal(SIGCHLD, client_died);
   if (pipe(fds) == -1)
   {
      perror("pipe");
      exit(1);
   }
   fflush(stdout);
   if ((pid = fork()) == 0)
   {
      close(0);
      dup2(fds[0], 0);
      close(fds[0]);
      close(fds[1]);
      launch_messager();
   }
   else
   {
      /*fclose(stdout);*/
      dup2(fds[1], 1);
      close(fds[1]);
      close(fds[0]);
      /*stdout = fdopen(1, "w");*/
      printf("Messager : pid = %d\n", pid);
      printf("Serveur : pid = %d\n", getpid());
      fflush(stdout);
      gl_config->messager_pid = pid;

      for (conf->turn_num = 0 ; conf->turn_num <= conf->nb_turns ; conf->turn_num++)
      {
         printf("Tour n %d sur %d\n", conf->turn_num, conf->nb_turns);
         fflush(stdout);

         for (i = 0 ; i < conf->nb_objects ; i++)
         {
            glbObjects[i].obj.change = False;
         }

         for (l = conf->players ; l != NULL ; l = l->next)
         {
            glbPlayer = l->data;
            turn_client(l->data);
         }

         if (conf->turn_num != 0)
         {
            exec_objects();
         }

         if (check_done())
         {
            break;
         }
      }
   }
}



