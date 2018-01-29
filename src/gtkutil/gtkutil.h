/* $Id: gtkutil.h,v 1.3 2001/04/19 08:33:44 kilobug Exp $ */

#ifndef __LIB_GTK_UTIL_H__
#define __LIB_GTK_UTIL_H__

#include "../general.h"

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* General callbacks */
void destroy_widget(GtkWidget *w, gpointer data);
void hide_widget(GtkWidget *w, gpointer data);
void set_data_from_entry(GtkWidget *w, char **data);
void set_data_from_toggle(GtkWidget *w, int *data);
void set_data_from_spin(GtkWidget *w, int *data);
void set_data_from_adjustment(GtkAdjustment *adj, int *value);
void set_data_from_adjustment_float(GtkAdjustment *adj, float *value);
void free_callback(GtkObject *obj, void *data);

/* General creators */
GtkWidget *create_frame(const char *label);
GtkWidget *create_label(const char *label);
GtkWidget *create_hscale(int min, int max, int *value);
GtkWidget *create_hscale_float(float min, float max, float *value);
GtkWidget *create_clist(int nbcol, int defsort, ...);
GtkWidget *create_spin(int min, int max, int def);
GtkWidget *create_check(gboolean def);
GtkWidget *create_entry(const char *def, int len);
GtkWidget *create_dialog(const char *title, GtkWidget *main_area, 
			 const char *msg1, const char *msg2,
			 GtkSignalFunc callback, void *data,
			 GtkSignalFunc destroy);
GtkWidget *create_filesel_button(const char *label, const char *title, 
				 GtkEntry *entry);

/* Special */
int clist_compare_str(GtkCList *clist, gconstpointer p1, gconstpointer p2);
int clist_compare_float(GtkCList *clist, gconstpointer p1, gconstpointer p2);
void clist_set_tooltips(GtkCList *clist, GtkTooltips *tt, ...);

#endif
