/* $Id: mysql_list.c,v 1.12 2001/04/20 10:25:13 kilobug Exp $ */

#include "mclient.h"

void mysql_list_refresh(const sql_list_t *l, const char *where)
{
  char *s;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char **tb;
  int *p, i;
  GdkColor *col;

  if (where != NULL)
    s = g_strconcat(l->query, " AND (", where, ")", NULL);
  else
    s = g_strdup(l->query);
  if (mysql_query_gtk(gl_config->mysql, s))
    return;
  g_free(s);
  res = mysql_store_result(gl_config->mysql);
  i  = mysql_num_fields(res);
  if (i != l->nb_fields)
    {
      fprintf(stderr, 
	      "Warning: SQL error: number of rows mismatch (%d / %d).\n",
	      i, l->nb_fields);
      return;
    }

  tb = g_malloc(sizeof(*tb) * (l->nb_fields - 1));
  gtk_clist_unselect_all(l->clist);
  gtk_clist_freeze(l->clist);
  gtk_clist_clear(l->clist);
  while ((row = mysql_fetch_row(res)) != NULL)
    {
      for (i = 1; i < l->nb_fields; i++)
	if (l->items[i].cvt_func != NULL)
	  tb[i - 1] = l->items[i].cvt_func(row[i], l->items[i].data);
	else
	  tb[i - 1] = row[i];
      p = g_malloc(sizeof(*p));
      *p = atoi(row[0]);
      col = NULL;
      if (l->color_func != NULL)
	col = l->color_func(l, *p, l->color_func_data);
      i = gtk_clist_append(l->clist, tb);
      gtk_clist_set_row_data_full(l->clist, i, p, g_free);
      if (col != NULL)
	{
	  gtk_clist_set_foreground(l->clist, i, col);
	  g_free(col);
	}
      for (i = 1; i < l->nb_fields; i++)
	if (l->items[i].destroy_func != NULL)
	  l->items[i].destroy_func(tb[i - 1]);
    }
  mysql_free_result(res);
  g_free(tb);

  gtk_clist_thaw(l->clist);
}

sql_list_t *mysql_list_make(const char *query, int nb_fields, GtkCList *clist,
			    ...)
{
  sql_list_t *res;
  va_list va;
  int i;

  res = g_malloc(sizeof(*res));
  res->query = query;
  res->clist = clist;
  res->color_func = NULL;
  res->color_func_data = NULL;
  res->nb_fields = nb_fields;
  res->items = g_malloc(sizeof(*(res->items)) * nb_fields);
  for (i = 0; i < nb_fields; i++)
    {
      res->items[i].destroy_func = NULL;
      res->items[i].cvt_func = NULL;
      res->items[i].data = NULL;
    }
  va_start(va, clist);
  while ((i = va_arg(va, int)) >= 0)
    {
      res->items[i].cvt_func = va_arg(va, ml_cvt_func_t *);
      res->items[i].destroy_func = va_arg(va, void *);
      res->items[i].data = va_arg(va, void *);
    }
  va_end(va);
  return res;
}

void mysql_list_set_color_func(sql_list_t *list, ml_color_func_t *callback, 
			       void *data)
{
  if (list == NULL)
    return;
  list->color_func = callback;
  list->color_func_data = data;
}

/*
** Adder
*/
static void mysql_adder_do_commit(const sql_add_t *adder, const char *before, 
				  const char *after)
{
  char buf[MYSQL_BUFFER_SIZE], *p;
  GSList *l;
  sql_add_item_t *item;
  int i;
  char *path, *apath, *s;

  if (adder->table == NULL)
    {
      adder->callback(NULL, adder);
      return;
    }
  g_snprintf(buf, MYSQL_BUFFER_SIZE,
	     "%s %s SET auteur=%d", before, adder->table, getuid());
  p = buf + strlen(buf);
  for (l = adder->items; l != NULL; l = l->next)
    {
      item = l->data;
      p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), ", %s=", item->field);
      switch (item->type)
	{
	case sql_add_str:
	  p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), 
			  "'%s'", gtk_entry_get_text(GTK_ENTRY(item->widget)));
	  break;
	case sql_add_file:
	  path = gtk_entry_get_text(GTK_ENTRY(item->widget));
	  if (g_path_is_absolute(path))
	    apath = path;
	  else
	    {
	      s = g_get_current_dir();
	      apath = g_strconcat(s, G_DIR_SEPARATOR_S, path, NULL);
	      g_free(s);
	    }
	  p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), "'%s'", apath);
	  if (apath != path)
	    g_free(apath);
	  break;
	case sql_add_num:
	  set_data_from_spin(item->widget, &i);
	  p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), "'%d'", i);
	  break;
	case sql_add_flag:
	  set_data_from_toggle(item->widget, &i);
	  p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), "'%d'", i);
	  break;
	default:
	  fprintf(stderr, 
		  "Warning: internal error in `mysql_adder_commit'.\n");
	}
    }
  p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), " %s", after);
  mysql_query_gtk(gl_config->mysql, buf);
  adder->callback(NULL, adder);
}

static void mysql_adder_commit(GtkWidget *wid, sql_add_t *adder)
{
  mysql_adder_do_commit(adder, "INSERT INTO ", "");
}

static void mysql_adder_item_create(sql_add_item_t *item, GtkWidget *tbl, 
				    int pos, 
				    GtkSignalFunc activate, void *actdata,
				    GtkSignalFunc activate2, void *actdata2)
{
  int *foo = NULL;
  char *s;
  GtkWidget *widget;

  widget = create_label(item->label);
  gtk_table_attach_defaults(GTK_TABLE(tbl), widget, 0, 1, pos, pos + 1);
  switch (item->type)
    {
    case sql_add_str:
      foo = item->param;
      item->widget = create_entry(NULL, *foo);
      gtk_table_attach_defaults(GTK_TABLE(tbl), item->widget, 
				1, 3, pos, pos + 1);
      gtk_signal_connect(GTK_OBJECT(item->widget), "activate", 
			 activate, actdata);
      gtk_signal_connect_after(GTK_OBJECT(item->widget), "activate", 
			       activate2, actdata2);
      break;
    case sql_add_num:
      foo = item->param;
      item->widget = create_spin(foo[0], foo[1], foo[2]);
      gtk_table_attach_defaults(GTK_TABLE(tbl), item->widget, 
				1, 3, pos, pos + 1);
      break;
    case sql_add_flag:
      foo = item->param;
      item->widget = create_check(*foo);
      gtk_table_attach(GTK_TABLE(tbl), item->widget, 1, 3, pos, pos + 1,
		       GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 5);
      break;
    case sql_add_file:
      s = item->param;
      item->widget = create_entry(NULL, 200);
      gtk_table_attach_defaults(GTK_TABLE(tbl), item->widget, 
				1, 2, pos, pos + 1);
      widget = create_filesel_button("Parcourir", "Parcourir...", 
				     GTK_ENTRY(item->widget));
      gtk_tooltips_set_tip(gl_config->tooltips, widget, 
			   "Choisir le fichier...", "");
      gtk_table_attach(GTK_TABLE(tbl), widget, 2, 3, pos, pos + 1,
		       GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 5, 0);
      gtk_signal_connect(GTK_OBJECT(item->widget), "activate", 
			 activate, actdata);
      gtk_signal_connect_after(GTK_OBJECT(item->widget), "activate", 
			       activate2, actdata2);
      break;
    default:
      fprintf(stderr, "Warning: internal error in `make_mysql_adder'.\n");
    }
  gtk_tooltips_set_tip(gl_config->tooltips, item->widget, item->tooltip, "");
}

static sql_add_item_t *mysql_adder_item_get(va_list *va)
{
  int *foo = NULL;
  sql_add_item_t *item;
  char *s;

  item = g_malloc(sizeof(*item));
  item->label = va_arg(*va, const char *);
  item->field = va_arg(*va, const char *);
  item->tooltip = va_arg(*va, const char *);
  item->type = va_arg(*va, int);
  switch (item->type)
    {
    case sql_add_str:
      foo = item->param = g_malloc(sizeof(int));
      *foo = va_arg(*va, int);
      break;
    case sql_add_num:
      foo = item->param = g_malloc(sizeof(int) * 3);
      foo[0] = va_arg(*va, int);
      foo[1] = va_arg(*va, int);
      foo[2] = va_arg(*va, int);
      break;
    case sql_add_flag:
      foo = item->param = g_malloc(sizeof(int));
      *foo = va_arg(*va, int);
      break;
    case sql_add_file:
      s = va_arg(*va, char *);
      item->param = s;
      break;
    }
  return item;
}

sql_add_t *mysql_adder_make(const char *table, const char *title,
			    GtkSignalFunc callback, int nb, ...)
{
  sql_add_t *res;
  GtkWidget *tbl;
  int i;
  va_list va;
  sql_add_item_t *item;

  va_start(va, nb);
  res = g_malloc(sizeof(*res));
  res->table = table;
  res->items = NULL;
  res->callback = callback;

  /* Main box */
  tbl = gtk_table_new(3, nb, FALSE);
  gtk_widget_show(tbl);
  res->widget = create_dialog(title, tbl, " Valider ", " Annuler ", 
			      mysql_adder_commit, res, hide_widget);

  /* Entrys */
  for (i = 0; i < nb; i++)
    {
      item = mysql_adder_item_get(&va);
      mysql_adder_item_create(item, tbl, i, mysql_adder_commit, res, 
			      hide_widget, res->widget);
      res->items = g_slist_prepend(res->items, item);
    }


  va_end(va);
  return res;
}

/*
** Deleter
*/
static void mysql_query_callback(GtkWidget *wid, const char *query)
{
  mysql_query_gtk(gl_config->mysql, query);
}

static void mysql_list_remove_confirm(char *query, int nb,
				      GtkSignalFunc refresh_callback)
{
  GtkWidget *dialog, *widget, *hbox;
  GtkAccelGroup *accel_group;
  char buf[MYSQL_BUFFER_SIZE];

  /* Main box */
  dialog = gtk_dialog_new();
  gtk_widget_hide(dialog);
  gtk_signal_connect(GTK_OBJECT(dialog), "destroy", 
		     GTK_SIGNAL_FUNC(gtk_widget_destroy), NULL);
  gtk_signal_connect_after(GTK_OBJECT(dialog), "destroy", 
			   GTK_SIGNAL_FUNC(free_callback), query);
  gtk_window_set_title(GTK_WINDOW(dialog), "Confirmation de suppression");
  gtk_container_set_border_width(GTK_CONTAINER(&(GTK_DIALOG(dialog)->window)),
				 7);  
  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(&(GTK_DIALOG(dialog)->window)), 
			     accel_group);

  /* Buttons */
  hbox = gtk_hbox_new(FALSE, 3);
  gtk_widget_show(hbox);
  widget = gtk_button_new_with_label(" Oui ");
  gtk_widget_add_accelerator(widget, "clicked", accel_group, GDK_Return, 0,
                              GTK_ACCEL_VISIBLE);
  gtk_widget_show(widget);
  gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
		     GTK_SIGNAL_FUNC(mysql_query_callback), query);
  gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
		     GTK_SIGNAL_FUNC(refresh_callback), NULL);
  gtk_signal_connect_after(GTK_OBJECT(widget), "clicked", 
			   GTK_SIGNAL_FUNC(destroy_widget), dialog); 
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
  widget = gtk_button_new_with_label(" Non ");
  gtk_widget_add_accelerator(widget, "clicked", accel_group, GDK_Escape, 0,
                              GTK_ACCEL_VISIBLE);
  gtk_widget_show(widget);
  gtk_signal_connect_after(GTK_OBJECT(widget), "clicked", 
			   GTK_SIGNAL_FUNC(destroy_widget), dialog); 
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), hbox, TRUE,
		     TRUE, 9);

  /* Message */
  if (nb > 1)
    g_snprintf(buf, MYSQL_BUFFER_SIZE, 
	       "Es-tu sûr de vouloir effacer ces %d éléments ?", nb);
  else
    g_snprintf(buf, MYSQL_BUFFER_SIZE, 
	       "Es-tu sûr de vouloir effacer cet élément ?");
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), create_label(buf), 
		     TRUE, TRUE, 9);
  gtk_widget_show(dialog);
}

void mysql_list_remove(const char *table, GtkCList *clist, 
		       GtkSignalFunc refresh_callback)
{
  GSList *valid = NULL, *sl;
  GList *l;
  int id, nb;
  char *buf, *p;

  for (l = clist->selection; l != NULL; l = l->next)
    {
      id = *((int *) gtk_clist_get_row_data(clist, GPOINTER_TO_INT(l->data)));
      if ((gl_config->staff) || 
	  (mysql_get_int(gl_config->mysql,
			 "SELECT auteur FROM %s WHERE id=%d", 
			 table, id) == getuid()))
	valid = g_slist_prepend(valid, GINT_TO_POINTER(id));
    }
  nb = g_slist_length(valid);
  if (nb != 0)
    {     
      p = buf = g_malloc(MYSQL_BUFFER_SIZE);
      p += g_snprintf(buf, MYSQL_BUFFER_SIZE, 
		      "UPDATE %s SET actif=0, nom=CONCAT('Z_', id, '_', nom) "
		      "WHERE 0", 
		      table);
      for (sl = valid; sl != NULL; sl = sl->next)
	{
	  id = GPOINTER_TO_INT(sl->data);
	  p += g_snprintf(p, MYSQL_BUFFER_SIZE - (p - buf), " OR id=%d", id);  
	}
      g_slist_free(valid);
      mysql_list_remove_confirm(buf, nb, refresh_callback);
    }
}

/*
** Modifier
*/
static sql_add_item_t *mysql_adder_item_duplicate(const sql_add_item_t *foo)
{
  sql_add_item_t *item;

  item = g_malloc(sizeof(*item));
  memcpy(item, foo, sizeof(*item));
  return item;
}

static void mysql_adder_item_set_value(sql_add_item_t *item, 
				       const char *table, int id)
{
  char *s;
  int i;

  switch (item->type)
    {
    case sql_add_file:
    case sql_add_str:
      s = mysql_get_str(gl_config->mysql, "SELECT %s FROM %s WHERE id=%d",
			item->field, table, id);
      gtk_entry_set_text(GTK_ENTRY(item->widget), s);
      g_free(s);
      break;
    case sql_add_num:
      i = mysql_get_int(gl_config->mysql, "SELECT %s FROM %s WHERE id=%d",
			item->field, table, id);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(item->widget), i);
      break;
    case sql_add_flag:
      i = mysql_get_int(gl_config->mysql, "SELECT %s FROM %s WHERE id=%d",
			item->field, table, id);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(item->widget), i);
      break;
    }
}

static void mysql_edit_commit(GtkWidget *wid, sql_add_t *adder)
{
  char buf[MYSQL_BUFFER_SIZE];

  g_snprintf(buf, MYSQL_BUFFER_SIZE, " WHERE id=%d", adder->id);
  mysql_adder_do_commit(adder, "UPDATE ", buf);  
}

static void mysql_edit_free(sql_add_t *adder)
{
  g_slist_free(adder->items);
  g_free(adder);
}

static void mysql_edit_dialog(const char *title, const sql_add_t *adder,
			      GtkSignalFunc refresh_callback,
			      int id)
{
  sql_add_t *res;
  GSList *l;
  GtkWidget *tbl;
  sql_add_item_t *item;
  int i;

  res = g_malloc(sizeof(*res));
  res->table = adder->table;
  res->id = id;
  res->callback = refresh_callback;
  res->items = NULL;

  /* Main box */
  tbl = gtk_table_new(3, g_slist_length(adder->items), FALSE);
  gtk_widget_show(tbl);
  res->widget = create_dialog(title, tbl, " Valider ", " Annuler ", 
			      mysql_edit_commit, res, destroy_widget);
  gtk_object_set_data_full(GTK_OBJECT(res->widget), "42", res, 
			   (GtkDestroyNotify) mysql_edit_free);
  gtk_widget_show(res->widget);

  /* Entrys */
  for (i = 0, l = adder->items; l != NULL; l = l->next, i++)
    {
      item = mysql_adder_item_duplicate(l->data);
      mysql_adder_item_create(item, tbl, i, mysql_edit_commit, res, 
			      destroy_widget, res->widget);
      mysql_adder_item_set_value(item, adder->table, id);
      res->items = g_slist_prepend(res->items, item);
    }
}

void mysql_list_edit(const char *title, const GtkCList *sel, 
		     const sql_add_t *adder, 
		     GtkSignalFunc refresh_callback)
{
  const GList *l;
  int id;

  for (l = sel->selection; l != NULL; l = l->next)
    {
      id = *((int *) gtk_clist_get_row_data(GTK_CLIST(sel), 
					    GPOINTER_TO_INT(l->data)));
      if ((gl_config->staff) || 
	  (getuid() == mysql_get_int(gl_config->mysql, 
				     "SELECT auteur FROM %s WHERE id=%d",
				     adder->table, id)))
	{
	  mysql_edit_dialog(title, adder, refresh_callback, id);
	}
    }
}
