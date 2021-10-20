#include "gtkutil.h"

typedef struct _filesel_t
{
    GtkEntry *entry;
    GtkWidget *filesel;
} filesel_t;

static void clean_filesel(GtkWidget *wid, filesel_t *file)
{
    if ( file->filesel != NULL )
    {
        gtk_widget_destroy(file->filesel);
    }
    g_free(file);
}

static void ok_filesel(GtkWidget *wid, filesel_t *file)
{
    char *s;

    s = gtk_file_selection_get_filename(GTK_FILE_SELECTION(file->filesel));
    gtk_entry_set_text(file->entry, s);
    gtk_widget_hide(file->filesel);
}

GtkWidget *create_filesel_button(const char *label, const char *title,
                                 GtkEntry *entry)
{
    GtkWidget *button;
    filesel_t *file;
    GtkFileSelection *fs;

    file = g_malloc(sizeof(*file));
    file->entry = entry;
    file->filesel = gtk_file_selection_new(title);
    gtk_widget_hide(file->filesel);
    fs = GTK_FILE_SELECTION(file->filesel);
    gtk_file_selection_hide_fileop_buttons(fs);
    gtk_signal_connect_object(GTK_OBJECT(fs->cancel_button), "clicked",
                              gtk_widget_hide, GTK_OBJECT(file->filesel));
    gtk_signal_connect(GTK_OBJECT(fs->ok_button), "clicked", ok_filesel, file);
    button = gtk_button_new_with_label(label);
    gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
                              gtk_widget_show, GTK_OBJECT(file->filesel));
    gtk_signal_connect_after(GTK_OBJECT(button), "destroy",
                             clean_filesel, file);
    gtk_widget_show(button);
    return button;
}
