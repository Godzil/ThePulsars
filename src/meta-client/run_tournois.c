/* $Id: run_tournois.c,v 1.6 2001/05/07 02:41:36 kilobug Exp $ */

#include "mclient.h"
#include "tournois.h"
#include <signal.h>

#define ALLOWED_CHARS "_+-$&*!@#~.éáàèëïûôêîìä"

static rt_t *gl_rt;

static void trn_start_new_match(rt_t *rt);

static void trn_display_line(rt_t *rt, const char *buf)
{
  GtkAdjustment *adj;
  int l;

  if (rt->textarea == NULL)
    return;
  gtk_text_freeze(GTK_TEXT(rt->textarea));

  l = gtk_text_get_length(GTK_TEXT(rt->textarea));
  l -= 5000;
  if (l > 0)
    {
      while ((GTK_TEXT_INDEX(GTK_TEXT(rt->textarea), l) != '\n') && l > 0)
	l--;
      if (l > 0)
	{
	  gtk_text_set_point(GTK_TEXT(rt->textarea), 0);
	  gtk_text_forward_delete(GTK_TEXT(rt->textarea), l);
	  gtk_text_set_point(GTK_TEXT(rt->textarea), 
			     gtk_text_get_length(GTK_TEXT(rt->textarea)));
	}
    }

  gtk_text_insert(GTK_TEXT(rt->textarea), NULL, 
		  NULL, NULL, buf, -1);
  gtk_text_thaw(GTK_TEXT(rt->textarea));  
  adj = GTK_TEXT(rt->textarea)->vadj;
  gtk_adjustment_set_value(adj, adj->upper);
}

static void trn_end(rt_t *rt)
{
  int r, i, j, class, class2;
  char buf[MYSQL_BUFFER_SIZE];
  char *s;

  alarm(0);
  if (rt->gdki > 0)
    {
      gdk_input_remove(rt->gdki);
      close(rt->fd);      
    }
  rt->gdki = -1;

  if (!rt->timeout)
    for (i = 0; i < rt->nbp; i++)
      if (rt->scores[i] != 0)
	{
	  class2 = class = 1;
	  for (j = 0; j < rt->nbp; j++)
	    if (rt->scores[j] > rt->scores[i])
	      {
		class *= 2;
		class2++;
	      }
	  r = rt->nbp * 100 / class;
	  mysql_do_query(gl_config->mysql, 
			 "UPDATE Champions "
			 "SET nbmap=nbmap+1, total=total+%d "
			 "WHERE id=%d",
			 r, rt->champ[rt->chmp[i]]);
	  mysql_do_query(gl_config->mysql,
			 "UPDATE Champions "
			 "SET score=total/nbmap, best=GREATEST(best, %f) "
			 "WHERE id=%d", rt->scores[i], 		     
			 rt->champ[rt->chmp[i]]);
	  
	  s = mysql_get_str(gl_config->mysql,
			    "SELECT login FROM Bitmaps B, Champions C "
			    "WHERE B.id = C.auteur AND C.id=%d", 
			    rt->champ[rt->chmp[i]]);
	  g_snprintf(buf, MYSQL_BUFFER_SIZE, "Joueur %s: %f (%d) => %d.\n",
		     s, rt->scores[i], class2, r);
	  trn_display_line(rt, buf);
	  g_free(s);
	}
  trn_start_new_match(rt);
}

static void sigalarm(int foo)
{  
  gl_rt->timeout = TRUE;
  system("killall server");
  fprintf(stderr, "!!!!!!! Timeout.\n");
  trn_end(gl_rt);
}

static void nothing(int foo)
{
}

static void trn_check_score(rt_t *rt, const char *buf)
{
  int i;
  float f;
  char *s;

  if (strncmp(buf, "<server> Player ", 16))
    return;
  i = strtol(buf + 16, &s, 10);
  s += 3;
  f = atof(s);
  mysql_do_query(gl_config->mysql, 
		 "INSERT INTO Scores SET champion=%d, score='%f', match_id=%d",
		 rt->champ[rt->chmp[i - 1]], f, rt->mysql_id);
  rt->scores[i - 1] = f;
}

static gboolean trn_treat_line(rt_t *rt, const char *buf)
{
  alarm(10);
  if (!strcmp(buf, "Bye!\n"))
    {
      trn_display_line(rt, "\nMatch terminé.\n");
      trn_end(rt);
      return FALSE;
    }
  if (strncmp(buf, "<server>", 8))
    trn_display_line(rt, buf);
  else
    trn_check_score(rt, buf);
  return TRUE;
}

static void trn_get_data(rt_t *rt, int fd, GdkInputCondition foo)
{
  char buf[MYSQL_BUFFER_SIZE], *s, *p;
  int sz;
  gboolean r;

  if (rt == NULL)
    return;
  sz = read(fd, buf, MYSQL_BUFFER_SIZE - 1);
  if (sz == -1)
    {
      trn_end(rt);
      return;
    }
  buf[sz] = 0;
  s = g_strconcat(rt->buf, buf, NULL);
  g_free(rt->buf);
  rt->buf = s;

  while ((p = strchr(rt->buf, '\n')) != NULL)
    {
      s = rt->buf;
      rt->buf = g_strdup(p + 1);
      p[1] = 0;
      r = trn_treat_line(rt, s);
      g_free(s);
      if (!r)
	break;
    }   
}

static void trn_do_start_match(rt_t *rt)
{
  int i, j;
  char buf[11];
  char buf2[MYSQL_BUFFER_SIZE];
  int fd[2];
  char *s, *n, *f;

  fprintf(stderr, "\nStarting match %d of %d between: ", rt->cur, rt->nbm);
  for (i = 0; i < rt->nbp; i++)
    {
      fprintf(stderr, "%d ", rt->chmp[i]);
    }
  fprintf(stderr, "\n");
  g_snprintf(buf2, MYSQL_BUFFER_SIZE, 
	     "INSERT INTO Matchs SET map=%d, host='%s', port=%d, statut=1,"
             "                       joueurs=%d, auteur=%d, public=%d",
	     rt->mapid,
 	     (gl_config->host == NULL) ? gl_config->hostname : gl_config->host,
	     4242, rt->nbp, getuid(), FALSE);
  mysql_query_gtk(gl_config->mysql, buf2);
  rt->mysql_id = mysql_insert_id(gl_config->mysql);
  pipe(fd);
  if (!fork())
    { 
      g_snprintf(buf, 11, "%d", rt->nbt);
      close(1);
      dup2(fd[1], 1);
      close(fd[0]);
      /* Memory leak mais suivi d'un execlp */
      s = g_strconcat(gl_config->server, " -n ", buf, " -m ", rt->map, " -q ", 
		      rt->cmd, " ", NULL);
      for (i = 0; i < rt->nbp; i++)
	{
	  n = mysql_get_str(gl_config->mysql,
			    "SELECT nom FROM Champions "
			    "WHERE id=%d",
			    rt->champ[rt->chmp[i]]);
	  for (j = 0; n[j]; j++)
	    if (!isalnum(n[j]) && (strchr(ALLOWED_CHARS, n[j]) == NULL))
	      n[j] = '_';
	  f = mysql_get_str(gl_config->mysql,
			    "SELECT fichier FROM Champions "
			    "WHERE id=%d",
			    rt->champ[rt->chmp[i]]);
	  if ((n != NULL) && (f != NULL))
	    s = g_strconcat(s, " ", n, ":", f, NULL);
	  g_free(n);
	  g_free(f);
	}
      sleep(1);
      if (gl_config->host == NULL)
	execlp("sh", "sh", "-c", s, NULL);
      else
	execlp("ssh", "ssh", gl_config->host, s, NULL);
      _exit(1);
    }
  close(fd[1]);
  fcntl(fd[0], F_SETFL, O_NONBLOCK);
  for (i = 0; i < rt->nbp; i++)
    rt->scores[i] = 0;
  rt->fd = fd[0];
  rt->gdki = gdk_input_add(fd[0], GDK_INPUT_READ, 
			   (GdkInputFunction) (trn_get_data), rt);
  alarm(20);
}

typedef struct _trn_comb_t
{
  int *max_allowed;
  int *cnt;
} trn_comb_t;

typedef struct _trn_rand_t
{
  int *cnt;
} trn_rand_t;

static void trn_make_rand(rt_t *rt)
{
  int i;
  int max;
  trn_rand_t *randd;

  randd = rt->data;
  max = rt->nbm * rt->nbp / rt->nbc + 1;
  for (i = 0; i < rt->nbp; i++)
    {
      do 
	rt->chmp[i] = random() % rt->nbc;	  
      while (randd->cnt[rt->chmp[i]] >= max);
      randd->cnt[rt->chmp[i]]++;
    }
}

static void trn_make_comb(rt_t *rt)
{
  trn_comb_t *comb;
  int j, i;
  
  comb = rt->data;
  for (i = 0; i < rt->nbp; i++)
    rt->chmp[i] = comb->cnt[i];

  for (j = rt->nbp - 1; j > 0; j--)
    if (comb->cnt[j] < comb->max_allowed[j])
      break;
  comb->cnt[j]++;
  if (comb->cnt[j] > comb->max_allowed[j])
    for (i = 0; i < rt->nbp; i++)
      comb->cnt[i] = i;
  else
    for (j++; j < rt->nbp - 1; j++)
      comb->cnt[j] = comb->cnt[j - 1] + 1;
}

static void trn_start_new_match(rt_t *rt)
{
  if (rt->cur >= rt->nbm)
    {
      trn_display_line(rt, "\nTournois terminé.\n");
      rt->run = FALSE;
      trn_free(rt);      
      return;
    }
  rt->timeout = FALSE;
  gl_rt = rt;
  rt->cur++;
  switch (rt->methode)
    {
    case met_rand:
      trn_make_rand(rt);
      break;
    case met_comb:
      trn_make_comb(rt);
      break;
    case met_pyr:
      trn_make_rand(rt);
      break;
    case met_rot:
      trn_make_rand(rt);
      break;
    default:
      fprintf(stderr, "Internal error in trn_init. Aborting.\n");
      exit(1);
      break;
    }  
  trn_do_start_match(rt);
}

void trn_init(rt_t *rt)
{
  int i;
  trn_comb_t *comb;
  trn_rand_t *rand;

  switch (rt->methode)
    {
    case met_rand:
      rt->data = rand = g_malloc(sizeof(*rand));
      rand->cnt = g_malloc(sizeof(int) * rt->nbc);
      bzero(rand->cnt, sizeof(int) * rt->nbc);
      break;
    case met_pyr:
    case met_rot:
      rt->data = NULL;
      break;
    case met_comb:
      rt->data = comb = g_malloc(sizeof(*comb));
      comb->cnt = g_malloc(sizeof(int) * rt->nbp);
      comb->max_allowed = g_malloc(sizeof(int) * rt->nbp);
      for (i = 0; i < rt->nbp; i++)
	{
	  comb->cnt[i] = i;
	  comb->max_allowed[i] = rt->nbc - (rt->nbp - i);
	}
      break;
    default:
      fprintf(stderr, "Internal error in trn_init. Aborting.\n");
      exit(1);
      break;
    }
  rt->gdki = -1;
  gl_rt = rt;
  signal(SIGALRM, sigalarm);
  trn_start_new_match(rt);
}

void trn_free(rt_t *rt)
{
  trn_comb_t *comb;
  trn_rand_t *rand;

  if (rt->run || rt->disp)
    return;
  switch (rt->methode)
    {
    case met_rand:
      rand = rt->data;
      g_free(rand->cnt);
      g_free(rand);
      break;
    case met_pyr:
    case met_rot:
      break;
    case met_comb:
      comb = rt->data;
      g_free(comb->cnt);
      g_free(comb->max_allowed);
      g_free(comb);
      break;
    default:
      fprintf(stderr, "Internal error in trn_init. Aborting.\n");
      exit(1);
      break;
    }
  g_free(rt->cmd);
  g_free(rt->chmp);
  g_free(rt->map);
  if (rt->buf != NULL)
    g_free(rt->buf);
  g_free(rt->scores);
  g_free(rt);
}

