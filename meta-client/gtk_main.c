/* $Id: gtk_main.c,v 1.14 2001/05/07 02:41:36 kilobug Exp $ */

#include "mclient.h"

static void switch_page(GtkNotebook *notebook, GtkNotebookPage *foo,
			int page, void *data)
{
  if (gl_config->menunb[page] == NULL)
    return;
  gtk_signal_handler_block_by_func(GTK_OBJECT(notebook), 
				   GTK_SIGNAL_FUNC(switch_page), data);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gl_config->menunb[page]),
				 TRUE);
  gtk_signal_handler_unblock_by_func(GTK_OBJECT(notebook), 
				     GTK_SIGNAL_FUNC(switch_page), data);
}

static void create_page(GtkWidget *noteb, GtkWidget *inside, const char *label,
			int nb, ...)
{
  GtkWidget *hbox, *vbox, *but, *lab, *scroll;
  va_list va;
  int i;
  
  va_start(va, nb);
  hbox = gtk_hbox_new(TRUE, 5);
  for (i = 0; i < nb; i++)
    {
      but = gtk_button_new_with_label(va_arg(va, const char *));
      gtk_widget_show(but);
      gtk_tooltips_set_tip(gl_config->tooltips, but, 
			   va_arg(va, const char *), "");
      gtk_signal_connect(GTK_OBJECT(but), "clicked", 
			 va_arg(va, GtkSignalFunc), NULL);
      gtk_box_pack_start(GTK_BOX(hbox), but, TRUE, FALSE, 5);
    }
  gtk_widget_show(hbox);

  vbox = gtk_vbox_new(FALSE, 0);
  scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scroll);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), 
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
  gtk_container_add(GTK_CONTAINER(scroll), inside);
  gtk_widget_show(vbox);
  lab = create_label(label);  
  gtk_notebook_append_page_menu(GTK_NOTEBOOK(noteb), vbox, 
				lab, create_label(label));
}

static GtkWidget *create_main_area()
{
  GtkWidget *widget;

  widget = gtk_notebook_new();
  gl_config->notebook = GTK_NOTEBOOK(widget);
  gtk_notebook_popup_enable(gl_config->notebook);
  gtk_notebook_set_homogeneous_tabs(gl_config->notebook, TRUE);
  gtk_widget_show(widget);
  gtk_signal_connect(GTK_OBJECT(widget), "switch-page", switch_page, NULL);
  gl_config->bmpl = create_clist(7, 0,
				 "Login", "Nom", "Prénom", "Pseudo", 
				 "Machine", "Statut", "Type",
				 50, 110, 110, 90, 60, 60, 60,
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING);
  gtk_clist_set_compare_func(GTK_CLIST(gl_config->bmpl), bmp_sort);
  clist_set_tooltips(GTK_CLIST(gl_config->bmpl), gl_config->tooltips,
		     "Trier par login", "Trier par nom", "Trier par prénom",
		     "Trier par pseudo", "Trier par machine", 
		     "Trier par satut", "Séparer les orgas des bitmaps", NULL);
		     
  create_page(widget, gl_config->bmpl, "Personnes", 2,
	      " Actualiser ", "Réactualiser la liste", bmp_refresh,
	      " Ecrire ", "Envoyer un message", bmp_send_message);
  gtk_clist_set_selection_mode(GTK_CLIST(gl_config->bmpl), 
			       GTK_SELECTION_EXTENDED);

  gl_config->mapl = create_clist(5, 0, 
				 "Nom", "Description", "Joueurs", 
				 "Auteur", "Taille",
				 80, 150, 50, 50, 50,
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING);
  create_page(widget, gl_config->mapl, "Cartes", 4,
	      " Actualiser ", "Réactualiser la liste", map_refresh, 
	      " Ajouter ", "Ajouter une carte perso", map_add, 
	      " Modifier ", "Modifier une carte", map_edit,
	      " Supprimer ", "Supprimer la carte sélectionnée", map_remove);
  gtk_clist_set_compare_func(GTK_CLIST(gl_config->mapl), map_sort);
  clist_set_tooltips(GTK_CLIST(gl_config->mapl), gl_config->tooltips,
		     "Trier par nom", "Trier par description", 
		     "Trier par nombre de joueurs",
		     "Trier par auteur", "Trier par taille", NULL);
  gl_config->mapa = 
    mysql_adder_make("Maps", "Ajout d'une carte", map_refresh, 6,
		     "Nom: ", "nom", "Nom de la carte", sql_add_str, 80,
		     "Description: ", "descr", "Description de la carte",
		     sql_add_str, 120,
		     "Nombre de joueurs: ", "joueurs", 
		     "Nombre de joueurs maximum", sql_add_num, 1, 42, 4,
		     "Taille: ", "taille", 
		     "Taille de la carte (ex: 100x100)", sql_add_str, 40,
		     "Publique: ", "public", 
		     "Cette carte est-elle visible par les autres?",
		     sql_add_flag, TRUE,
		     "Fichier: ", "fichier", "Nom du fichier carte",
		     sql_add_file, "./maps");

  gl_config->prgl = create_clist(5, 0, 
				 "Nom", "Auteur", "Meilleur score", 
				 "Score moyen", "Tournois", 
				 80, 120, 80, 70, 30, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_DESCENDING, 
				 GTK_SORT_DESCENDING, 
				 GTK_SORT_DESCENDING);
  create_page(widget, gl_config->prgl, "Champions", 4,
	      " Actualiser ", "Réactualiser la liste", prg_refresh, 
	      " Ajouter ", "Ajouter un champion", prg_add, 
	      " Modifier ", "Modifier un champion", prg_edit,
	      " Supprimer ", "Supprimer les champions sélectionnés", 
	      prg_remove);
  clist_set_tooltips(GTK_CLIST(gl_config->prgl), gl_config->tooltips,
		     "Trier par nom", "Trier par auteur", 
		     "Trier par meilleur score",  "Trier par sore moyen", 
		     "", NULL);
  gtk_clist_set_selection_mode(GTK_CLIST(gl_config->prgl), 
			       GTK_SELECTION_EXTENDED);
  gtk_clist_set_compare_func(GTK_CLIST(gl_config->prgl), prg_sort);
  gl_config->prga = 
    mysql_adder_make("Champions", "Ajout d'un chamion", prg_refresh, 4,
		     "Nom: ", "nom", "Nom du champion", sql_add_str, 80,
		     "Publique: ", "public", 
		     "Les autres candidats peuvent-ils tester "
		     "leur champion contre le tien?", sql_add_flag, TRUE,
		     "Tournois: ", "tourn", 
		     "Inclure dans les tournois ? ", sql_add_flag, FALSE,
		     "Fichier: ", "fichier", 
		     "Fichier .so contenant le champion.", sql_add_file, 
		     "./code");
  
  gl_config->matl = create_clist(4, 0, 
				 "Machine", "Bitmap", "Map", "Joueurs", 
				 70, 120, 80, 80,
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING, 
				 GTK_SORT_ASCENDING);
  create_page(widget, gl_config->matl, "Matchs en cours", 2,
	      " Actualiser ", "Réactualiser la liste", mat_refresh, 
	      " Se connecter ", 
	      "Lancer un client graphique sur les matchs sélectionnés", 
	      mat_connect); 
  clist_set_tooltips(GTK_CLIST(gl_config->prgl), gl_config->tooltips,
		     "Trier par machine", 
		     "Trier par bitmap (ayant lancé la partie)", 
		     "Trier par carte",  "Trier par nombre de joueurs", 
		     NULL);
  gtk_clist_set_compare_func(GTK_CLIST(gl_config->matl), mat_sort);
  gtk_clist_set_selection_mode(GTK_CLIST(gl_config->matl), 
			       GTK_SELECTION_EXTENDED);

  return widget;
}

void create_gtk_stuff()
{
  GtkWidget *vbox;

  gl_config->tooltips = gtk_tooltips_new();
  gl_config->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect(GTK_OBJECT(gl_config->window), "delete-event", 
		     GTK_SIGNAL_FUNC(byebye), NULL);
  gtk_window_set_title(GTK_WINDOW(gl_config->window), 
		       "Meta-client Prologin 2001");
  gtk_window_set_default_size(GTK_WINDOW(gl_config->window), 650, 250);
  gtk_widget_show(gl_config->window);

  vbox = gtk_vbox_new(FALSE, 4);
  gtk_container_add(GTK_CONTAINER(gl_config->window), vbox);

  gtk_box_pack_start(GTK_BOX(vbox), create_menubar(), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), create_main_area(), TRUE, TRUE, 0);
  
  prg_refresh(NULL, NULL);
  bmp_refresh(NULL, NULL);
  map_refresh(NULL, NULL);
  mat_refresh(NULL, NULL);

  gtk_widget_show(vbox);  
}
