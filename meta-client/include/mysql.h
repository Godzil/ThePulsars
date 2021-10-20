#ifndef THE_PULSAR_MCLIENT_MYSQL_H
#define THE_PULSAR_MCLIENT_MYSQL_H

#define MYSQL_BUFFER_SIZE 1000

#include <mysql/mysql.h>

int mysql_query_gtk(MYSQL *mysql, const char *query);

int mysql_get_int(MYSQL *mysql, const char *query, ...);

char *mysql_get_str(MYSQL *mysql, const char *query, ...);

void mysql_do_query(MYSQL *mysql, const char *query, ...);

typedef struct _sql_list_t sql_list_t;

typedef char *ml_cvt_func_t(const char *val, void *data);

typedef GdkColor *ml_color_func_t(const sql_list_t *list, int id, void *data);

typedef struct _sql_item_t
{
    ml_cvt_func_t *cvt_func;
    GtkDestroyNotify destroy_func;
    void *data;
} sql_item_t;

struct _sql_list_t
{
    const char *query;
    int nb_fields;
    sql_item_t *items;
    GtkCList *clist;
    ml_color_func_t *color_func;
    void *color_func_data;
};

typedef struct _sql_add_item_t
{
    const char *field;
    enum
    {
        sql_add_str, sql_add_num, sql_add_flag, sql_add_file
    } type;
    void *param;
    GtkWidget *widget;
    const char *tooltip, *label;
} sql_add_item_t;

typedef struct _sql_add_t
{
    const char *table;
    GSList *items;
    GtkWidget *widget;
    GtkSignalFunc callback;
    int id;
} sql_add_t;

void mysql_list_set_color_func(sql_list_t *list, ml_color_func_t *callback,
                               void *data);

void mysql_list_refresh(const sql_list_t *l, const char *where);

void mysql_list_remove(const char *table, GtkCList *clist,
                       GtkSignalFunc refresh_callback);

/* Syntax: make_mysql_list(query, nb_fields, clist,
                           num1, convert_callback1, destroy_callback1, data1,
                           num2, convert_callback2, destroy_callback2, data2,
			   ..., -1); */
sql_list_t *mysql_list_make(const char *query, int nb_fields, GtkCList *clist,
                            ...);

/* Syntax: make_mysql_adder(table, window_title, callback, nb_fields,
                            label1, field_name1, tooltip, type1, { args1 },
                            label2, field_name2, tooltip, type2, { args2 },
			    ...);
    Args:
        int => min, max, def
	str => length
	file => path
	check => default
*/

sql_add_t *mysql_adder_make(const char *table, const char *title,
                            GtkSignalFunc callback, int nb, ...);

void mysql_list_edit(const char *title, const GtkCList *sel,
                     const sql_add_t *adder,
                     GtkSignalFunc refresh_callback);

#endif
