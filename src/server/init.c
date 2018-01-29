/* $Id */
#include <sys/types.h>
#include <sys/wait.h>


#include "server.h"
#include "conf.h"

static conf_t *get_default_conf()
{
  conf_t *res;
  
  res = shm_alloc(sizeof(*res), get_shm_conf_id());
  res->verbose = FALSE;
  res->quiet = FALSE;
  res->gfx = FALSE;
  res->map_name = CONF_DEFAULT_MAP;
  res->nb_turns = CONF_DEFAULT_NBTURNS;
  res->turn_msec = CONF_DEFAULT_TURN_MSEC;
  res->turn_take_akx = DEF_TURN_TAKE_AKX;
  res->turn_untake_akx = DEF_TURN_UNTAKE_AKX;
  res->turn_take_r4d2 = DEF_TURN_TAKE_R4D2;
  res->turn_untake_r4d2 = DEF_TURN_UNTAKE_R4D2;
  res->r4d2_speed = DEF_R4D2_SPEED;
  res->akx_speed = DEF_AKX_SPEED;
  res->pulse_power = DEF_PULSE_POWER;
  res->see_power = DEF_SEE_POWER;
  res->destroy_speed = DEF_DESTROY_SPEED;
  res->pulse_coef = DEF_PULSE_COEF;
  res->nb_akx = res->nb_r4d2 =res->nb_objects = 0;
  res->size_x = DEF_SIZE_X;
  res->size_y = DEF_SIZE_Y;
  res->players = NULL;
  res->nb_players = 0;
  res->turn_num = 0;
  res->sand_file = NULL;
  res->port = CONF_DEFAULT_PORT;
  res->pub = TRUE;
  return res;
}

static void cmdline_add_player(const char *str, conf_t *conf)
{
  char *name, *filename;
  char *p;
  static int id = 1;
  
  name = g_strdup(str);
  p = strchr(name, ':');
  if (p == NULL)
    {
      filename = name;
      name = g_strdup(g_basename(filename));
    }      
  else
    {
      *p = 0;
      filename = g_strdup(p + 1);
    }
  conf->players = g_slist_prepend(conf->players,
				  new_player(filename, name, id++, conf));
}

static void help(char **argv)
{
  printf("%s", "Serveur Prologin version "VERSION"\n");
  printf("\n");
  printf("Syntaxe: %s [options] [-m carte] nom:fichier ...\n",
	 argv[0]);
  printf("\nOptions:\n");
  printf("  -v     \t Mode verbose.\n");
  printf("  -q     \t Supprime les messages des clients.\n");
  printf("  -n val \t Nombre de tours que dure la partie.\n");
  printf("  -t val \t Nombre de milliseconde pas tour.\n");
  printf("  -m     \t Fichier carte.\n");
  printf("\nOptions graphiques:\n");
  printf("  -g     \t Accepte les clients graphiques.\n");
  printf("  -P     \t Mode privé.\n");
  printf("  -p val \t Port pour les clients graphiques (4242).\n");
}

static conf_t *parse_cmdline(int argc, char **argv)
{
  int i;
  conf_t *res;
 
  res = get_default_conf();
  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-')
      {
	switch (argv[i][1])
	  {
	  case 'h':
	    help(argv);
	    exit(0);
	  case 'P':
	    res->pub = FALSE;
	    break;
	  case 'v':
	    res->verbose = TRUE;
	    break;
	  case 'g':
	    res->gfx = TRUE;
	    break;
	  case 'q':
	    res->quiet = TRUE;
	    break;
	  case 'n':
	    i++;
	    if (i == argc)
	      {
		printf("-n requiert un argument.\n");
		return NULL;
	      }
	    res->nb_turns = atoi(argv[i]);
	    break;
	  case 't':
	    i++;
	    if (i == argc)
	      {
		printf("-t requiert un argument.\n");
		return NULL;
	      }
	    res->turn_msec = atoi(argv[i]);
	    break;
	  case 'p':
	    i++;
	    if (i == argc)
	      {
		printf("-p requiert un argument.\n");
		return NULL;
	      }
	    res->port = atoi(argv[i]);
	    break;
	  case 'm':
	    i++;
	    if (i == argc)
	      {
		printf("-m requiert un argument.\n");
		return NULL;
	      }
	    res->map_name = argv[i];
	    break;
	  }
      }
    else
      cmdline_add_player(argv[i], res);
  res->nb_players = g_slist_length(res->players);
  return res;
}

conf_t *init(int argc, char **argv)
{
  conf_t *conf;
  sigset_t mask2;

  if (argc < 2)
    {
      help(argv);
      return NULL;
    }
  gl_config = conf = parse_cmdline(argc, argv);
  if (conf == NULL)
    return NULL;
  atexit(clean_shm);
  load_map(conf);
  gfx_init();
  sigprocmask(0, NULL, &mask2);
  sigaddset(&mask2, SIGUSR1);
  sigaddset(&mask2, SIGCHLD);
  conf->mask = p_malloc(sizeof(*conf->mask));
  sigprocmask(SIG_SETMASK, &mask2, conf->mask);
  signal(SIGINT, exit);
  return conf;
}

void abort()
{
  fflush(stdout);
  fflush(stderr);
  fprintf(stdout, "\nBye!\n");
  fflush(stdout);
  exit(1);
}
