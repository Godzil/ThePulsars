/* $Id: bitmaps.c,v 1.5 2001/04/25 16:37:21 kilobug Exp $ */

#include "mclient.h"

#define SEND_MESSAGE "ns_send_msg"

typedef struct _msg_t
{
  GSList *recv;
  GtkWidget *dialog;
  GtkWidget *entry;
} msg_t;


static void bmp_msg_do_send(GtkWidget *wid, msg_t *msg)
{
  GSList *l;
  char buf[MYSQL_BUFFER_SIZE];

  for (l = msg->recv; l != NULL; l = l->next)
    {
      g_snprintf(buf, MYSQL_BUFFER_SIZE, "%s %s '%s'",
		 SEND_MESSAGE, (char *) l->data, 
		 gtk_entry_get_text(GTK_ENTRY(msg->entry)));
      system(buf);
    }
}

static void bmp_msg_free(msg_t *msg)
{
  GSList *l;

  for (l = msg->recv; l != NULL; l = l->next)
    g_free(l->data);
  g_slist_free(msg->recv);
  g_free(msg);
}

void bmp_send_message(GtkWidget *wid, void *data)
{
  GList *bmps, *l;
  msg_t *msg;
  int id;

  bmps = GTK_CLIST(gl_config->bmpl)->selection;
  if (bmps == NULL)
    return;
  msg = g_malloc(sizeof(*msg));
  for (msg->recv = NULL, l = bmps; l != NULL; l = l->next)
    {
      id = *((int *) gtk_clist_get_row_data(GTK_CLIST(gl_config->bmpl), 
					    GPOINTER_TO_INT(l->data)));
      msg->recv = g_slist_prepend(msg->recv, 
				  mysql_get_str(gl_config->mysql,
						"SELECT login FROM Bitmaps "
						"WHERE id=%d", id));
    }
  msg->entry = create_entry("", 200);
  msg->dialog = create_dialog("Envoyer un message", msg->entry,
			      "Envoyer", "Annuler", bmp_msg_do_send,
			      msg, destroy_widget);
  gtk_object_set_data_full(GTK_OBJECT(msg->dialog), "42", msg, 
			   (GtkDestroyNotify) bmp_msg_free);
  gtk_signal_connect(GTK_OBJECT(msg->entry), "activate", 
		     bmp_msg_do_send, msg);
  gtk_signal_connect_after(GTK_OBJECT(msg->entry), "activate", 
			   destroy_widget, msg->dialog);
  gtk_widget_show(msg->dialog);
}

int bmp_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
{
  return clist_compare_str(clist, ptr1, ptr2);
}

static char *bmp_statut(const char *val, void *data)
{
  switch (atoi(val))
    {
    case STATUT_ABSENT:
      return "Absent";
    case STATUT_PRESENT:
      return "Présent";
    case STATUT_ZLOCKE:
      return "Zlocké";
    }
  return "Inconnu";
}

static char *bmp_type(const char *val, void *data)
{
  switch (atoi(val))
    {
    case 0:
      return "Bitmap";
    default:
      return "Organisateur";
    }
}

static const char *bmp_colors[] = 
{ "#900000", "#009000", "#000090",
  "#FF0000", "#00FF00", "#0000FF" };

static GdkColor *bmp_color(const sql_list_t *l, int id, void *data)
{
  GdkColor *col = NULL;
  int i;

  if (mysql_get_int(gl_config->mysql, 
		    "SELECT staff FROM Bitmaps WHERE id=%d", id))
    i = 3;
  else 
    i = 0;
  switch (mysql_get_int(gl_config->mysql, 
			"SELECT statut FROM Bitmaps WHERE id=%d", id))
    {
    case STATUT_ABSENT:
      i += 0;
      break;
    case STATUT_ZLOCKE:
      i += 1;
      break;
    case STATUT_PRESENT:
      i += 2;
      break;
    }
  col = g_malloc(sizeof(*col));
  gdk_color_parse(bmp_colors[i], col);
  gdk_color_alloc(gdk_colormap_get_system(), col);
  return col;
}

void bmp_refresh(GtkWidget *wid, void *data)
{
  static sql_list_t *lst = NULL;

  if (lst == NULL)
    {
      lst = mysql_list_make("SELECT * FROM Bitmaps WHERE 1",
			    8, GTK_CLIST(gl_config->bmpl),
			    6, bmp_statut, NULL, NULL,
			    7, bmp_type, NULL, NULL, -1);
      mysql_list_set_color_func(lst, bmp_color, NULL);
    }
  mysql_list_refresh(lst, NULL);
}
