#include "client.h"

static int pl_sort(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
{
    switch ( clist->sort_column )
    {
    case 1:
        return clist_compare_str(clist, ptr1, ptr2);
    case 0:
    case 2:
        return clist_compare_float(clist, ptr1, ptr2);
    }
    return 0;
}

void player_list_refresh(const conf_t *conf)
{
    char score[10];
    player_t *pl;
    int i;

    if ( conf->map->players == NULL )
    {
        return;
    }
    gtk_clist_freeze(GTK_CLIST(conf->pl));
    for ( i = 0 ; i < conf->map->nb_players ; i++ )
    {
        pl = gtk_clist_get_row_data(GTK_CLIST(conf->pl), i);
        g_snprintf(score, 10, "%.2f", pl->score);
        gtk_clist_set_text(GTK_CLIST(conf->pl), i, 2, score);
    }
    gtk_clist_sort(GTK_CLIST(conf->pl));
    gtk_clist_thaw(GTK_CLIST(conf->pl));
    gtk_progress_configure(GTK_PROGRESS(conf->turn_bar), conf->map->turn_num,
                           0, conf->map->nb_turns);
}

void pl_make_color(GdkColor *col, int r, int g, int b)
{
    col->pixel = 0;
    col->red = r * 257;
    col->green = g * 257;
    col->blue = b * 257;
    gdk_colormap_alloc_color(gdk_colormap_get_system(), col, FALSE, TRUE);
}

void player_list_make(conf_t *conf)
{
    char **tb;
    char id[4], score[10];
    player_t *pl;
    int i, j;

    if ( conf->map->players == NULL )
    {
        return;
    }
    gtk_clist_freeze(GTK_CLIST(conf->pl));
    gtk_clist_clear(GTK_CLIST(conf->pl));
    tb = g_malloc(sizeof(*tb) * 4);
    tb[0] = id;
    tb[2] = score;
    tb[3] = NULL;
    for ( i = 1 ; i <= conf->map->nb_players ; i++ )
    {
        pl = conf->map->players[i];
        tb[1] = pl->name;
        g_snprintf(id, 4, "%d", pl->id);
        g_snprintf(score, 10, "%.2f", pl->score);
        j = gtk_clist_append(GTK_CLIST(conf->pl), tb);
        gtk_clist_set_row_data(GTK_CLIST(conf->pl), j, pl);
        gtk_clist_set_foreground(GTK_CLIST(conf->pl), j, pl->gdkcol);
        gtk_clist_set_compare_func(GTK_CLIST(conf->pl), pl_sort);
    }
    g_free(tb);
    gtk_clist_thaw(GTK_CLIST(conf->pl));
    gtk_progress_configure(GTK_PROGRESS(conf->turn_bar), conf->map->turn_num,
                           0, conf->map->nb_turns);
}

GtkWidget *create_players_list(conf_t *conf)
{
    GtkWidget *frame, *vbox;

    frame = create_frame("G�n�ral");
    conf->pl = create_clist(3, 2,
                            "Equipe", "Nom", "Score",
                            40, 100, 60,
                            GTK_SORT_ASCENDING,
                            GTK_SORT_ASCENDING,
                            GTK_SORT_DESCENDING);
    clist_set_tooltips(GTK_CLIST(conf->pl), conf->tooltips, "Trier par �quipe",
                       "Trier par nom", "Trier par score", NULL);

    conf->turn_bar = gtk_progress_bar_new();
    gtk_progress_set_format_string(GTK_PROGRESS(conf->turn_bar),
                                   "%v/%u (%p %%)");
    gtk_progress_set_show_text(GTK_PROGRESS(conf->turn_bar), TRUE);
    gtk_progress_configure(GTK_PROGRESS(conf->turn_bar), 0, 0, 1);
    gtk_widget_show(conf->turn_bar);

    vbox = gtk_vbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), conf->pl, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), conf->turn_bar, TRUE, TRUE, 5);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    return frame;
}
