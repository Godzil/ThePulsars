/* $Id: matchs.c,v 1.13 2001/05/01 13:34:20 kilobug Exp $ */

#include "mclient.h"

int mat_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
{
  switch (clist->sort_column)
    {
    case 0: case 1: case 2:
      return clist_compare_str(clist, ptr1, ptr2);
    case 3:
      return clist_compare_float(clist, ptr1, ptr2);
    }
  return 0;
}

void mat_refresh(GtkWidget *wid, void *data)
{
  static sql_list_t *lst = NULL;
  char *buf;

  if (lst == NULL)
    {
      buf = g_malloc(MYSQL_BUFFER_SIZE);
      g_snprintf(buf, MYSQL_BUFFER_SIZE,
		 "SELECT m.id, CONCAT(m.host, ':', m.port), b.pseudo, map.nom,"
                 "       m.joueurs "
		 "FROM   Maps map, Matchs m, Bitmaps b "
		 "WHERE  (b.id = m.auteur) AND (map.id = m.map) "
		 "  AND  (m.statut = 1) "
                 "  AND  ((m.public) OR (m.auteur = %d) OR (%d = 1))",
		 getuid(), gl_config->staff);
      lst = mysql_list_make(buf, 5, GTK_CLIST(gl_config->matl), -1);
    }
  mysql_list_refresh(lst, NULL);
}

void mat_connect(GtkWidget *wid, void *data)
{
  GList *l;
  char *s;
  char buf[12];
  int i, id;

  for (l = GTK_CLIST(gl_config->matl)->selection; l != NULL; l = l->next)
    {
      id = *((int *) gtk_clist_get_row_data(GTK_CLIST(gl_config->matl), 
					    GPOINTER_TO_INT(l->data)));
      i = mysql_get_int(gl_config->mysql, 
			"SELECT port FROM Matchs WHERE id = %d", 
			id);
      s = mysql_get_str(gl_config->mysql, 
			"SELECT host FROM Matchs WHERE id = %d", 
			id);
      g_snprintf(buf, 12, "%d", i);
      if (!fork())
	{	  
	  execlp(gl_config->gfx_client, gl_config->gfx_client, 
		 "-h", s, "-p", buf, NULL);
	  _exit(0);
	}
      g_free(s);
    }
}

