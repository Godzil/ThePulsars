/* $Id: tournois.c,v 1.4 2001/05/07 02:41:36 kilobug Exp $ */

#include "mclient.h"
#include "tournois.h"

typedef struct _tournois_t
{
  int mapid, nbc;
  GtkWidget *nbp, *nbt, *nbm, *methode, *cmd;
  int *champ;
} tournois_t;

static void kill_trn_window(GtkWidget *wid, rt_t *rt)
{
  if (rt->dialog == NULL)
    return;
  gtk_widget_destroy(rt->dialog);
  rt->dialog = NULL;
  rt->textarea = NULL;  
  rt->disp = FALSE;
}

static void kill_trn(GtkWidget *wid, rt_t *rt)
{
  rt->nbm = 0;
}

static char *trn_met[] = { "Combinatoire", "Aléatoire", NULL, "Rotatif",
			   "Pyramidal", NULL };

static void run_trn_commit(GtkWidget *wid, tournois_t *trn)
{
  rt_t *rt;
  char *s;
  int i;
  GtkWidget *sw, *dial, *widget, *hbox;
  GtkScrolledWindow *scrw;

  rt = g_malloc(sizeof(*rt));
  rt->mapid = trn->mapid;
  rt->nbc = trn->nbc;
  rt->champ = trn->champ;
  rt->cmd = g_strdup(gtk_entry_get_text(GTK_ENTRY(trn->cmd)));
  set_data_from_spin(trn->nbp, &rt->nbp);
  set_data_from_spin(trn->nbt, &rt->nbt);
  set_data_from_spin(trn->nbm, &rt->nbm);
  s = gtk_entry_get_text(GTK_ENTRY(trn->methode));
  for (i = 0; trn_met[i] != NULL; i++)
    if (!strcmp(trn_met[i], s))
      break;
  rt->scores = g_malloc(sizeof(*rt->scores) * rt->nbp);
  rt->methode = i;
  rt->disp = rt->run = TRUE;
  rt->cur = 0;
  rt->buf = g_strdup("");
  rt->chmp = g_malloc(sizeof(int) * rt->nbp);
  s = mysql_get_str(gl_config->mysql,
		    "SELECT fichier FROM Maps "
		    "WHERE id=%d", rt->mapid);
  if (!g_path_is_absolute(s))
    {
      rt->map = g_strconcat(getenv("PROLO_DATA_DIR"), 
			    G_DIR_SEPARATOR_S, s, NULL);
      g_free(s);
    }
  else
    rt->map = s;

  trn_init(rt);

  /* Main box */
  dial = rt->dialog = gtk_dialog_new();
  gtk_widget_hide(dial);
  gtk_signal_connect(GTK_OBJECT(dial), "destroy", 
		     GTK_SIGNAL_FUNC(kill_trn_window), rt);
  gtk_window_set_title(GTK_WINDOW(dial), "Suivi d'un tournois");
  gtk_container_set_border_width(GTK_CONTAINER(&(GTK_DIALOG(dial)->window)), 
				 7);
  sw = gtk_scrolled_window_new(NULL, NULL);
  scrw = GTK_SCROLLED_WINDOW(sw);
  rt->textarea = gtk_text_new(gtk_scrolled_window_get_hadjustment(scrw),
			      gtk_scrolled_window_get_vadjustment(scrw));
  gtk_text_set_word_wrap(GTK_TEXT(rt->textarea), FALSE);
  gtk_text_set_line_wrap(GTK_TEXT(rt->textarea), TRUE);
  gtk_text_set_editable(GTK_TEXT(rt->textarea), FALSE);
  gtk_widget_show(sw);
  gtk_scrolled_window_set_policy(scrw, GTK_POLICY_AUTOMATIC, 
				 GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(sw), rt->textarea);
  gtk_widget_show(rt->textarea);

  /* Buttons */
  hbox = gtk_hbox_new(FALSE, 3);
  gtk_widget_show(hbox);
  widget = gtk_button_new_with_label(" Arrêter ");
  gtk_widget_show(widget);
  gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
		     GTK_SIGNAL_FUNC(kill_trn), rt);
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
  widget = gtk_button_new_with_label(" Fermer ");
  gtk_widget_show(widget);
  gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
		     GTK_SIGNAL_FUNC(kill_trn_window), rt);
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dial)->action_area), hbox, TRUE,
		     TRUE, 9);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dial)->vbox), sw, TRUE,
		     TRUE, 9);
  
  gtk_window_set_default_size(GTK_WINDOW(&(GTK_DIALOG(dial)->window)),
			      340, 280);
  gtk_widget_show(rt->dialog);
}

GtkWidget *start_tournois_box()
{
  tournois_t *trn;
  GtkWidget *dialog = NULL, *tbl = NULL, *wid = NULL;
  GList *l, *maps, *prgs;
  int nbp, i;

  if (!gl_config->staff)
    {
      wid = create_label("Fonctionnalité réservée aux organisateurs.");
      return create_dialog("Lancement d'un trounois", wid, NULL,
			   " Annuler ", NULL, NULL, destroy_widget);      
    }

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
  trn = g_malloc(sizeof(*trn));
  trn->nbc = g_list_length(prgs);
  trn->champ = g_malloc(sizeof(*(trn->champ)) * trn->nbc);
  for (i = 0, l = prgs; l != NULL; l = l->next, i++)
    trn->champ[i] = *((int *) 
		      gtk_clist_get_row_data(GTK_CLIST(gl_config->prgl), 
					     GPOINTER_TO_INT(l->data)));

  trn->mapid = *((int *) gtk_clist_get_row_data(GTK_CLIST(gl_config->mapl), 
						GPOINTER_TO_INT(maps->data)));
  nbp = mysql_get_int(gl_config->mysql,
		      "SELECT joueurs FROM Maps where id=%d", trn->mapid);

  tbl = gtk_table_new(2, 5, FALSE);
  gtk_widget_show(tbl);
  dialog = create_dialog("Lancement d'un tournois", tbl, " Valider ", 
			 " Annuler ", run_trn_commit, trn, destroy_widget);

  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label(" Tours: "),
			    0, 1, 0, 1);
  trn->nbt = create_spin(1, 5000, 200);
  gtk_tooltips_set_tip(gl_config->tooltips, trn->nbt, 
		       "Nombre de tours que dureront les parties", "");
  gtk_table_attach_defaults(GTK_TABLE(tbl), trn->nbt, 1, 2, 0, 1);

  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label(" Joueurs: "),
			    0, 1, 1, 2);
  trn->nbp = create_spin(1, MIN(nbp, trn->nbc), MIN(nbp, trn->nbc));
  gtk_tooltips_set_tip(gl_config->tooltips, trn->nbp, 
		       "Nombre de joueurs par partie", "");
  gtk_table_attach_defaults(GTK_TABLE(tbl), trn->nbp, 1, 2, 1, 2);

  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label(" Nb matchs: "),
			    0, 1, 3, 4);
  trn->nbm = create_spin(1, 10000, trn->nbc * 2);
  gtk_tooltips_set_tip(gl_config->tooltips, trn->nbm, 
		       "Nombre de matchs maximum", "");
  gtk_table_attach_defaults(GTK_TABLE(tbl), trn->nbm, 1, 2, 3, 4);

  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label("Paramètres: "),
			    0, 1, 4, 5);
  trn->cmd = create_entry(NULL, 100);
  gtk_signal_connect(GTK_OBJECT(trn->cmd), "activate", 
		     run_trn_commit, trn);
  gtk_signal_connect_after(GTK_OBJECT(trn->cmd), "activate", 
			   destroy_widget, dialog);
  gtk_tooltips_set_tip(gl_config->tooltips, trn->cmd, 
		       "Paramètres supplémentaires à passer au serveur", "");
  gtk_table_attach_defaults(GTK_TABLE(tbl), trn->cmd, 1, 2, 4, 5);

  l = NULL;
  for (i = 0; trn_met[i] != NULL; i++)
    l = g_list_append(l, trn_met[i]);
  gtk_table_attach_defaults(GTK_TABLE(tbl), create_label(" Méthode: "),
			    0, 1, 2, 3);  
  wid = gtk_combo_new();
  gtk_widget_show(wid);
  gtk_tooltips_set_tip(gl_config->tooltips, GTK_COMBO(wid)->entry, 
		       "Méthode d'exécution des matchs", "");
  gtk_combo_set_popdown_strings(GTK_COMBO(wid), l);
  gtk_combo_set_value_in_list(GTK_COMBO(wid), TRUE, TRUE);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(wid)->entry), l->data);
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(wid)->entry), FALSE);
  gtk_table_attach_defaults(GTK_TABLE(tbl), wid, 1, 2, 2, 3);
  trn->methode = GTK_COMBO(wid)->entry;
  g_list_free(l);

  gtk_object_set_data_full(GTK_OBJECT(dialog), "42", trn, 
			   (GtkDestroyNotify) free);

  return dialog;
}

