/* $Id: gtk.c,v 1.7 2001/04/14 20:01:15 kilobug Exp $ */

#include "client.h"

GtkWidget *create_picture(const char *filename, conf_t *conf)
{
  GdkPixmap *pm;
  GdkBitmap *mask;
  GtkStyle *style;
  GtkWidget *wid;
  char *name;

  name = g_strconcat(getenv("PROLO_DATA_DIR"), G_DIR_SEPARATOR_S, 
		     filename, NULL);
  style = gtk_widget_get_style(conf->window);
  pm = gdk_pixmap_create_from_xpm(conf->window->window, &mask, 
				  &style->bg[GTK_STATE_NORMAL],
				  name);
  g_free(name);
  wid = gtk_pixmap_new(pm, mask);
  gtk_widget_show(wid);
  return wid;
}

GtkWidget *create_xpm_button(const char *filename, GtkSignalFunc callback, 
			     conf_t *conf)
{
  GtkWidget *pic, *wid;

  pic = create_picture(filename, conf);
  wid = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(wid), pic);  
  gtk_widget_show(wid);
  if (callback != NULL)
    gtk_signal_connect(GTK_OBJECT(wid), "pressed", callback, conf);
  return wid;
}

