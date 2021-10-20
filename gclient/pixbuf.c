#include "client.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

image_t *load_picture(const char *filename, const char *mask)
{
    image_t *res;
    guchar *data = NULL, *mdata = NULL;
    GdkPixbuf *pixbuf, *pbmask = NULL;
    int x, y, sx2, bpp;
    char *s;

    if ( getenv("PROLO_DATA_DIR") == NULL )
    {
        fprintf(stderr, "Variable PROLO_DATA_DIR invalide.\n");
        exit(1);
    }
    res = g_malloc(sizeof(*res));
    res->sx = res->sy = 0;
    res->data = NULL;
    if ( !g_path_is_absolute(filename))
    {
        res->name = g_strconcat(getenv("PROLO_DATA_DIR"), G_DIR_SEPARATOR_S,
                                filename, NULL);
    }
    else
    {
        res->name = g_strdup(filename);
    }
    pixbuf = gdk_pixbuf_new_from_file(res->name);
    if ( pixbuf == NULL )
    {
        fprintf(stderr, "Erreur lors du chargement de l'image: %s\n", res->name);
        g_free(res->name);
        g_free(res);
        return NULL;
    }
    if ( mask != NULL )
    {
        if ( !g_path_is_absolute(mask))
        {
            s = g_strconcat(getenv("PROLO_DATA_DIR"), G_DIR_SEPARATOR_S, mask,
                            NULL);
        }
        else
        {
            s = g_strdup(mask);
        }
        pbmask = gdk_pixbuf_new_from_file(s);
        mdata = gdk_pixbuf_get_pixels(pbmask);
        g_free(s);
    }
    res->sx = gdk_pixbuf_get_width(pixbuf);
    res->sy = gdk_pixbuf_get_height(pixbuf);
    res->data = g_malloc(res->sx * res->sy * 4);
    data = gdk_pixbuf_get_pixels(pixbuf);
    sx2 = gdk_pixbuf_get_rowstride(pixbuf);
    bpp = gdk_pixbuf_get_has_alpha(pixbuf) ? 4 : 3;
    for ( x = 0 ; x < res->sx ; x++ )
    {
        for ( y = 0 ; y < res->sy ; y++ )
        {
            memcpy(res->data + ( x + y * res->sx ) * 4, data + x * bpp + y * sx2, 3);
            if ( mdata != NULL )
            {
                res->data[( x + y * res->sx ) * 4 + 3] = mdata[x * bpp + y * sx2];
            }
            else
            {
                res->data[( x + y * res->sx ) * 4 + 3] = 0;
            }
        }
    }
    gdk_pixbuf_unref(pixbuf);
    if ( pbmask != NULL )
    {
        gdk_pixbuf_unref(pbmask);
    }
    return res;
}

void destroy_picture(image_t *pic)
{
    if ( pic == NULL )
    {
        return;
    }
    g_free(pic->data);
    g_free(pic->name);
    g_free(pic);
}

image_t *dup_picture(const image_t *src)
{
    image_t *res;
    int size;

    res = g_malloc(sizeof(*res));
    res->sx = src->sx;
    res->sy = src->sy;
    res->name = g_strdup(src->name);
    size = src->sx * src->sy * 4;
    res->data = g_malloc(size);
    memcpy(res->data, src->data, size);
    return res;
}

image_t *tint_picture(const image_t *src, guchar *col)
{
    image_t *res;
    int temp;
    int ofs, size;

    res = g_malloc(sizeof(*res));
    res->sx = src->sx;
    res->sy = src->sy;
    res->name = g_strdup(src->name);
    size = src->sx * src->sy * 4;
    res->data = g_malloc(size);
    for ( ofs = 0 ; ofs < size ; ofs += 4 )
    {
        if ( src->data[ofs + 3] == 0 )
        {
            temp = col[2] / SPRITE_COLOR_DIV + (int) src->data[ofs];
            if ( temp > 0xFF )
            {
                temp = 0xFF;
            }
            res->data[ofs] = (guchar) temp;

            temp = col[1] / SPRITE_COLOR_DIV + (int) src->data[ofs + 1];
            if ( temp > 0xFF )
            {
                temp = 0xFF;
            }
            res->data[ofs + 1] = (guchar) temp;

            temp = col[0] / SPRITE_COLOR_DIV + (int) src->data[ofs + 2];
            if ( temp > 0xFF )
            {
                temp = 0xFF;
            }
            res->data[ofs + 2] = (guchar) temp;
        }
        res->data[ofs + 3] = src->data[ofs + 3];
    }
    return res;
}
