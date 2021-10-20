#include "client.h"

#define ZOOM_ALPHA 40.0
#define ZOOM_BETA 0.4
#define ZOOM_GAMMA 1.5

void gamearea_refresh(GtkWidget *w, conf_t *conf)
{
    gdk_draw_rgb_32_image(w->window,
                          w->style->fg_gc[GTK_STATE_NORMAL],
                          0, 0, MAP_SIZE, MAP_SIZE,
                          GDK_RGB_DITHER_NORMAL,
                          gen_bigmap(conf, TRUE),
                          MAP_SIZE * 4);
}

static gint gamearea_expose_event(GtkWidget *w, GdkEventExpose *e,
                                  conf_t *conf)
{
    guint offs;

    offs = ( e->area.x + e->area.y * MAP_SIZE ) * 4;
    gdk_draw_rgb_32_image(w->window,
                          w->style->fg_gc[GTK_WIDGET_STATE(w)],
                          e->area.x, e->area.y,
                          e->area.width, e->area.height,
                          GDK_RGB_DITHER_NORMAL,
                          gen_bigmap(conf, FALSE) + offs,
                          MAP_SIZE * 4);
    return FALSE;
}

GtkWidget *create_game_area(conf_t *conf)
{
    GtkWidget *frame, *vbox;

    frame = create_frame("Terrain de jeu");
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
    conf->main = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(conf->main),
                          MAP_SIZE, MAP_SIZE);
    gtk_signal_connect(GTK_OBJECT(conf->main), "expose_event",
                       (GtkSignalFunc) gamearea_expose_event, conf);
    gtk_container_add(GTK_CONTAINER(frame), conf->main);
    gtk_widget_show(conf->main);

    return vbox;
}

void scroll_bigmap(conf_t *conf, float x, float y)
{
    float sx, sy;
    float alpha;
    int xsize, ysize;

    if ( conf->map == NULL )
    {
        return;
    }

    alpha = ZOOM_ALPHA / MAX(conf->map->sizex, conf->map->sizey);
    conf->big_zoom = alpha * exp(ZOOM_BETA * conf->log_big_zoom + ZOOM_GAMMA);

    conf->mini_center_x = x;
    conf->mini_center_y = y;
    xsize = conf->map->sizex * conf->big_zoom + 100;
    ysize = conf->map->sizey * conf->big_zoom + 100;
    sx = x * xsize / MINI_MAP_SIZE - MAP_SIZE / 2;
    sy = y * ysize / MINI_MAP_SIZE - MAP_SIZE / 2;
    conf->big_ofs_x = CLAMP((int) sx, -50, xsize - MAP_SIZE - 50);
    conf->big_ofs_y = CLAMP((int) sy, -50, ysize - MAP_SIZE - 50);
    minimap_refresh(conf->mini, conf);
    gamearea_refresh(conf->main, conf);
}
