#include "client.h"

void minimap_refresh(GtkWidget *w, const conf_t *conf)
{
    gdk_draw_rgb_32_image(w->window,
                          w->style->fg_gc[GTK_STATE_NORMAL],
                          0, 0, MINI_MAP_SIZE, MINI_MAP_SIZE,
                          GDK_RGB_DITHER_NORMAL,
                          gen_minimap(conf, TRUE),
                          MINI_MAP_SIZE * 4);
}

static gint mini_expose_event(GtkWidget *w, GdkEventExpose *e,
                              const conf_t *conf)
{
    guint offs;

    offs = ( e->area.x + e->area.y * MINI_MAP_SIZE ) * 4;
    gdk_draw_rgb_32_image(w->window,
                          w->style->fg_gc[GTK_WIDGET_STATE(w)],
                          e->area.x, e->area.y,
                          e->area.width, e->area.height,
                          GDK_RGB_DITHER_NORMAL,
                          gen_minimap(conf, FALSE) + offs,
                          MINI_MAP_SIZE * 4);
    return FALSE;
}

static void mini_press_event(GtkWidget *w, GdkEventButton *e, conf_t *conf)
{
    if (( e->button != 1 ) || ( conf == NULL ))
    {
        return;
    }
    scroll_bigmap(conf, e->x, e->y);
}

static void mini_motion_event(GtkWidget *w, GdkEventMotion *e, conf_t *conf)
{
    int x, y;
    GdkModifierType state;

    if ( conf == NULL )
    {
        return;
    }
    if ( e->is_hint )
    {
        gdk_window_get_pointer(e->window, &x, &y, &state);
    }
    else
    {
        x = e->x;
        y = e->y;
        state = e->state;
    }
    if ( !( state & GDK_BUTTON1_MASK ))
    {
        return;
    }
    scroll_bigmap(conf, x, y);
}

GtkWidget *create_mini_map(conf_t *conf)
{
    GtkWidget *frame;

    frame = create_frame("Minimap");

    conf->mini = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(conf->mini),
                          MINI_MAP_SIZE, MINI_MAP_SIZE);
    gtk_signal_connect(GTK_OBJECT(conf->mini), "expose_event",
                       (GtkSignalFunc) mini_expose_event, conf);
    gtk_signal_connect(GTK_OBJECT(conf->mini), "button_press_event",
                       (GtkSignalFunc) mini_press_event, conf);
    gtk_signal_connect(GTK_OBJECT(conf->mini), "motion_notify_event",
                       (GtkSignalFunc) mini_motion_event, conf);
    gtk_widget_set_events(conf->mini,
                          GDK_EXPOSURE_MASK
                          | GDK_LEAVE_NOTIFY_MASK
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_POINTER_MOTION_MASK
                          | GDK_POINTER_MOTION_HINT_MASK);
    gtk_widget_show(conf->mini);
    gtk_container_add(GTK_CONTAINER(frame), conf->mini);
    return frame;
}
