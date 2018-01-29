/* $Id: create.c,v 1.5 2001/04/25 14:31:48 kilobug Exp $ */

#include "gtkutil.h"

/*
** Create and show items
*/
GtkWidget *create_frame(const char *label)
{
  GtkWidget *widget;
  
  widget = gtk_frame_new(label);
  gtk_frame_set_shadow_type(GTK_FRAME(widget), GTK_SHADOW_ETCHED_IN);
  gtk_frame_set_label_align(GTK_FRAME(widget), 0.5, 0);
  gtk_widget_show(widget);
  return widget;
}

GtkWidget *create_label(const char *label)
{
  GtkWidget *widget;
  
  widget = gtk_label_new(label);
  gtk_widget_show(widget);
  return widget;
}

GtkWidget *create_hscale(int min, int max, int *value)
{
  GtkObject *adj;
  GtkWidget *wid;

  adj = gtk_adjustment_new(*value, min, max, 1, 10, 20);
  wid = gtk_hscale_new(GTK_ADJUSTMENT(adj));
  gtk_signal_connect(GTK_OBJECT(adj), "value_changed", 
		     set_data_from_adjustment,  value);
  gtk_scale_set_digits(GTK_SCALE(wid), 0);
  gtk_widget_show(wid);  
  return wid;
}

GtkWidget *create_hscale_float(float min, float max, float *value)
{
  GtkObject *adj;
  GtkWidget *wid;

  adj = gtk_adjustment_new(*value, min, max, 1, 2, 0);
  wid = gtk_hscale_new(GTK_ADJUSTMENT(adj));
  gtk_signal_connect(GTK_OBJECT(adj), "value_changed", 
		     set_data_from_adjustment_float,  value);
  gtk_scale_set_digits(GTK_SCALE(wid), 2);
  gtk_widget_show(wid);  
  return wid;
}

GtkWidget *create_dialog(const char *title, GtkWidget *main_area, 
			 const char *msg1, const char *msg2,
			 GtkSignalFunc callback, void *data,
			 GtkSignalFunc destroy)
{
  GtkWidget *dialog, *hbox, *widget;
  GtkAccelGroup *accel_group;

  /* Main box */
  dialog = gtk_dialog_new();
  gtk_widget_hide(dialog);
  gtk_signal_connect(GTK_OBJECT(dialog), "destroy", 
		     GTK_SIGNAL_FUNC(destroy), dialog);
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gtk_container_set_border_width(GTK_CONTAINER(&(GTK_DIALOG(dialog)->window)),
				 7);  
  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(&(GTK_DIALOG(dialog)->window)), 
			     accel_group);
  
  /* Buttons */
  hbox = gtk_hbox_new(FALSE, 3);
  gtk_widget_show(hbox);
  if (msg1 != NULL)
    {
      widget = gtk_button_new_with_label(msg1);
      gtk_widget_add_accelerator(widget, "clicked", accel_group, GDK_Return, 0,
				 GTK_ACCEL_VISIBLE);
      gtk_widget_show(widget);
      gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
			 GTK_SIGNAL_FUNC(callback), data);
      gtk_signal_connect_after(GTK_OBJECT(widget), "clicked", 
			       GTK_SIGNAL_FUNC(destroy), dialog); 
      gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
    }
  widget = gtk_button_new_with_label(msg2);
  gtk_widget_add_accelerator(widget, "clicked", accel_group, GDK_Escape, 0,
                              GTK_ACCEL_VISIBLE);
  if (callback == NULL)
    gtk_widget_add_accelerator(widget, "clicked", accel_group, GDK_Return, 0,
			       GTK_ACCEL_VISIBLE);
  gtk_widget_show(widget);
  gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
		     GTK_SIGNAL_FUNC(destroy), dialog); 
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), hbox, TRUE,
		     TRUE, 9);

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), main_area, TRUE,
		     TRUE, 9);
  return dialog;
}

GtkWidget *create_spin(int min, int max, int def)
{
  GtkAdjustment *adj;
  GtkWidget *result;

  adj = (GtkAdjustment *)gtk_adjustment_new(def, min, max, 1, 10, 0);
  result = gtk_spin_button_new(adj, 0.4, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(result), TRUE);
  gtk_widget_show(result);
  return result;
}

GtkWidget *create_entry(const char *def, int len)
{
  GtkWidget *result;

  if (len > 0)
    result = gtk_entry_new_with_max_length(len);
  else
    result = gtk_entry_new();
  if (def != NULL)
    gtk_entry_set_text(GTK_ENTRY(result), def);
  gtk_widget_show(result);
  return result;
}

GtkWidget *create_check(gboolean def)
{
  GtkWidget *result;

  result = gtk_check_button_new();
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(result), def);
  gtk_widget_show(result);
  return result;
}

