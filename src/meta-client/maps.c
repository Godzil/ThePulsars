/* $Id: maps.c,v 1.10 2001/04/20 09:47:49 kilobug Exp $ */

#include "mclient.h"

int map_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
{
  int i = 0;
  
  switch (clist->sort_column)
    {
    case 0: case 1: case 3:
      return clist_compare_str(clist, ptr1, ptr2);
    case 2:
      return clist_compare_float(clist, ptr1, ptr2);
    case 4:
      i = clist_compare_float(clist, ptr1, ptr2);
      if (i == 0)
	i = clist_compare_str(clist, ptr1, ptr2);
      return i;
    }
  return 0;
}

static char *map_owner(const char *val, void *data)
{
  int i;

  i = atoi(val);
  if (i < 0)
    return g_strdup("Prologin");
  else
    return mysql_get_str(gl_config->mysql, 
			 "SELECT pseudo FROM Bitmaps WHERE id=%d", i);
}

static GdkColor *map_color(const sql_list_t *l, int id, void *data)
{
  GdkColor *col = NULL;
  int auteur;

  auteur = mysql_get_int(gl_config->mysql, 
			 "SELECT auteur FROM Maps WHERE id=%d", id);
  if ((auteur == -1) || (mysql_get_int(gl_config->mysql, 
				       "SELECT staff FROM Bitmaps "
				       "WHERE id=%d", auteur)))
    {
      col = g_malloc(sizeof(*col));
      gdk_color_parse("blue", col);
    }
  else
    if (auteur == getuid())
      {
	col = g_malloc(sizeof(*col));
	gdk_color_parse("#009000", col);
      }
  if (col != NULL)
    gdk_color_alloc(gdk_colormap_get_system(), col);
  return col;
}

void map_refresh(GtkWidget *wid, void *data)
{
  static sql_list_t *lst = NULL;
  char *buf;

  if (lst == NULL)
    {
      buf = g_malloc(MYSQL_BUFFER_SIZE);
      g_snprintf(buf, MYSQL_BUFFER_SIZE,
		 "SELECT id, nom, descr, joueurs, auteur, taille "
		 "FROM   Maps "
		 "WHERE  (actif) "
		 "  AND  ((public) OR (auteur = %d) OR (%d = 1))",
		 getuid(), gl_config->staff);
      lst = mysql_list_make(buf, 6, GTK_CLIST(gl_config->mapl),
			    4, map_owner, g_free, NULL, -1);      
      mysql_list_set_color_func(lst, map_color, NULL);
    }
  mysql_list_refresh(lst, NULL);
}

void map_add(GtkWidget *wid, void *data)
{
  gtk_widget_show(gl_config->mapa->widget);
}

void map_remove(GtkWidget *wid, void *data)
{
  mysql_list_remove("Maps", GTK_CLIST(gl_config->mapl), map_refresh);
}

void map_edit(GtkWidget *wid, void *data)
{
  mysql_list_edit("Modification d'une carte", GTK_CLIST(gl_config->mapl), 
		  gl_config->mapa, map_refresh);
}
