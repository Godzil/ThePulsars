# $Id: Makefile.am,v 1.8 2001/05/03 23:17:47 kilobug Exp $

INCLUDES = @GTK_CFLAGS@

bin_PROGRAMS		= prologin
prologin_SOURCES	= init.c main.c mclient.h \
			  gtk_main.c menubar.c \
			  bitmaps.c maps.c champions.c \
		   	  run_match.c matchs.c \
			  tournois.c tournois.h run_tournois.c \
		          mysql.h mysql_list.c mysql.c
prologin_LDADD		= @GTK_LIBS@ -lmysqlclient -L../gtkutil -lgtkutil
prologin_DEPENDENCIES	= ../gtkutil/libgtkutil.a

