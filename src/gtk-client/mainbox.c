/* $Id: mainbox.c,v 1.13 2001/04/29 22:33:21 kilobug Exp $ */

#include "client.h"

void byebye(GtkWidget *widget, conf_t *conf)
{
  if (conf->debug && conf->connected)
    {
      conf->ready = TRUE;
      conf->terminated = TRUE;
      allow_new_turn(conf);      
    }
  gtk_main_quit();
}

static void create_mainbox(conf_t *conf)
{
  GtkWidget *table, *widget;

  conf->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(conf->window), 
			      MAP_SIZE + MINI_MAP_SIZE + 10,
			      MAP_SIZE + 20);
  gtk_signal_connect(GTK_OBJECT(conf->window), "destroy", 
		     GTK_SIGNAL_FUNC(byebye), conf);
  gtk_window_set_title(GTK_WINDOW(conf->window), "Finale Prologin 2001");
  gtk_window_set_policy(GTK_WINDOW(conf->window), FALSE, FALSE, TRUE);
  gtk_widget_show(conf->window);

  table = gtk_table_new(3, 4, FALSE);
  gtk_container_add(GTK_CONTAINER(conf->window), table);

  conf->tooltips = gtk_tooltips_new();
  conf->status = create_statusbar(conf);
  gtk_table_attach_defaults(GTK_TABLE(table), conf->status, 0, 3, 3, 4);
  widget = create_menubar(conf);
  gtk_table_attach_defaults(GTK_TABLE(table), widget, 0, 3, 0, 1);
  widget = create_toolbar(conf);
  gtk_table_attach_defaults(GTK_TABLE(table), widget, 2, 3, 1, 3);
  widget = create_game_area(conf);
  gtk_table_attach_defaults(GTK_TABLE(table), widget, 0, 2, 1, 3);

  gtk_widget_show(table);
}

static void create_connect(conf_t *conf)
{
  GtkWidget *wid, *table;

  table = gtk_table_new(2, 3, FALSE);
  gtk_widget_show(table);  

  gtk_table_attach_defaults(GTK_TABLE(table), create_label("Machine: "),
			    0, 1, 0, 1);
  wid = create_entry(conf->host, 80);
  gtk_signal_connect(GTK_OBJECT(wid), "changed", set_data_from_entry,
		     &(conf->host));
  gtk_signal_connect(GTK_OBJECT(wid), "activate",
		     GTK_SIGNAL_FUNC(callback_connect), conf);
  gtk_tooltips_set_tip(conf->tooltips, wid, "Machine où se trouve le seveur",
		       "");
  gtk_signal_connect(GTK_OBJECT(wid), "activate",
		     GTK_SIGNAL_FUNC(callback_connect), conf);
  gtk_signal_connect_after(GTK_OBJECT(wid), "activate", hide_widget, 
			   conf->conf);
  gtk_table_attach_defaults(GTK_TABLE(table), wid, 1, 2, 0, 1);

  gtk_table_attach_defaults(GTK_TABLE(table), create_label("Port: "),
			    0, 1, 1, 2);
  wid = create_spin(1024, 65535, conf->port);
  gtk_tooltips_set_tip(conf->tooltips, wid, "Port sur lequel le seveur écoute",
		       "");
  gtk_signal_connect(GTK_OBJECT(wid), "changed", set_data_from_spin,
		     &(conf->port));
  gtk_table_attach_defaults(GTK_TABLE(table), wid, 1, 2, 1, 2);
  conf->conf = create_dialog("Connexion à un serveur", table,
			     " Se connecter ", " Annuler ", 
			     GTK_SIGNAL_FUNC(callback_connect), conf,
			     hide_widget);
}

void create_gtk_stuff(conf_t *conf)
{
  create_mainbox(conf);
  create_connect(conf);
  create_stats(conf);
  gtk_widget_show(conf->window);
}
