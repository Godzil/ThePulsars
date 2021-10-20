#ifndef THE_PULSAR_MCLIENT_H
#define THE_PULSAR_MCLIENT_H

typedef struct _conf_t conf_t;

#include "../general.h"
#include "../gtkutil/gtkutil.h"
#include "mysql.h"

#define STATUT_ABSENT 0
#define STATUT_PRESENT 1
#define STATUT_ZLOCKE 2

#define NOTEBOOK_PAGE_NUM 4

extern conf_t *gl_config;

struct _conf_t
{
    MYSQL *mysql;
    GtkWidget *bmpl, *mapl, *prgl, *result, *matl;  /* CList */
    sql_add_t *mapa, *prga;
    GtkWidget *window;
    GtkWidget *menunb[NOTEBOOK_PAGE_NUM];
    GtkNotebook *notebook;
    GtkTooltips *tooltips;
    char *hostname;
    const char *bindir;
    const char *gfx_client, *server;
    const char *db, *host;
    bool staff;
};

void init(int argc, char **argv);

void set_my_status(int stat);

void byebye(GtkWidget *widget, void *data);

void create_gtk_stuff();

GtkWidget *create_menubar();

int bmp_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2);

void bmp_refresh(GtkWidget *wid, void *data);

void bmp_send_message(GtkWidget *wid, void *data);

int map_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2);

void map_refresh(GtkWidget *wid, void *data);

void map_add(GtkWidget *wid, void *data);

void map_remove(GtkWidget *wid, void *data);

int prg_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2);

void prg_refresh(GtkWidget *wid, void *data);

void prg_add(GtkWidget *wid, void *data);

void prg_remove(GtkWidget *wid, void *data);

void prg_edit(GtkWidget *wid, void *data);

int mat_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2);

void mat_refresh(GtkWidget *wid, void *data);

void mat_connect(GtkWidget *wid, void *data);

GtkWidget *start_match_box();

GtkWidget *start_tournois_box();

void map_edit(GtkWidget *wid, void *data);

#endif
