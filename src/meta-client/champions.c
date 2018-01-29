/* $Id: champions.c,v 1.10 2001/05/07 02:41:36 kilobug Exp $ */

#include "mclient.h"

int prg_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
{
  switch (clist->sort_column)
    {
    case 0: case 1: case 4:
      return clist_compare_str(clist, ptr1, ptr2);
    case 2: case 3:
      return clist_compare_float(clist, ptr1, ptr2);
    }
  return 0;
}

static GdkColor *prg_color(const sql_list_t *l, int id, void *data)
{
  GdkColor *col = NULL;
  int auteur;

  auteur = mysql_get_int(gl_config->mysql, 
			 "SELECT auteur FROM Champions WHERE id=%d", id);
  if (auteur == getuid())
    {
      col = g_malloc(sizeof(*col));
      gdk_color_parse("#009000", col);
    }
  if (col != NULL)
    gdk_color_alloc(gdk_colormap_get_system(), col);
  return col;
}

void prg_refresh(GtkWidget *wid, void *data)
{
  static sql_list_t *lst = NULL;
  char *buf;

  if (lst == NULL)
    {
      buf = g_malloc(MYSQL_BUFFER_SIZE);
      g_snprintf(buf, MYSQL_BUFFER_SIZE,
		 "SELECT c.id, c.nom, b.pseudo, c.best, c.score, m.msg "
		 "FROM   Champions c, Bitmaps b, Msg m "
		 "WHERE  (c.auteur = b.id) AND (c.actif) AND (m.id = c.tourn)"
		 "  AND  ((c.public) OR (c.auteur = %d) OR (%d = 1))",
		 getuid(), gl_config->staff);
      lst = mysql_list_make(buf, 6, GTK_CLIST(gl_config->prgl), -1);
      mysql_list_set_color_func(lst, prg_color, NULL);
    }
  mysql_list_refresh(lst, NULL);
}

void prg_add(GtkWidget *wid, void *data)
{
  gtk_widget_show(gl_config->prga->widget);
}

void prg_remove(GtkWidget *wid, void *data)
{
  mysql_list_remove("Champions", GTK_CLIST(gl_config->prgl), prg_refresh);
}

void prg_edit(GtkWidget *wid, void *data)
{
  mysql_list_edit("Modification d'un champion", GTK_CLIST(gl_config->prgl), 
		  gl_config->prga, prg_refresh);
}
