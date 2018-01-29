/* $Id: run_match.c,v 1.25 2001/05/06 01:23:05 kilobug Exp $ */

#include "mclient.h"

#define ALLOWED_CHARS "_+-$&*!@#~.éáàèëïûôêîìä"

typedef struct _player_t
{
  int id;
  float score;
  GdkColor *color;
} player_t;

typedef struct _match_t
{
  GtkWidget *gfx, *port, *cmd, *public, *turns;
  int mapid;
  player_t *players;
  GtkWidget **wids;
} match_t;

typedef struct _rmatch_t
{
  GtkWidget *dialog, *textarea;
  int fd;
  int pid, mysql_id;
  player_t *players;
  int nbp;
  int gdki;
  char *buf;
} rmatch_t;

static void kill_match(GtkWidget *wi, rmatch_t*rm)
{
  if (rm->pid > 0)
    kill(rm->pid, SIGUSR2);
}

static gboolean match_display_line(rmatch_t *rm, const char *buf, 
				   gboolean force);

static void match_fetch_player(player_t *players, int num, int nbp)
{
  int i, class = 1;

  if (gl_config->staff && (num > 0) && (nbp > 1))
    {
      for (i = 1; i <= nbp; i++)
	if (players[i].score > players[num].score)
	  class *= 2;      
      i = nbp * 100 / class;
      mysql_do_query(gl_config->mysql, 
		     "UPDATE Champions "
		     "SET nbmap=nbmap+1, total=total+%d "
		     "WHERE id=%d",
		     i, players[num].id);
      mysql_do_query(gl_config->mysql,
		     "UPDATE Champions "
		     "SET score=total/nbmap, best=GREATEST(best, %f) "
		     "WHERE id=%d", players[num].score, players[num].id);
    }
  g_free(players[num].color);
}

static void match_end(GtkWidget *wi, rmatch_t*rm)
{
  int i;

  gdk_input_remove(rm->gdki);
  match_display_line(rm, "<server> Match terminé.\n", FALSE);
  match_display_line(rm, "\n", TRUE);
  close(rm->fd);
  g_free(rm->buf);
  for (i = 0; i <= rm->nbp; i++)
    match_fetch_player(rm->players, i, rm->nbp);
  g_free(rm->players);
}

static void kill_match_window(GtkWidget *wi, rmatch_t *rm)
{
  gtk_widget_destroy(rm->dialog);
  rm->dialog = NULL;
  rm->textarea = NULL;
}

static void match_check_score(rmatch_t *rm, const char *buf)
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
		 rm->players[i].id, f, rm->mysql_id);
  rm->players[i].score = f;
}

static void match_check_bouvier(rmatch_t *rm, const char *buf)
{
  const char *p;
  char *q;
  int i;
  char b[MYSQL_BUFFER_SIZE];

  p = strchr(buf, '(');
  if (p == NULL)
    return;
  p++;
  i = strtol(p, &q, 10);
  if (strcmp(q, ")> Bravo, tu as gagne un tour de patio!\n"))
    return;
  q = mysql_get_str(gl_config->mysql, 
		    "SELECT b.login FROM Bitmaps b, Champions c "
		    "WHERE c.id=%d AND c.auteur = b.id", rm->players[i].id);
  snprintf(b, MYSQL_BUFFER_SIZE, 
	   "ns_send_msg %s 'Bravo, tu as gagne un tour de patio!'",
	   q);
  system(b);
  snprintf(b, MYSQL_BUFFER_SIZE, 
	   "echo '%s a gagne un tour de patio!' | mail bouvie_p -s Bouvieur "
	   "-c prolo",
	   q);
  system(b);
  g_free(q);
}

static gboolean match_display_line(rmatch_t *rm, const char *buf, 
				   gboolean force)
{
  char *p;
  int id;
  static int last_id = -42;
  static char *buf2 = NULL;
  
  if (!strcmp(buf, "Bye!\n"))
    {
      match_end(NULL, rm);
      return FALSE;
    }
  match_check_score(rm, buf);
  match_check_bouvier(rm, buf);
  if (rm->textarea == NULL)
    return TRUE;
  p = strchr(buf, '(');
  if (p == NULL)
    id = 0;
  else
    id = atoi(p + 1);
  if (id > rm->nbp)
    id = 0;

  if (id == 0)
    force = TRUE;

  if ((id == last_id) && (!force))
    {
      p = buf2;
      buf2 = g_strconcat(p, buf, NULL);
      g_free(p);
    }
  else
    {
      if (buf2 != NULL)
	{
	  gtk_text_insert(GTK_TEXT(rm->textarea), NULL, 
			  rm->players[last_id].color, 
			  NULL, buf2, -1);
	  g_free(buf2);
	}
      buf2 = g_strdup(buf);
      last_id = id;
    }
  return TRUE;
}

static void match_get_data(rmatch_t *rm, int fd, GdkInputCondition foo)
{
  char buf[MYSQL_BUFFER_SIZE], *s, *p;
  int sz, l;
  gboolean r;
  GtkAdjustment *adj;

  if (rm == NULL)
    return;
  sz = read(fd, buf, MYSQL_BUFFER_SIZE - 1);
  if (sz == -1)
    {
      match_end(NULL, rm);
      return;
    }
  buf[sz] = 0;
  s = g_strconcat(rm->buf, buf, NULL);
  g_free(rm->buf);
  rm->buf = s;

  if (rm->textarea != NULL)
    {
      gtk_text_freeze(GTK_TEXT(rm->textarea));
      l = gtk_text_get_length(GTK_TEXT(rm->textarea));
      l -= 5000;
      if (l > 0)
	{
	  while ((GTK_TEXT_INDEX(GTK_TEXT(rm->textarea), l) != '\n') && l > 0)
	    l--;
	  if (l > 0)
	    {
	      gtk_text_set_point(GTK_TEXT(rm->textarea), 0);
	      gtk_text_forward_delete(GTK_TEXT(rm->textarea), l);
	      gtk_text_set_point(GTK_TEXT(rm->textarea), 
				 gtk_text_get_length(GTK_TEXT(rm->textarea)));
	    }
	}
    }

  while ((p = strchr(rm->buf, '\n')) != NULL)
    {
      s = rm->buf;
      rm->buf = g_strdup(p + 1);
      p[1] = 0;
      r = match_display_line(rm, s, FALSE);
      g_free(s);
      if (!r)
	break;
    }      

  if (rm->textarea != NULL)
    {
      gtk_text_thaw(GTK_TEXT(rm->textarea));  
      adj = GTK_TEXT(rm->textarea)->vadj;
      gtk_adjustment_set_value(adj, adj->upper);
    }
}

static int my_creneau(int val)
{
  if (val >= 0)
      return CLAMP((120 - val) * 0xFF / 120, 0, 0xFF);
  else
      return CLAMP((120 + val) * 0xFF / 120, 0, 0xFF);
}

static GdkColor *create_player_color(int i, int nb)
{
  int	angle;
  GdkColor *col;

  col = g_malloc(sizeof(*col));
  col->pixel = 0;
  angle = (360 * i) / nb; /* position => angle dans spectre */
  if (angle > 180)
    angle = angle - 360;
  col->red = my_creneau(angle) * 128;
  col->green = my_creneau(angle - 120) * 128;
  col->blue = my_creneau(angle + 120) * 128;
  gdk_colormap_alloc_color(gdk_colormap_get_system(), col, TRUE, TRUE);
  return col;
}

static void create_listener(int fd, int pid, int nbp, int mysql_id, 
			    int *play_ids)
{
  rmatch_t *rm;
  GtkWidget *sw, *dial, *widget, *hbox;
  GtkScrolledWindow *scrw;
  int i;

  rm = g_malloc(sizeof(*rm));
  rm->fd = fd;
  rm->gdki = gdk_input_add(fd, GDK_INPUT_READ, 
			   (GdkInputFunction) (match_get_data), rm);
  rm->pid = pid;
  rm->mysql_id = mysql_id;
  rm->nbp = nbp;
  rm->buf = g_strdup("");
  rm->players = g_malloc(sizeof(*(rm->players)) * (rm->nbp + 1));
  for (i = 0; i <= rm->nbp; i++)
    {
      rm->players[i].id = (i > 0) ? play_ids[i - 1] : 0;
      rm->players[i].score = 0;
      rm->players[i].color = create_player_color(i, rm->nbp + 1);
    }

  /* Main box */
  dial = rm->dialog = gtk_dialog_new();
  gtk_widget_hide(dial);
  gtk_signal_connect(GTK_OBJECT(dial), "destroy", 
		     GTK_SIGNAL_FUNC(kill_match_window), rm);
  gtk_window_set_title(GTK_WINDOW(dial), "Suivi d'un match");
  gtk_container_set_border_width(GTK_CONTAINER(&(GTK_DIALOG(dial)->window)), 
				 7);
  sw = gtk_scrolled_window_new(NULL, NULL);
  scrw = GTK_SCROLLED_WINDOW(sw);
  rm->textarea = gtk_text_new(gtk_scrolled_window_get_hadjustment(scrw),
			      gtk_scrolled_window_get_vadjustment(scrw));
  gtk_text_set_word_wrap(GTK_TEXT(rm->textarea), FALSE);
  gtk_text_set_line_wrap(GTK_TEXT(rm->textarea), TRUE);
  gtk_text_set_editable(GTK_TEXT(rm->textarea), FALSE);
  gtk_widget_show(sw);
  gtk_scrolled_window_set_policy(scrw, GTK_POLICY_AUTOMATIC, 
				 GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(sw), rm->textarea);
  gtk_widget_show(rm->textarea);

  /* Buttons */
  hbox = gtk_hbox_new(FALSE, 3);
  gtk_widget_show(hbox);
  widget = gtk_button_new_with_label(" Arrêter ");
  gtk_widget_show(widget);
  gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
		     GTK_SIGNAL_FUNC(kill_match), rm);
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
  widget = gtk_button_new_with_label(" Fermer ");
  gtk_widget_show(widget);
  gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
		     GTK_SIGNAL_FUNC(kill_match_window), rm);
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dial)->action_area), hbox, TRUE,
		     TRUE, 9);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dial)->vbox), sw, TRUE,
		     TRUE, 9);
  
  gtk_window_set_default_size(GTK_WINDOW(&(GTK_DIALOG(dial)->window)),
			      340, 280);
  gtk_widget_show(rm->dialog);
}

static void do_run_match(const char *cmd, gboolean gfx, int portn, int id,
			 int nbp, int *play_ids)
{
  char port[11];
  char buf[MYSQL_BUFFER_SIZE];
  int fd[2];
  pid_t pid = -1;

  g_snprintf(port, 11, "%d", portn);
  pipe(fd);
  if (!fork())
    { 
      close(1);
      dup2(fd[1], 1);
      close(fd[0]);
      if (gl_config->host == NULL)
	g_snprintf(buf, MYSQL_BUFFER_SIZE, "exec %s -p %s %s", 
		   gl_config->server, port, cmd);
      else
	g_snprintf(buf, MYSQL_BUFFER_SIZE, "exec ssh %s %s -p %s %s", 
		   gl_config->host, gl_config->server, port, cmd);
	
      system(buf);
      g_snprintf(buf, MYSQL_BUFFER_SIZE, 
		 "UPDATE Matchs SET statut=0 WHERE id=%d", id);
      mysql_query(gl_config->mysql, buf);
      _exit(1);
    }
  close(fd[1]);
  if (gfx)
    if (!(pid = fork()))
      {
	sleep(1);
	if (gl_config->host == NULL)
	  execlp(gl_config->gfx_client, gl_config->gfx_client, 
		 "-d", "-p", port, NULL);
	else
	  execlp(gl_config->gfx_client, gl_config->gfx_client, 
		 "-d", "-p", port, "-h", gl_config->host, NULL);
	_exit(1);
      }
  fcntl(fd[0], F_SETFL, O_NONBLOCK); 
  create_listener(fd[0], pid, nbp, id, play_ids);
}

static void run_match_commit(GtkWidget *wid, match_t *match)
{
  gboolean gfx, pub;
  int port;
  char buf[MYSQL_BUFFER_SIZE], buf2[MYSQL_BUFFER_SIZE];
  char *p = NULL, *s = NULL, *s2 = NULL;
  int i, j, nbp, play;
  int nbturn;
  int players_id[64];
  int id;
  
  if (match == NULL)
    return;
  if (match->wids == NULL)
    return;
  set_data_from_toggle(match->gfx, &gfx);
  set_data_from_spin(match->port, &port);
  set_data_from_spin(match->turns, &nbturn);
  if (gfx)
    set_data_from_toggle(match->public, &pub);
  else 
    pub = FALSE;
  p = buf;
  s = mysql_get_str(gl_config->mysql,
		    "SELECT fichier FROM Maps WHERE id=%d", match->mapid);
  if (!g_path_is_absolute(s) && getenv("PROLO_DATA_DIR") != NULL)
    {
      s2 = s;
      s = g_strconcat(getenv("PROLO_DATA_DIR"), G_DIR_SEPARATOR_S, s2, NULL);
      g_free(s2);
    }
  p += g_snprintf(buf, MYSQL_BUFFER_SIZE, "%s -m %s -n %d ", 
		  gfx ? "-g" : "", s, nbturn);
  g_free(s);
  for (play = i = 0; match->wids[i] != NULL; i++)
    {
      s2 = g_strdup(gtk_entry_get_text(GTK_ENTRY(match->wids[i])));
      if (strcmp(s2, "<Aucun>"))
	{
	  players_id[play] = mysql_get_int(gl_config->mysql,
					   "SELECT id "
					   "FROM Champions WHERE Nom='%s'", 
					   s2);
	  s = mysql_get_str(gl_config->mysql,
			    "SELECT fichier FROM Champions WHERE id='%d'", 
			    players_id[play]);
	  play++;
	  for (j = 0; s2[j]; j++)
	    if (!isalnum(s2[j]) && (strchr(ALLOWED_CHARS, s2[j]) == NULL))
	      s2[j] = '_';
	  p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), "%s:%s ", s2, s);
	  g_free(s);
	}
      g_free(s2);
    }
  nbp = play;
  g_snprintf(buf2, MYSQL_BUFFER_SIZE, 
	     "INSERT INTO Matchs SET map=%d, host='%s', port=%d, statut=1,"
             "                       joueurs=%d, auteur=%d, public=%d",
	     match->mapid, 
	     (gl_config->host == NULL) ? gl_config->hostname : gl_config->host,
	     port, nbp, getuid(), pub);
  mysql_query_gtk(gl_config->mysql, buf2);
  id = mysql_insert_id(gl_config->mysql);
  s = gtk_entry_get_text(GTK_ENTRY(match->cmd));
  p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), "%s ", s);
  if (!pub)
    p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), "-P ");
  g_free(match->wids);
  g_free(match);
  do_run_match(buf, gfx, port, id, nbp, players_id);
}

GtkWidget *start_match_box()
{
  match_t *mat;
  GtkWidget *dialog = NULL, *tbl = NULL, *wid = NULL;
  GList *l, *champ = NULL, *maps, *prgs;
  char buf[MYSQL_BUFFER_SIZE];
  int nbp, nbc;
  int i, id;

  maps = GTK_CLIST(gl_config->mapl)->selection;
  if (maps == NULL)
    {
      wid = create_label("Aucune carte sélectionnée.");
      return create_dialog("Erreur", wid, NULL, " Fermer ", NULL, NULL, 
			   destroy_widget);
    }

  prgs = GTK_CLIST(gl_config->prgl)->selection;
  if (prgs == NULL)
    {
      wid = create_label("Aucun champion sélectionné.");
      return create_dialog("Erreur", wid, NULL, " Fermer ", NULL, NULL, 
			   destroy_widget);
    }

  mat = g_malloc(sizeof(*mat));
  for (l = prgs; l != NULL; l = l->next)
    {
      id = *((int *) gtk_clist_get_row_data(GTK_CLIST(gl_config->prgl), 
					    GPOINTER_TO_INT(l->data)));
      champ = g_list_append(champ, mysql_get_str(gl_config->mysql,
						 "SELECT nom FROM Champions "
						 "WHERE id=%d", id));
    }
  champ = g_list_append(champ, g_strdup("<Aucun>"));

  nbc = g_list_length(prgs);
  mat->mapid = *((int *) gtk_clist_get_row_data(GTK_CLIST(gl_config->mapl), 
						GPOINTER_TO_INT(maps->data)));
  nbp = mysql_get_int(gl_config->mysql,
		      "SELECT joueurs FROM Maps where id=%d", mat->mapid);
  mat->wids = g_malloc(sizeof(*(mat->wids)) * (nbp + 1));

  tbl = gtk_table_new(4, nbp + 1, FALSE);
  gtk_widget_show(tbl);
  dialog = create_dialog("Lancement d'une partie", tbl, " Valider ", 
			 " Annuler ", run_match_commit, mat, destroy_widget);

  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label("Graphismes: "),
			    0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label(" Port: "),
			    2, 3, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label(" Tours: "),
			    0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label(" Publique: "), 
			    2, 3, 1, 2);
  mat->gfx = create_check(TRUE);
  gtk_tooltips_set_tip(gl_config->tooltips, mat->gfx, 
		       "Lancer un client graphique", "");
  mat->port = create_spin(1024, 65535, 4242);
  gtk_tooltips_set_tip(gl_config->tooltips, mat->port, 
		       "Port d'écoute du serveur", "");
  mat->public = create_check(TRUE);
  gtk_tooltips_set_tip(gl_config->tooltips, mat->public, 
		       "Autorisé les connexions d'autres clients graphiques ?",
		       "");
  mat->turns = create_spin(1, 5000, 200);
  gtk_tooltips_set_tip(gl_config->tooltips, mat->turns, 
		       "Nombre de tours que durera la partie", "");
  gtk_table_attach_defaults(GTK_TABLE(tbl), mat->gfx, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(tbl), mat->port, 3, 4, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(tbl), mat->turns, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(tbl), mat->public, 3, 4, 1, 2);

  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label("Paramètres: "),
			    0, 1, 2, 3);
  mat->cmd = create_entry(NULL, 100);
  gtk_signal_connect(GTK_OBJECT(mat->cmd), "activate", 
		     run_match_commit, mat);
  gtk_signal_connect_after(GTK_OBJECT(mat->cmd), "activate", 
			   destroy_widget, dialog);
  gtk_tooltips_set_tip(gl_config->tooltips, mat->cmd, 
		       "Paramètres supplémentaires à passer au serveur", "");
  gtk_table_attach_defaults(GTK_TABLE(tbl), mat->cmd, 1, 4, 2, 3);
  wid = gtk_hseparator_new();
  gtk_widget_show(wid);
  gtk_table_attach(GTK_TABLE(tbl), wid, 0, 4, 3, 4, GTK_FILL, GTK_FILL, 2, 5);

  for (i = 0, l = champ; i < nbp; i++)
    {
      snprintf(buf, MYSQL_BUFFER_SIZE, "Joueur n°%d: ", i);
      gtk_table_attach_defaults(GTK_TABLE(tbl), create_label(buf),
				0, 1, i + 4, i + 5);
      wid = gtk_combo_new();
      gtk_widget_show(wid);
      gtk_tooltips_set_tip(gl_config->tooltips, GTK_COMBO(wid)->entry, 
			   "Choix des champions à utiliser", "");
      gtk_combo_set_popdown_strings(GTK_COMBO(wid), champ);
      gtk_combo_set_value_in_list(GTK_COMBO(wid), TRUE, TRUE);
      gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(wid)->entry), l->data);
      gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(wid)->entry), FALSE);
      gtk_table_attach_defaults(GTK_TABLE(tbl), wid, 1, 4, i + 4, i + 5);
      mat->wids[i] = GTK_COMBO(wid)->entry;
      if (l->next != NULL)
	l = l->next;
    }
  mat->wids[nbp] = NULL;

  for (l = champ; l != NULL; l = l->next)
    g_free(l->data);
  g_list_free(champ);

  return dialog;
}

