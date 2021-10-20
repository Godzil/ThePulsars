#include "client.h"

void tb_pause(GtkWidget *wid, conf_t *conf)
{
    if ( !conf->connected )
    {
        return;
    }
    if ( conf->turn.mode == tt_fwd )
    {
        gtk_timeout_remove(conf->turn.gtkto);
    }
    conf->turn.gtkto = 0;
    conf->turn.mode = tt_pause;
}

void tb_play(GtkWidget *wid, conf_t *conf)
{
    if ( !conf->connected )
    {
        return;
    }
    tb_pause(wid, conf);
    allow_new_turn(conf);
}

void tb_step(GtkWidget *wid, conf_t *conf)
{
    if ( !conf->connected )
    {
        return;
    }
    tb_pause(wid, conf);
    conf->turn.st_turns = conf->turn.nbt - 1;
    conf->turn.mode = tt_skip;
    allow_new_turn(conf);
}

bool tb_new_turn(void *conf)
{
    allow_new_turn(conf);
    return FALSE;
}

void tb_ffwd(GtkWidget *wid, conf_t *conf)
{
    if ( !conf->ready )
    {
        return;
    }
    conf->turn.mode = tt_fwd;
    conf->turn.gtkto = gtk_timeout_add(conf->turn.spd, tb_new_turn, conf);
}

static GtkWidget *create_the_toolbar(conf_t *conf)
{
    GtkWidget *table, *wid, *hbox, *frame;

    frame = create_frame("Commandes");
    if ( !conf->debug )
    {
        table = create_picture("logo.xpm", conf);
    }
    else
    {
        table = gtk_table_new(2, 3, FALSE);
        gtk_widget_show(table);

        hbox = gtk_hbox_new(FALSE, 4);
        gtk_widget_show(hbox);
        gtk_table_attach_defaults(GTK_TABLE(table), hbox, 0, 2, 0, 1);

        wid = create_xpm_button("pause.xpm", NULL, conf);
        gtk_tooltips_set_tip(conf->tooltips, wid,
                             "Pause: Arr�ter temporairement la simulation", "");
        gtk_box_pack_start(GTK_BOX(hbox), wid, TRUE, TRUE, 5);
        gtk_signal_connect(GTK_OBJECT(wid), "clicked", tb_pause, conf);
        wid = create_xpm_button("play.xpm", NULL, conf);
        gtk_tooltips_set_tip(conf->tooltips, wid,
                             "Play: Tour suivant", "");
        gtk_box_pack_start(GTK_BOX(hbox), wid, TRUE, TRUE, 5);
        gtk_signal_connect(GTK_OBJECT(wid), "clicked", tb_play, conf);
        wid = create_xpm_button("step.xpm", NULL, conf);
        gtk_tooltips_set_tip(conf->tooltips, wid,
                             "Step: Ex�cuter le nombre de tours sp�cifi�", "");
        gtk_box_pack_start(GTK_BOX(hbox), wid, TRUE, TRUE, 5);
        gtk_signal_connect(GTK_OBJECT(wid), "clicked", tb_step, conf);
        wid = create_xpm_button("ffwd.xpm", NULL, conf);
        gtk_tooltips_set_tip(conf->tooltips, wid,
                             "Ffwd: Lancer la simulation en continu", "");
        gtk_box_pack_start(GTK_BOX(hbox), wid, TRUE, TRUE, 5);
        gtk_signal_connect(GTK_OBJECT(wid), "clicked", tb_ffwd, conf);

        gtk_table_attach(GTK_TABLE(table),
                         create_label("Vitesse (ms): "),
                         0, 1, 1, 2, 0, GTK_FILL | GTK_EXPAND, 0, 0);
        wid = create_hscale(1, 2000, &( conf->turn.spd ));
        gtk_tooltips_set_tip(conf->tooltips, wid,
                             "D�lai entre deux tours (mode ffwd)", "");
        gtk_table_attach_defaults(GTK_TABLE(table), wid, 1, 2, 1, 2);
        gtk_table_attach(GTK_TABLE(table),
                         create_label("Pas (tours): "),
                         0, 1, 2, 3, 0, GTK_FILL | GTK_EXPAND, 0, 0);
        wid = create_hscale(0, 200, &( conf->turn.nbt ));
        gtk_tooltips_set_tip(conf->tooltips, wid,
                             "Nombres de tours � sauter (mode step)", "");
        gtk_table_attach_defaults(GTK_TABLE(table), wid, 1, 2, 2, 3);
    }
    gtk_container_add(GTK_CONTAINER(frame), table);
    return frame;
}

static void display_sprite_changed(GtkWidget *t, conf_t *conf)
{
    conf->big_radar = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(t));
    gamearea_refresh(conf->main, conf);
}

static void display_pulse_changed(GtkWidget *t, conf_t *conf)
{
    conf->big_pulse = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(t));
    gamearea_refresh(conf->main, conf);
}

static void zoom_ratio_changed(GtkWidget *t, conf_t *conf)
{
    scroll_bigmap(conf, conf->mini_center_x, conf->mini_center_y);
}

static GtkWidget *create_zoom_ratio(conf_t *conf)
{
    GtkWidget *frame, *ratio, *vbox, *wid;
    GtkAdjustment *adj;

    frame = create_frame("Contr�le de la zone principal");
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox);
    ratio = create_hscale_float(0, 10, &( conf->log_big_zoom ));
    gtk_tooltips_set_tip(conf->tooltips, ratio,
                         "Niveau de zoom",
                         "");
    adj = gtk_range_get_adjustment(GTK_RANGE(ratio));
    gtk_signal_connect_after(GTK_OBJECT(adj), "value_changed",
                             zoom_ratio_changed, conf);
    gtk_box_pack_start(GTK_BOX(vbox), ratio, TRUE, FALSE, 0);
    wid = gtk_check_button_new_with_label("Mode radar");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), FALSE);
    gtk_signal_connect(GTK_OBJECT(wid), "toggled", display_sprite_changed,
                       conf);
    gtk_widget_show(wid);
    gtk_box_pack_start(GTK_BOX(vbox), wid, TRUE, FALSE, 0);
    wid = gtk_check_button_new_with_label("Affichage des zones de pulse");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), TRUE);
    gtk_signal_connect(GTK_OBJECT(wid), "toggled", display_pulse_changed,
                       conf);
    gtk_widget_show(wid);
    gtk_box_pack_start(GTK_BOX(vbox), wid, TRUE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    return frame;
}

GtkWidget *create_toolbar(conf_t *conf)
{
    GtkWidget *table, *wid;

    table = gtk_table_new(1, 4, FALSE);
    wid = create_the_toolbar(conf);
    gtk_table_attach(GTK_TABLE(table), wid, 0, 1, 0, 1,
                     GTK_EXPAND | GTK_FILL, 0, 5, 0);
    wid = create_players_list(conf);
    gtk_table_attach(GTK_TABLE(table), wid, 0, 1, 1, 2,
                     GTK_EXPAND | GTK_FILL, 0, 5, 0);
    wid = create_mini_map(conf);
    gtk_table_attach(GTK_TABLE(table), wid, 0, 1, 2, 3, 0, 0, 0, 0);
    wid = create_zoom_ratio(conf);
    gtk_table_attach(GTK_TABLE(table), wid, 0, 1, 3, 4,
                     GTK_EXPAND | GTK_FILL, 0, 10, 0);
    gtk_widget_show(table);
    return table;
}
