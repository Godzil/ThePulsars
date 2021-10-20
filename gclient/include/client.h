#ifndef __PROLO_CLIENT_H__
#define __PROLO_CLIENT_H__

#define MINI_MAP_SIZE 200
#define MAP_SIZE 500
#define WANT_PROTOCOL_VERSION 1
#define DIVID_PULSE_MINI 1/* 6 */
#define PULSE_SAT 0xFF/*7F*/
#define SPRITE_COLOR_DIV 4

#include "../netlib/net.h"
#include "../general.h"
#include "../gtkutil/gtkutil.h"

typedef enum _obj_type_t
{
    obj_r4d2 = 0, obj_akx = 1
} obj_type_t;

typedef enum _obj_act_t
{
    act_akx_move = 0, act_akx_pulse = 1,
    act_akx_link = 2
} obj_act_t;

#define    SQR(X) ((X) * (X))

typedef struct _image_t
{
    int sx;
    int sy;
    guchar *data;/* en 32bits R8 G8 B8 A8*/
    char *name;
} image_t;

typedef struct _player_t
{
    int id;
    char *name;
    float score;
    guchar colors[3];
    image_t *r4d2;
    image_t *akx;
    GdkColor *gdkcol;
} player_t;


typedef struct _obj_t
{
    int id;
    obj_type_t type;
    float posx, posy;
    int team_id;
    bool changed;

    obj_act_t action;
    int target;
    float tx, ty;
    float angle;
} obj_t;

typedef struct _map_t
{
    int nb_players;
    int nb_turns, turn_num;
    int nb_objects, nb_akx, nb_r4d2;
    float sizex, sizey;
    player_t **players;
    obj_t **objects;
    char *sand_file;
    image_t *background;
} map_t;

typedef struct _turn_t
{
    int nbt;
    int spd;
    int gtkto;
    int st_turns;
    enum
    {
        tt_pause, tt_skip, tt_fwd
    } mode;
} turn_t;

typedef struct _stat_t
{
    GtkWidget *pl, *stat, *r4d2, *akx, *menu;
    bool on;
} stat_t;

typedef struct _conf_t
{
    int socket;
    FILE *file;
    bool debug;
    const char *host;
    int port;
    GtkWidget *status, *window, *conf;
    GtkWidget *main, *mini, *pl, *turn_bar;
    GtkTooltips *tooltips;
    map_t *map;
    int gdk_io_id;
    bool ready, connected, terminated;
    turn_t turn;
    stat_t stat;

    int big_ofs_x, big_ofs_y;
    int mini_center_x, mini_center_y;
    float big_zoom, log_big_zoom;
    bool big_radar;
    bool big_pulse;
    guchar *screen;
    guchar *convtb;

    image_t *r4d2;
    image_t *akx;
    image_t *background;
} conf_t;

extern conf_t *gl_config;

/*
** Init
*/
conf_t *init(int argc, char **argv);

void create_gtk_stuff(conf_t *conf);

void byebye(GtkWidget *widget, conf_t *conf);

void network_error(net_type_t get, net_type_t wanted, void *data);

void do_connect(conf_t *conf);

gint idle_connect(void *conf);  /* Always returns FALSE for gtk_idle */
void callback_connect(GtkWidget *widget, void *conf);

void send_goodbye(conf_t *conf);

void load_map(conf_t *conf);

void destroy_map(conf_t *conf);

void get_object_param(int fd, obj_t *obj, conf_t *conf);

void allow_new_turn(conf_t *conf);

void close_socket(conf_t *conf);

void protocol_error(conf_t *conf, const char *msg);

bool tb_new_turn(void *conf);

guchar *gen_minimap(const conf_t *conf, bool force);

guchar *gen_bigmap(conf_t *conf, bool force);

guchar *gen_bigmap2(conf_t *conf, bool force);

void gen_colors(int id, int nbr, player_t *play);

void scroll_bigmap(conf_t *conf, float x, float y);

image_t *load_picture(const char *filename, const char *mask);

void destroy_picture(image_t *pic);

image_t *tint_picture(const image_t *src, guchar *col);

image_t *dup_picture(const image_t *src);

int fast_sqrt(unsigned int a);

#include "client_gtk.h"

#endif
