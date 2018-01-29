/* $Id: init.c,v 1.26 2001/04/29 22:24:37 kilobug Exp $ */

#include "client.h"

#define DEFAULT_PORT 4242
#define DEFAULT_HOST "localhost"

conf_t *gl_config = NULL;

static void clean()
{
  send_goodbye(gl_config);
}

static void clean_exit()
{
  clean();
  exit(1);
}

static void help(const char *name)
{
  printf("syntaxe: %s [-d] [-h machine] [-p port]\n", name);
  exit(1);
}

static const char *get_arg(int argc, char **argv, int *i)
{
  if (*i == argc - 1)
    {
      printf("%s requiert un argument.\n", argv[*i]);
      exit(1);
    }
  return (argv[++(*i)]);
}

static void sigusr2()
{
  gl_config->terminated = TRUE;
  if (gl_config->ready)
    allow_new_turn(gl_config);
}

static guchar *gen_convtb()
{
  guchar *res;
  int org, add;
  double f;

  res = g_malloc(256 * 256 * sizeof(*res));
  for (org = 0; org < 256; org++)
    for (add = 0; add < 256; add++)
      {
	/*	f = ((double) org / 256);
		f = (0.2 + 0.8 * SQR(f)) * add / 1.3;*/
	f = add * 0.3;
	res[org + add * 256] = MIN(org + f, 255);
      }
  return res;
}

conf_t *init(int argc, char **argv)
{
  conf_t *res;
  int i;

  gtk_set_locale();
  gtk_init(&argc, &argv);
  gdk_set_use_xshm(TRUE);
  gdk_rgb_init();

  res = g_malloc(sizeof(*res));
  res->screen = NULL;
  res->debug = FALSE;
  res->socket = -1;
  res->host = DEFAULT_HOST;
  res->port = DEFAULT_PORT;
  res->gdk_io_id = -1;
  res->map = NULL;
  res->convtb = gen_convtb();
  res->turn.mode = tt_pause;
  res->turn.nbt = 10;
  res->turn.spd = 50;
  res->connected = res->ready = FALSE;
  res->window = res->status = res->mini = res->main = res->conf = NULL;
  res->stat.r4d2 = res->stat.akx = res->stat.pl = res->stat.stat = NULL;
  res->stat.menu = NULL;
  res->stat.on = FALSE;
  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] != '-')
	help(argv[0]);
      switch (argv[i][1])
	{
	case 'd':
	  res->debug = TRUE;
	  break;
	case 'h':
	  res->host = get_arg(argc, argv, &i);
	  break;
	case 'p':
	  res->port = atoi(get_arg(argc, argv, &i));
	  break;
	default:
	  help(argv[0]);
	}
    }
  res->host = g_strdup(res->host);
  res->background = load_picture("sand.gif", NULL);
  res->akx = load_picture("akx.xpm", "akx_mask.xpm");
  res->r4d2 = load_picture("r4d2.xpm", "r4d2_mask.xpm");
  res->big_ofs_x = res->big_ofs_y = 0;
  res->mini_center_x = res->mini_center_y = 0;
  res->big_zoom = 50.0;
  res->log_big_zoom = 4;
  res->big_radar = FALSE;
  res->big_pulse = TRUE;
  net_set_error_handler(network_error, res);
  gl_config = res;
  atexit(clean);
  signal(SIGINT, clean_exit);
  signal(SIGUSR2, sigusr2);
  return res;
}

gint idle_connect(void *conf)
{
  do_connect(conf);
  return FALSE;
}

void callback_connect(GtkWidget *widget, void *conf)
{
  do_connect(conf);
}
