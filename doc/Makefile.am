# $Id: Makefile.am,v 1.5 2001/05/04 15:43:44 kilobug Exp $

EXTRA_DIST	= mserver.sql protocole.txt\
		  server.man gtk-client.man map.man prologin.man bouvier.man\
		  sujet/Makefile sujet/sujet.tex

man_MANS	= server.1 gtk-client.1 prologin.1 map.5 bouvier.1

server.1: server.man
	groff -t -man -Tlatin1 $< > $@

gtk-client.1: gtk-client.man
	groff -t -man -Tlatin1 $< > $@

prologin.1: prologin.man
	groff -t -man -Tlatin1 $< > $@

map.5: map.man
	groff -t -man -Tlatin1 $< > $@

bouvier.1: bouvier.man
	groff -t -man -Tlatin1 $< > $@