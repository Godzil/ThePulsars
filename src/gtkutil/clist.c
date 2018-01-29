/* $Id: clist.c,v 1.4 2001/04/25 14:31:48 kilobug Exp $ */

#include "gtkutil.h"

static void list_click(GtkCList *clist, gint column, GtkSortType *sort)
{
  if (sort[0] == column)
    sort[column + 1] = !sort[column + 1];
  else
    sort[0] = column;
  gtk_clist_set_sort_type(clist, sort[column + 1]);
  gtk_clist_set_sort_column(clist, column);
  gtk_clist_sort(clist);
}

GtkWidget *create_clist(int nbcol, int defsort, ...)
{
  char **titles;
  GtkSortType *sort = NULL;
  int i;
  va_list va;
  int *width;
  GtkWidget *l;

  if (nbcol <= 0)
    return NULL;

  titles = g_malloc(nbcol * sizeof(*titles));
  va_start(va, defsort);
  for (i = 0; i < nbcol; i++)
    titles[i] = va_arg(va, char *);
  width = g_malloc(nbcol * sizeof(*width));
  for (i = 0; i < nbcol; i++)
    width[i] = va_arg(va, int);
  sort = g_malloc((nbcol + 1) * sizeof(*sort));
  for (i = 0; i < nbcol; i++)
    sort[i + 1] = va_arg(va, GtkSortType);
  sort[0] = defsort;
  
  l = gtk_clist_new_with_titles(nbcol, titles);
  g_free(titles);
  gtk_clist_set_shadow_type(GTK_CLIST(l), GTK_SHADOW_IN);
  gtk_clist_column_titles_active(GTK_CLIST(l));
  for (i = 0; i < nbcol; i++)
    gtk_clist_set_column_width(GTK_CLIST(l), i, width[i]);
  g_free(width);
  gtk_clist_set_auto_sort(GTK_CLIST(l), TRUE);
  gtk_clist_set_sort_column(GTK_CLIST(l), sort[0]);
  gtk_clist_set_sort_type(GTK_CLIST(l), sort[sort[0] + 1]);
  gtk_signal_connect(GTK_OBJECT(l), "click-column", list_click, sort);
  gtk_signal_connect_after(GTK_OBJECT(l), "destroy", free_callback, sort);
  gtk_widget_show(l);
  va_end(va);
  return l;
}

static const char *clist_comp_get_str(GtkCList *clist, gconstpointer ptr)
{
  return ((GtkCListRow *) ptr)->cell[clist->sort_column].u.text;
}

int clist_compare_str(GtkCList *clist, gconstpointer p1, gconstpointer p2)
{
  int i;  
  const char *buf1, *buf2;

  buf1 = clist_comp_get_str(clist, p1);
  buf2 = clist_comp_get_str(clist, p2);
  i = g_strcasecmp(buf1, buf2);
  if (i < 0)
    i = -42;
  if (i > 0)
    i = 42;
  return i;
}

int clist_compare_float(GtkCList *clist, gconstpointer p1, gconstpointer p2)
{
  int i;  
  const char *buf1, *buf2;

  buf1 = clist_comp_get_str(clist, p1);
  buf2 = clist_comp_get_str(clist, p2);
  i = atoi(buf1) - atoi(buf2);
  if (i < 0)
    i = -42;
  if (i > 0)
    i = 42;
  return i;
}

void clist_set_tooltips(GtkCList *clist, GtkTooltips *tt, ...)
{
  va_list va;
  const char *s;
  int i;
  GtkWidget *wid;

  va_start(va, tt);
  if ((clist != NULL) && (tt != NULL))
    for (i = 0; (s = va_arg(va, const char *)) != NULL; i++)
      {
	wid = gtk_clist_get_column_widget(clist, i);
	gtk_tooltips_set_tip(tt, wid->parent, s, ""); 
      }
  va_end(va);
}
