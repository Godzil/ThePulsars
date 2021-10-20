#include "client.h"

#define STAT_BUF_SIZE 42

/* Calcul des infos */
static int stat_count_objects(conf_t *conf, int id, obj_type_t type)
{
    int i, r;

    if (( type == obj_r4d2 ) && ( id == 0 ))
    {
        return 0;
    }
    r = 0;
    for ( i = 0 ; i < conf->map->nb_objects ; i++ )
    {
        if (( conf->map->objects[i]->type == type ) &&
            ( conf->map->objects[i]->team_id == id ))
        {
            r++;
        }
    }
    return r;
}

static void stat_make_buf_r4d2(obj_t *obj, char **tb)
{
    g_snprintf(tb[0], STAT_BUF_SIZE, "%d", obj->team_id);
    g_snprintf(tb[1], STAT_BUF_SIZE, "%.2f", obj->posx);
    g_snprintf(tb[2], STAT_BUF_SIZE, "%.2f", obj->posy);
}

static void stat_make_buf_akx(obj_t *obj, char **tb)
{
    g_snprintf(tb[0], STAT_BUF_SIZE, "%d", obj->team_id);
    g_snprintf(tb[1], STAT_BUF_SIZE, "%.2f", obj->posx);
    g_snprintf(tb[2], STAT_BUF_SIZE, "%.2f", obj->posy);
    switch ( obj->action )
    {
    case act_akx_move:
        g_snprintf(tb[3], STAT_BUF_SIZE, "%s", "D�placement");
        g_snprintf(tb[4], STAT_BUF_SIZE, "(%.2f ; %.2f)", obj->tx, obj->ty);
        break;
    case act_akx_pulse:
        g_snprintf(tb[3], STAT_BUF_SIZE, "%s", "Pulse");
        g_snprintf(tb[4], STAT_BUF_SIZE, "(%.2f ; %.2f)", obj->tx, obj->ty);
        break;
    case act_akx_link:
        g_snprintf(tb[3], STAT_BUF_SIZE, "%s", "Lien");
        g_snprintf(tb[4], STAT_BUF_SIZE, "%d", obj->target);
        break;
    default:
        g_snprintf(tb[3], STAT_BUF_SIZE, "%s", "Erreur");
        g_snprintf(tb[4], STAT_BUF_SIZE, "Aucune");
        break;
    }
}

static void stat_obj_color(GtkCList *clist, conf_t *conf, obj_t *obj, int j)
{
    int i;

    i = obj->team_id;
    if ( i > conf->map->nb_players )
    {
        i = 0;
    }
    gtk_clist_set_foreground(clist, j, conf->map->players[i]->gdkcol);
}

/* Remplissage des listes */
static void stat_append_player(conf_t *conf, player_t *pl, char **tb)
{
    char *s;
    int j;

    s = tb[1];
    g_snprintf(tb[0], STAT_BUF_SIZE, "%d", pl->id);
    tb[1] = pl->name;
    g_snprintf(tb[2], STAT_BUF_SIZE, "%d",
               stat_count_objects(conf, pl->id, obj_akx));
    g_snprintf(tb[3], STAT_BUF_SIZE, "%d",
               stat_count_objects(conf, pl->id, obj_r4d2));
    g_snprintf(tb[4], STAT_BUF_SIZE, "%.2f", pl->score);
    j = gtk_clist_append(GTK_CLIST(conf->stat.pl), tb);
    gtk_clist_set_row_data(GTK_CLIST(conf->stat.pl), j, pl);
    gtk_clist_set_foreground(GTK_CLIST(conf->stat.pl), j, pl->gdkcol);
    tb[1] = s;
}

static void stat_append_r4d2(conf_t *conf, obj_t *obj, char **tb)
{
    int j;

    g_snprintf(tb[0], STAT_BUF_SIZE, "%d", obj->id);
    stat_make_buf_r4d2(obj, tb + 1);
    j = gtk_clist_append(GTK_CLIST(conf->stat.r4d2), tb);
    gtk_clist_set_row_data(GTK_CLIST(conf->stat.r4d2), j, obj);
    stat_obj_color(GTK_CLIST(conf->stat.r4d2), conf, obj, j);
}

static void stat_append_akx(conf_t *conf, obj_t *obj, char **tb)
{
    int j;

    g_snprintf(tb[0], STAT_BUF_SIZE, "%d", obj->id);
    stat_make_buf_akx(obj, tb + 1);
    j = gtk_clist_append(GTK_CLIST(conf->stat.akx), tb);
    gtk_clist_set_row_data(GTK_CLIST(conf->stat.akx), j, obj);
    stat_obj_color(GTK_CLIST(conf->stat.akx), conf, obj, j);
}

void stats_make(conf_t *conf)
{
    char **tb;
    player_t *pl;
    int i;
    obj_t *obj;

    if ( conf->map->players == NULL )
    {
        return;
    }
    if ( conf->map->objects == NULL )
    {
        return;
    }
    tb = g_malloc(sizeof(*tb) * 6);
    for ( i = 0 ; i < 6 ; i++ )
    {
        tb[i] = g_malloc(STAT_BUF_SIZE);
    }

    gtk_clist_freeze(GTK_CLIST(conf->stat.pl));
    gtk_clist_clear(GTK_CLIST(conf->stat.pl));
    for ( i = 0 ; i <= conf->map->nb_players ; i++ )
    {
        pl = conf->map->players[i];
        stat_append_player(conf, pl, tb);
    }
    gtk_clist_sort(GTK_CLIST(conf->stat.pl));
    gtk_clist_thaw(GTK_CLIST(conf->stat.pl));

    gtk_clist_freeze(GTK_CLIST(conf->stat.r4d2));
    gtk_clist_freeze(GTK_CLIST(conf->stat.akx));
    gtk_clist_clear(GTK_CLIST(conf->stat.r4d2));
    gtk_clist_clear(GTK_CLIST(conf->stat.akx));
    for ( i = 0 ; i < conf->map->nb_objects ; i++ )
    {
        obj = conf->map->objects[i];
        if ( obj == NULL )
        {
            continue;
        }
        if ( obj->type == obj_akx )
        {
            stat_append_akx(conf, obj, tb);
        }
        else
        {
            stat_append_r4d2(conf, obj, tb);
        }
    }
    gtk_clist_sort(GTK_CLIST(conf->stat.r4d2));
    gtk_clist_sort(GTK_CLIST(conf->stat.akx));
    gtk_clist_thaw(GTK_CLIST(conf->stat.r4d2));
    gtk_clist_thaw(GTK_CLIST(conf->stat.akx));

    for ( i = 0 ; i < 6 ; i++ )
    {
        g_free(tb[i]);
    }
    g_free(tb);
}

/* Mise � jour des infos */
static void stat_update_player(conf_t *conf, int i, char *tb)
{
    player_t *pl;

    pl = gtk_clist_get_row_data(GTK_CLIST(conf->stat.pl), i);
    g_snprintf(tb, STAT_BUF_SIZE, "%d",
               stat_count_objects(conf, pl->id, obj_akx));
    gtk_clist_set_text(GTK_CLIST(conf->stat.pl), i, 2, tb);
    g_snprintf(tb, STAT_BUF_SIZE, "%d",
               stat_count_objects(conf, pl->id, obj_r4d2));
    gtk_clist_set_text(GTK_CLIST(conf->stat.pl), i, 3, tb);
    g_snprintf(tb, STAT_BUF_SIZE, "%.2f", pl->score);
    gtk_clist_set_text(GTK_CLIST(conf->stat.pl), i, 4, tb);
}

static void stat_update_akx(conf_t *conf, int i, char **tb)
{
    obj_t *obj;
    int j;

    obj = gtk_clist_get_row_data(GTK_CLIST(conf->stat.akx), i);
    if ( !obj->changed )
    {
        return;
    }
    stat_make_buf_akx(obj, tb);
    for ( j = 0 ; j < 5 ; j++ )
    {
        gtk_clist_set_text(GTK_CLIST(conf->stat.akx), i, j + 1, tb[j]);
    }
    stat_obj_color(GTK_CLIST(conf->stat.akx), conf, obj, i);
}

static void stat_update_r4d2(conf_t *conf, int i, char **tb)
{
    obj_t *obj;
    int j;

    obj = gtk_clist_get_row_data(GTK_CLIST(conf->stat.r4d2), i);
    if ( !obj->changed )
    {
        return;
    }
    stat_make_buf_r4d2(obj, tb);
    for ( j = 0 ; j < 3 ; j++ )
    {
        gtk_clist_set_text(GTK_CLIST(conf->stat.r4d2), i, j + 1, tb[j]);
    }
    stat_obj_color(GTK_CLIST(conf->stat.r4d2), conf, obj, i);
}

void stats_refresh(conf_t *conf)
{
    int i;
    char **tb;

    if ( conf->map->players == NULL )
    {
        return;
    }
    if ( conf->map->objects == NULL )
    {
        return;
    }
    tb = g_malloc(sizeof(*tb) * 5);
    for ( i = 0 ; i < 5 ; i++ )
    {
        tb[i] = g_malloc(STAT_BUF_SIZE);
    }

    gtk_clist_freeze(GTK_CLIST(conf->stat.pl));
    for ( i = 0 ; i <= conf->map->nb_players ; i++ )
    {
        stat_update_player(conf, i, tb[0]);
    }
    gtk_clist_sort(GTK_CLIST(conf->stat.pl));
    gtk_clist_thaw(GTK_CLIST(conf->stat.pl));

    gtk_clist_freeze(GTK_CLIST(conf->stat.akx));
    for ( i = 0 ; i < conf->map->nb_akx ; i++ )
    {
        stat_update_akx(conf, i, tb);
    }
    gtk_clist_sort(GTK_CLIST(conf->stat.akx));
    gtk_clist_thaw(GTK_CLIST(conf->stat.akx));

    gtk_clist_freeze(GTK_CLIST(conf->stat.r4d2));
    for ( i = 0 ; i < conf->map->nb_r4d2 ; i++ )
    {
        stat_update_r4d2(conf, i, tb);
    }
    gtk_clist_sort(GTK_CLIST(conf->stat.r4d2));
    gtk_clist_thaw(GTK_CLIST(conf->stat.r4d2));

    for ( i = 0 ; i < 5 ; i++ )
    {
        g_free(tb[i]);
    }
    g_free(tb);
}

/* Cr�ation des widgets */
static int players_sort(GtkCList *clist, gconstpointer p1, gconstpointer p2)
{
    switch ( clist->sort_column )
    {
    case 1:
        return clist_compare_str(clist, p1, p2);
    case 0:
    case 2:
    case 3:
    case 4:
        return clist_compare_float(clist, p1, p2);
    }
    return 0;
}

static int objects_sort(GtkCList *clist, gconstpointer p1, gconstpointer p2)
{
    switch ( clist->sort_column )
    {
    case 0:
    case 1:
    case 2:
    case 3:
        return clist_compare_float(clist, p1, p2);
    case 4:
    case 5:
        return clist_compare_str(clist, p1, p2);
    }
    return 0;
}

static void stat_hide(GtkWidget *w, conf_t *data)
{
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(data->stat.menu), FALSE);
}

void create_stats(conf_t *conf)
{
    GtkWidget *wind, *wid, *vbox, *frame, *scrolled;
    GtkAccelGroup *accel_group;

    wind = gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_window_set_default_size(GTK_WINDOW(wind), 400, 600);
    gtk_widget_hide(wind);
    gtk_container_set_border_width(GTK_CONTAINER(wind), 7);
    gtk_window_set_title(GTK_WINDOW(wind), "Informations sur le jeu");
    conf->stat.stat = wind;
    vbox = gtk_vbox_new(FALSE, 4);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(wind), vbox);

    frame = create_frame("Joueurs");
    conf->stat.pl = create_clist(5, 4,
                                 "Equipe", "Nom", "Akx", "R4D2", "Score",
                                 40, 100, 30, 30, 60,
                                 GTK_SORT_ASCENDING,
                                 GTK_SORT_ASCENDING,
                                 GTK_SORT_DESCENDING,
                                 GTK_SORT_DESCENDING,
                                 GTK_SORT_DESCENDING);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolled);
    gtk_container_add(GTK_CONTAINER(scrolled), conf->stat.pl);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(frame), scrolled);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
    gtk_clist_set_compare_func(GTK_CLIST(conf->stat.pl), players_sort);
    gtk_clist_set_auto_sort(GTK_CLIST(conf->stat.pl), FALSE);
    clist_set_tooltips(GTK_CLIST(conf->stat.pl), conf->tooltips,
                       "Trier par num�ro d'�quipe", "Trier par nom",
                       "Trier par nombre d'anakronox",
                       "Trier par nombre de r4d2", "Trier par score", NULL);

    frame = create_frame("Anakronox");
    conf->stat.akx = create_clist(6, 0,
                                  "Id", "Equipe", "X", "Y", "Action", "Cible",
                                  20, 40, 40, 40, 50, 60,
                                  GTK_SORT_ASCENDING,
                                  GTK_SORT_ASCENDING,
                                  GTK_SORT_ASCENDING,
                                  GTK_SORT_ASCENDING,
                                  GTK_SORT_ASCENDING,
                                  GTK_SORT_ASCENDING);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolled);
    gtk_container_add(GTK_CONTAINER(scrolled), conf->stat.akx);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(frame), scrolled);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
    gtk_clist_set_compare_func(GTK_CLIST(conf->stat.akx), objects_sort);
    gtk_clist_set_auto_sort(GTK_CLIST(conf->stat.akx), FALSE);
    clist_set_tooltips(GTK_CLIST(conf->stat.akx), conf->tooltips,
                       "Trier par num�ro", "Trier par �quipe",
                       "Trier par position X", "Trier par position Y",
                       "Trier par action", "Trier par cible", NULL);

    frame = create_frame("R4D2");
    conf->stat.r4d2 = create_clist(4, 0,
                                   "Id", "Equipe", "X", "Y",
                                   20, 40, 40, 40,
                                   GTK_SORT_ASCENDING,
                                   GTK_SORT_ASCENDING,
                                   GTK_SORT_ASCENDING,
                                   GTK_SORT_ASCENDING);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolled);
    gtk_container_add(GTK_CONTAINER(scrolled), conf->stat.r4d2);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(frame), scrolled);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
    gtk_clist_set_compare_func(GTK_CLIST(conf->stat.r4d2), objects_sort);
    gtk_clist_set_auto_sort(GTK_CLIST(conf->stat.r4d2), FALSE);
    clist_set_tooltips(GTK_CLIST(conf->stat.r4d2), conf->tooltips,
                       "Trier par num�ro", "Trier par �quipe",
                       "Trier par position X", "Trier par position Y", NULL);

    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(conf->stat.stat), accel_group);
    wid = gtk_button_new_with_label("Fermer");
    gtk_widget_add_accelerator(wid, "clicked", accel_group, GDK_Return, 0,
                               GTK_ACCEL_VISIBLE);
    gtk_signal_connect_object(GTK_OBJECT(wind), "delete_event",
                              GTK_SIGNAL_FUNC(gtk_button_clicked),
                              GTK_OBJECT(wid));
    gtk_widget_show(wid);
    gtk_signal_connect(GTK_OBJECT(wid), "clicked",
                       GTK_SIGNAL_FUNC(stat_hide), conf);
    gtk_box_pack_start(GTK_BOX(vbox), wid, FALSE, FALSE, 5);

}

