#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <server.h>

#include <general.h>
#include <ipc.h>

player_t *glbPlayer;

static void *get_symbol(GModule *hdl, const char *name, boolean_t stop_error)
{
   void *p = NULL;
   char *nm = strdup(name);

   if (!g_module_symbol(hdl, nm, &p))
   {
      nm[0] = toupper(nm[0]);

      if (!g_module_symbol(hdl, nm, &p) && stop_error)
      {
         fprintf(stderr, "Dynamic library error : %s : %s\n", name,
                 g_module_error());
         return NULL;
      }
   }
   free(nm);
   return p;
}

player_t *player_new(const char *libname, const char *name, int id,
                     conf_t *conf)
{
   player_t *player;
   void (*caml)(char **);
   char *caml_par[2] = { "./server", NULL };

   printf("%s : %s ( %d )\n", name, libname, id);
   player = shm_alloc(sizeof(player_t), get_shm_plr_id() + id);
   if (!g_module_supported())
   {
      fprintf(stderr, "%s\n", "Fatal error : Dynamic library not supported!");
      server_abort();
   }
   player->team_id = id;
   player->name = name;
   player->score = 0;
   player->on = True;
   player->handle = g_module_open(libname, 0);
   player->time_turn = conf->turn_msec;
   if (player->handle == NULL)
   {
      fprintf(stderr, "Dynamic library error : %s : %s\n",
              libname, g_module_error());
      server_abort();
   }
   caml = get_symbol(player->handle, "caml_startup", False);
   if (caml)
   {
      caml(caml_par);
   }

   player->pipe_read = 0;
   player->pipe_write = 1;
   player->alive = False;
   player->init = get_symbol(player->handle, "player_init", True);
   player->new_turn = get_symbol(player->handle, "player_new_turn", True);
   player->turn_akx = get_symbol(player->handle, "player_akx_turn", True);
   player->turn_r4d2 = get_symbol(player->handle, "player_r4d2_turn", True);
   return player;
}

void player_destroy(player_t *player)
{
   g_module_close(player->handle);
   free(player);
}

void player_init(player_t *p, int team_id)
{
   if (p->init)
   {
      p->init(team_id, gl_config->nb_players);
   }
   else
   {
      printf("erreur de symbol : 'player_init'\n");
   }
}

void player_new_turn(player_t *p, int turn_num)
{
   if (p->new_turn)
   {
      p->new_turn(turn_num);
   }
   else
   {
      printf("erreur de symbol : 'player_new_turn'\n");
   }
}

void player_turn_akx(player_t *p, int akx_id)
{
   if (p->turn_akx)
   {
      p->turn_akx(akx_id);
   }
   else
   {
      printf("erreur de symbol : 'player_turn_akx'\n");
   }
}

void player_turn_r4d2(player_t *p, int r4d2_id)
{
   if (p->turn_r4d2)
   {
      p->turn_r4d2(r4d2_id);
   }
   else
   {
      printf("erreur de symbol : 'player_turn_r4d2'\n");
   }
}
