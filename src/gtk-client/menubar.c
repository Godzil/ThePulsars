/* $Id: menubar.c,v 1.11 2001/04/20 10:25:10 kilobug Exp $ */

#include "client.h"

static void menu_disp_about(GtkWidget *wi, conf_t *conf)
{
  GtkWidget *dialog;
  GtkWidget *widget;
  GtkAccelGroup *accel_group;

  dialog = gtk_dialog_new();
  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(&(GTK_DIALOG(dialog)->window)), 
			     accel_group);
  gtk_container_set_border_width(GTK_CONTAINER(&(GTK_DIALOG(dialog)->window)),
				 7);
  gtk_window_set_title(GTK_WINDOW(&(GTK_DIALOG(dialog)->window)), 
				  "Finale Prologin 2001");
  widget = gtk_label_new("Client GTK+ version "VERSION
			 "\n\nContact: info@prologin.org");
  gtk_widget_show(widget);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), widget, TRUE, TRUE, 9);
  widget = gtk_button_new_with_label(" OK ");
  gtk_widget_add_accelerator(widget, "clicked", accel_group, GDK_Return, 0,
                              GTK_ACCEL_VISIBLE);
  gtk_widget_show(widget);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), widget, TRUE, 
		     TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(widget), "clicked", 
		     GTK_SIGNAL_FUNC(destroy_widget), dialog); 
  gtk_widget_show(dialog);
}

static void menu_disp_connect(GtkWidget *wi, conf_t *conf)
{
  gtk_widget_show(conf->conf);
}

static void menu_stat(GtkWidget *wi, conf_t *conf)
{
  conf->stat.on = !conf->stat.on;
  if (conf->stat.on)
    gtk_widget_show(conf->stat.stat);
  else
    gtk_widget_hide(conf->stat.stat);
}

static void menu_ttip(GtkCheckMenuItem *wi, conf_t *conf)
{
  if (wi->active)
    gtk_tooltips_enable(conf->tooltips);
  else
    gtk_tooltips_disable(conf->tooltips);
}

static void menu_stop(GtkWidget *wi, conf_t *conf)
{
  conf->terminated = TRUE;
  allow_new_turn(conf);
}

static GtkItemFactoryEntry gtk_menu_items[] = {
      { "/_Système",            NULL,         NULL, 0, "<Branch>" },
      { "/Système/C_onnecter",  "<control>O", menu_disp_connect, 0, NULL },
      { "/Système/sep1",        NULL,         NULL, 0, "<Separator>" },
      { "/Système/_Quitter",    "<control>Q", byebye, 0, NULL },
      { "/_Simulation",         NULL,         NULL, 0, "<Branch>" },
      { "/Simulation/_Stats",    "<control>S", menu_stat, 5, "<ToggleItem>" },
      { "/_Aide",               NULL,         NULL, 0, "<LastBranch>" },
      { "/_Aide/_Bulles d'aide", "<control>B", menu_ttip, 4, "<ToggleItem>" },
      { "/_Aide/_A propos",     NULL,         menu_disp_about, 0, NULL },
    };

static GtkItemFactoryEntry gtk_menu_items_debug[] = {
      { "/_Système",            NULL,         NULL, 0, "<Branch>" },
      { "/Système/C_onnecter",  "<control>O", menu_disp_connect, 0, NULL },
      { "/Système/sep1",        NULL,         NULL, 0, "<Separator>" },
      { "/Système/_Quitter",    "<control>Q", byebye, 0, NULL },
      { "/_Simulation",         NULL,         NULL, 0, "<Branch>" },
      { "/Simulation/_Pause",   "<control>P", tb_pause, 0, NULL },
      { "/Simulation/_Tour suivant",  "<control>T", tb_play, 0, NULL },
      { "/Simulation/_N tours",  "<control>N", tb_step, 0, NULL },
      { "/Simulation/_Continuer",  "<control>C", tb_ffwd, 0, NULL },
      { "/Simulation/Arreter",  "<control>A", menu_stop, 0, NULL },
      { "/Simulation/sep1",     NULL,         NULL, 0, "<Separator>" },
      { "/Simulation/_Stats",    "<control>S", menu_stat, 5, "<ToggleItem>" },
      { "/_Aide",               NULL,         NULL, 0, "<LastBranch>" },
      { "/_Aide/_Bulles d'aide", "<control>B", menu_ttip, 4, "<ToggleItem>" },
      { "/_Aide/_A propos",     NULL,         menu_disp_about, 0, NULL },
    };

GtkWidget *create_menubar(conf_t *conf)
{
  GtkItemFactory *item_factory = NULL;
  GtkAccelGroup	*accel_group  = NULL;
  GtkWidget *widget = NULL, *ttip = NULL;
  GtkItemFactoryEntry *menu_items;
  int nmenu_items;

  if (conf->debug)
    {
      menu_items = gtk_menu_items_debug;
      nmenu_items = sizeof (gtk_menu_items_debug) / sizeof (menu_items[0]);
    }
  else
    {
      menu_items = gtk_menu_items;
      nmenu_items = sizeof (gtk_menu_items) / sizeof (menu_items[0]);
    }

  accel_group = gtk_accel_group_new();
  item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", 
				      accel_group);
  gtk_item_factory_create_items_ac(item_factory, nmenu_items, menu_items, 
				   conf, 2);
  gtk_window_add_accel_group(GTK_WINDOW(conf->window), accel_group);
  widget = gtk_item_factory_get_widget(item_factory, "<main>");
  conf->stat.menu = gtk_item_factory_get_widget_by_action(item_factory, 5);
  ttip = gtk_item_factory_get_widget_by_action(item_factory, 4);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ttip), TRUE);
  gtk_widget_show(widget);
  return widget;
}
