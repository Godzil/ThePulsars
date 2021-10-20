#ifndef __PROLO_CLIENT_GTK_H__
#define __PROLO_CLIENT_GTK_H__

/*
** General GTK wrappers
*/
void update_map_callback(void *conf, gint src, GdkInputCondition cond);

GtkWidget *create_picture(const char *filename, conf_t *conf);

GtkWidget *create_xpm_button(const char *filename, GtkSignalFunc callback,
                             conf_t *conf);

/*
** Statusbar
*/
void set_status(const conf_t *conf, const char *msg);

GtkWidget *create_statusbar(conf_t *conf);

/*
** Menubar
*/
GtkWidget *create_menubar(conf_t *conf);

/*
** Stats
*/
void create_stats(conf_t *conf);

void stats_refresh(conf_t *conf);

void stats_make(conf_t *conf);

void pl_make_color(GdkColor *col, int r, int g, int b);

/*
** Toolbar
*/
GtkWidget *create_toolbar(conf_t *conf);

void player_list_refresh(const conf_t *conf);

void player_list_make(conf_t *conf);

GtkWidget *create_players_list(conf_t *conf);

void tb_pause(GtkWidget *wid, conf_t *conf);

void tb_play(GtkWidget *wid, conf_t *conf);

void tb_step(GtkWidget *wid, conf_t *conf);

void tb_ffwd(GtkWidget *wid, conf_t *conf);

/*
** Gamearea
*/
GtkWidget *create_game_area(conf_t *conf);

void gamearea_refresh(GtkWidget *w, conf_t *conf);

/*
** Minimap
*/
GtkWidget *create_mini_map(conf_t *conf);

void minimap_refresh(GtkWidget *w, const conf_t *conf);

#endif
