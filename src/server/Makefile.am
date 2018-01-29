# $Id: Makefile.am,v 1.25 2001/05/03 16:37:47 glop Exp $

if AM_BUILD_PASCAL
PASCAL_INC = -DENABLE_GPC
PASCAL_LD  = -lgpc
else
PASCAL_INC = 
PASCAL_LD  =
endif

INCLUDES = @GLIB_CFLAGS@ $(PASCAL_INC)

bin_PROGRAMS	= server
server_SOURCES  = public.h private.h server.h plugins.h conf.h action.h \
		  main.c init.c plugins.c exec.c exec.h erreur.h \
		  ipc.c ipc.h client.c message.c message.h\
		  parser.c parser.h map.c map_parser.c bouvier.c\
		  game.c time.c pulse.c game.h objects.c \
		  network.c network_connect.c network.h client.h 
server_LDADD	= @GLIB_LIBS@ -L../netlib -lnet -lgmodule $(PASCAL_LD)
server_LDFLAGS  = -rdynamic
server_DEPENDENCIES = ../netlib/libnet.a

