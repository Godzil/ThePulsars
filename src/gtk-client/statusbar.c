/* $Id: statusbar.c,v 1.4 2001/04/08 16:02:28 kilobug Exp $ */

#include "client.h"

void set_status(const conf_t *conf, const char *msg)
{
  gtk_statusbar_pop(GTK_STATUSBAR(conf->status), 42);
  gtk_statusbar_push(GTK_STATUSBAR(conf->status), 42, msg);
  gtk_main_iteration_do(FALSE);
}

GtkWidget *create_statusbar(conf_t *conf)
{
  GtkWidget *widget;

  widget = gtk_statusbar_new();
  gtk_statusbar_push(GTK_STATUSBAR(widget), 42, 
		     "Connexion en cours...");
  gtk_widget_show(widget);
  return (widget);
}
