/* $Id: init.c,v 1.26 2001/05/04 14:36:09 kilobug Exp $ */

#include "mclient.h"
#include <sys/wait.h>
#include <time.h>
#include <locale.h>

#define MYSQL_PASSWORD "pr1L0g1N"
#define MYSQL_LOGIN    "bitmap"
#define MYSQL_PORT     3306

#define MYSQL_HOST     "drizzt"
#define MYSQL_DB       "Finale"
/*
#define MYSQL_HOST     "pantera.prologin"
#define MYSQL_DB       "finale"
*/
#define GFX_CLIENT "gtk-client"
#define SERVER "server"

void byebye(GtkWidget *widget, void *data)
{
  gtk_main_quit();
}

static void sigchild()
{
  wait(NULL);
}

static void clean_abort()
{
  exit(2);
}

static void clean_quit()
{
  set_my_status(STATUT_ABSENT);
}

void set_my_status(int stat)
{
  char buf[MYSQL_BUFFER_SIZE];

  g_snprintf(buf, MYSQL_BUFFER_SIZE, 
	     "UPDATE Bitmaps SET statut=%d, machine='%s' WHERE id=%d",
	     stat, gl_config->hostname, getuid());
  mysql_query(gl_config->mysql, buf);
}

static void get_args(int argc, char **argv, conf_t *res)
{
  int i;

  res->db = MYSQL_DB;
  res->host = NULL;

  for (i = 1; i < argc - 1; i++)
    {
      if (!strcmp(argv[i], "-r"))
	res->host = argv[++i];
      else
	if (!strcmp(argv[i], "-d"))
	  res->db = argv[++i];
      else
	fprintf(stderr, "Paramètre inconnu: %s\n", argv[i]);
    }
}

void init(int argc, char **argv)
{
  conf_t *res;
  char *passwd, c;
  int i;  

  srandom(time(NULL));
  res = g_malloc(sizeof(*res));
  res->hostname = g_malloc(MYSQL_BUFFER_SIZE);
  gethostname(res->hostname, MYSQL_BUFFER_SIZE);

  res->bindir = g_dirname(argv[0]);
  if (g_path_is_absolute(SERVER) || (res->bindir[0] != argv[0][0]))
    res->server = SERVER;
  else
    res->server = g_strconcat(res->bindir, G_DIR_SEPARATOR_S, SERVER, 
				  NULL);
  if (g_path_is_absolute(GFX_CLIENT) || (res->bindir[0] != argv[0][0]))
    res->gfx_client = GFX_CLIENT;
  else
    res->gfx_client = g_strconcat(res->bindir, G_DIR_SEPARATOR_S, GFX_CLIENT,
				  NULL);

  get_args(argc, argv, res);

  res->mysql = NULL;
  res->matl = res->bmpl = res->mapl = res->prgl = res->result = NULL;
  res->mapa = res->prga = NULL;
  res->window = NULL;
  for (i = 0; i < NOTEBOOK_PAGE_NUM; i++)
    res->menunb[i] = NULL;

  res->mysql = mysql_init(NULL);
  mysql_options(res->mysql, MYSQL_READ_DEFAULT_GROUP, "meta-client");
  passwd = g_strdup(MYSQL_PASSWORD);
  for (i = 0; i < strlen(passwd); i += 2)
    {
      c = passwd[i];
      passwd[i] = passwd[i + 1];
      passwd[i + 1] = c;
    }
  if (mysql_real_connect(res->mysql, MYSQL_HOST, MYSQL_LOGIN, passwd,
			 res->db, MYSQL_PORT, NULL, 0) == NULL)
    {
      fprintf(stderr, "Failed to connect to database: Error: %s\n",
	      mysql_error(res->mysql));
      exit(1);
    }
  gl_config = res;
  set_my_status(STATUT_PRESENT);
  i = mysql_get_int(gl_config->mysql,
		    "SELECT id FROM Bitmaps WHERE id = %d", 
		    getuid());
  if (i != getuid())
    {
      fprintf(stderr, "You're nobody. Go away.\n");
      exit(1);
    }
  res->staff = mysql_get_int(gl_config->mysql,
			     "SELECT staff FROM Bitmaps WHERE id = %d", 
			     getuid());

  atexit(clean_quit);
  signal(SIGINT, clean_abort);
  signal(SIGHUP, clean_abort);
  signal(SIGQUIT, clean_abort);
  signal(SIGABRT, clean_abort);
  signal(SIGTERM, clean_abort);
  signal(SIGCHLD, sigchild);
  setlocale(LC_ALL, "fr_FR");
  gtk_set_locale();  
  setlocale(LC_NUMERIC, "C");
  gtk_init(&argc, &argv);
}
