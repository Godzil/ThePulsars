# $Id: Makefile.am,v 1.14 2001/05/01 11:56:11 kilobug Exp $

INCLUDES		= @GTK_CFLAGS@ @GDK_PIXBUF_CFLAGS@

bin_PROGRAMS		= gtk-client
gtk_client_SOURCES	= main.c client.h init.c gen_bigmap2.c\
			  gtk.c client_gtk.h network.c pixbuf.c \
			  map.c load_map.c gen_minimap.c gen_bigmap.c\
			  mainbox.c statusbar.c menubar.c toolbar.c \
			  gamearea.c minimap.c players.c stats.c
gtk_client_LDADD	= @GDK_PIXBUF_LIBS@ -L../netlib -lnet \
			  -L../gtkutil -lgtkutil
gtk_client_DEPENDENCIES	= ../netlib/libnet.a ../gtkutil/libgtkutil.a
