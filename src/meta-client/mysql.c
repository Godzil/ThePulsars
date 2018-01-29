/* $Id: mysql.c,v 1.5 2001/04/12 22:45:29 kilobug Exp $ */

#include "mclient.h"

int mysql_query_gtk(MYSQL *mysql, const char *query)
{
  int i;
  GtkWidget *wi;
  char buf[MYSQL_BUFFER_SIZE];

  i = mysql_query(mysql, query);
  if (i)
    {
      g_snprintf(buf, MYSQL_BUFFER_SIZE, 
		 "> %s <\n\n"
		 "(Requète = `%s')\n", mysql_error(mysql), query);
      wi = create_dialog("Erreur SQL", create_label(buf), NULL, " Fermer ",
			 NULL, NULL, destroy_widget);
      gtk_widget_show(wi);
    }
  return i;
}

static MYSQL_RES *mysql_get_result_v(MYSQL *mysql, const char *query, 
				     va_list ap)
{
  char mysql_buffer[MYSQL_BUFFER_SIZE];

  g_vsnprintf(mysql_buffer, MYSQL_BUFFER_SIZE, query, ap);
  if (mysql_query_gtk(mysql, mysql_buffer))
    return NULL;
  return mysql_store_result(mysql);
}

static MYSQL_ROW mysql_get_row_v(MYSQL *mysql, const char *query, 
				  va_list ap)
{
  MYSQL_ROW row;
  static MYSQL_RES *res = NULL;

  if (res != NULL)
    mysql_free_result(res);
  if ((res = mysql_get_result_v(mysql, query, ap)) == NULL)
    return NULL;
  row = mysql_fetch_row(res);
  return row;
}

static char *mysql_get_str_v(MYSQL *mysql, const char *query, va_list ap)
{
  MYSQL_ROW row;

  row = mysql_get_row_v(mysql, query, ap);
  if (row == NULL)
    return g_strdup("");
  else
    return g_strdup(row[0]);
}

int mysql_get_int(MYSQL *mysql, const char *query, ...)
{
  va_list ap;
  int i;
  char *s;

  va_start(ap, query);
  s = mysql_get_str_v(mysql, query, ap);
  i = atoi(s);
  g_free(s);
  va_end(ap);
  return i;
}

char *mysql_get_str(MYSQL *mysql, const char *query, ...)
{
  va_list ap;
  char *s;

  va_start(ap, query);
  s = mysql_get_str_v(mysql, query, ap);
  va_end(ap);
  return s;
}

void mysql_do_query(MYSQL *mysql, const char *query, ...)
{
  va_list ap;
  va_start(ap, query);
  mysql_get_result_v(mysql, query, ap);
  va_end(ap);  
}
