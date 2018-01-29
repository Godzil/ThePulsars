/* $Id: menubar.c,v 1.11 2001/05/03 23:17:47 kilobug Exp $ */

#include "mclient.h"

#define ZLOCK_COMMAND "zlock -immed"

static void menu_disp_about(GtkWidget *wi, void *data)
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
  widget = create_label("Meta-client version "VERSION
			"\n\nContact: info@prologin.org");
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

static void menu_zlock(GtkWidget *wi, void *data)
{
  set_my_status(STATUT_ZLOCKE);
  system(ZLOCK_COMMAND);
  set_my_status(STATUT_PRESENT);
}

static void menu_set_pseudo(GtkWidget *wi, GtkWidget *entry)
{
  mysql_do_query(gl_config->mysql, 
		 "UPDATE Bitmaps SET pseudo='%s' WHERE id=%d",
		 gtk_entry_get_text(GTK_ENTRY(entry)), getuid());
  bmp_refresh(NULL, NULL);
}

static void menu_pseudo(GtkWidget *wi, void *data)
{
  static GtkWidget *dialog = NULL;
  GtkWidget *widget, *hbox, *entry;
  char *s;

  if (dialog == NULL)
    {
      hbox = gtk_hbox_new(FALSE, 3);
      gtk_widget_show(hbox);
      widget = create_label("Pseudo: ");
      s = mysql_get_str(gl_config->mysql, 
			"SELECT pseudo FROM Bitmaps WHERE id=%d", getuid());
      gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 9);
      entry = create_entry(s, 80);
      g_free(s);
      gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 9);
      dialog = create_dialog("Choix d'un pseudo", hbox,
			     " Valider ", " Annuler ", 
			     menu_set_pseudo, entry, hide_widget);
      gtk_signal_connect(GTK_OBJECT(entry), "activate", menu_set_pseudo, 
			 entry);
      gtk_signal_connect_after(GTK_OBJECT(entry), "activate", hide_widget, 
			 dialog);
    }
  gtk_widget_show(dialog);
}

static void menu_upall(GtkWidget *wi, void *data)
{
  bmp_refresh(wi, data);
  prg_refresh(wi, data);
  map_refresh(wi, data);  
  mat_refresh(wi, data);
}

static void menu_runmatch(GtkWidget *wi, void *data)
{
  gtk_widget_show(start_match_box());
}

static void menu_runtournois(GtkWidget *wi, void *data)
{
  gtk_widget_show(start_tournois_box());
}

static void menu_ttip(GtkCheckMenuItem *wi, void *data)
{
  if (wi->active)
    gtk_tooltips_enable(gl_config->tooltips);
  else
    gtk_tooltips_disable(gl_config->tooltips);
}

static void menu_nb(GtkCheckMenuItem *wi, void *data, int page)
{
  if (wi->active)
    gtk_notebook_set_page(gl_config->notebook, page - 1);
}

static GtkItemFactoryEntry gtk_menu_items[] = {
      { "/_Système",            NULL,         NULL, 0, "<Branch>" },
      { "/Système/Choisir un _Pseudo", "<control>P",  menu_pseudo, 0, NULL },
      { "/Système/_Zlock",     "<control>Z",  menu_zlock, 0, NULL },
      { "/Système/Tout _Actualiser", "<control>A",  menu_upall, 0, NULL },
      { "/Système/sep1",        NULL, NULL, 0, "<Separator>" },
      { "/Système/_Quitter",    "<control>Q", byebye, 0, NULL },
      { "/_Section",           NULL,         NULL, 0, "<Branch>" },
      { "/Section/_Personnes",  "F1",        menu_nb, 1, "<RadioItem>" },
      { "/Section/_Cartes",     "F2",        menu_nb, 2, 
	"<main>/Section/Personnes" },
      { "/Section/C_hampions",  "F3",        menu_nb, 3, 
	"<main>/Section/Personnes" },
      { "/Section/_Matchs en cours",  "F4",  menu_nb, 4, 
	"<main>/Section/Personnes" },
      { "/_Match",           NULL,         NULL, 0, "<Branch>" },
      { "/Match/_Lancer",    "<control>L", menu_runmatch, 0, NULL },
      { "/Match/_Tournois",  "<control>T", menu_runtournois, 0, NULL },
      { "/_Aide",               NULL,         NULL, 0, "<LastBranch>" },
      { "/_Aide/_Bulles d'aide", "<control>B", menu_ttip, 10, "<ToggleItem>" },
      { "/_Aide/_A propos",     NULL,         menu_disp_about, 0, NULL },
    };

GtkWidget *create_menubar()
{
  GtkItemFactory *item_factory = NULL;
  GtkAccelGroup	*accel_group  = NULL;
  GtkWidget *widget = NULL, *ttip = NULL;
  int nmenu_items, i;

  nmenu_items = sizeof (gtk_menu_items) / sizeof (gtk_menu_items[0]);
  accel_group = gtk_accel_group_new();
  item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", 
				      accel_group);
  gtk_item_factory_create_items_ac(item_factory, nmenu_items, gtk_menu_items, 
				   NULL, 2);
  gtk_window_add_accel_group(GTK_WINDOW(gl_config->window), accel_group);
  widget = gtk_item_factory_get_widget(item_factory, "<main>");
  ttip = gtk_item_factory_get_widget_by_action(item_factory, 10);  
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ttip), TRUE);  
  for (i = 0; i < NOTEBOOK_PAGE_NUM; i++)
    gl_config->menunb[i] = gtk_item_factory_get_widget_by_action(item_factory,
								 i + 1);  
  gtk_widget_show(widget);
  return widget;
}
