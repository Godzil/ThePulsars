# $Id: Makefile.am,v 1.9 2001/05/01 11:56:10 kilobug Exp $

EXTRA_DIST = cleanshm.sh general.h

if AM_BUILD_MCLIENT
METACLIENT = gtkutil meta-client
else
METACLIENT = 
endif

if AM_BUILD_GTKCLIENT
GTKCLIENT = netlib gtkutil gtk-client
else
GTKCLIENT =
endif

if AM_BUILD_SERVER
SERVER = netlib server
else
SERVER =
endif

SUBDIRS = $(SERVER) $(GTKCLIENT) $(METACLIENT)

DIST_SUBDIRS = netlib gtkutil server gtk-client meta-client
