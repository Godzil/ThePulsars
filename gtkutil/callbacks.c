/* $Id: callbacks.c,v 1.1 2001/04/14 20:01:15 kilobug Exp $ */

#include "gtkutil.h"

/* Destroy target widget */
void destroy_widget(GtkWidget *w, gpointer data)
{
  gtk_widget_destroy(data);
}

/* Hide target widget */
void hide_widget(GtkWidget *w, gpointer data)
{
  gtk_widget_hide(data);  
}

/* Free target pointer */
void free_callback(GtkObject *obj, void *data)
{
  if (data != NULL)
    g_free(data);
}

void set_data_from_entry(GtkWidget *w, char **data)
{
  if (*data != NULL)
    g_free(*data);
  *data = strdup(gtk_entry_get_text(GTK_ENTRY(w)));
}

void set_data_from_toggle(GtkWidget *w, int *data)
{
  *data = (GTK_TOGGLE_BUTTON(w)->active);
}

void set_data_from_spin(GtkWidget *w, int *data)
{
  *data = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
}

void set_data_from_adjustment(GtkAdjustment *adj, int *value)
{
  *value = adj->value;
}

void set_data_from_adjustment_float(GtkAdjustment *adj, float *value)
{
  *value = adj->value;
}

