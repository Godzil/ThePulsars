cmake_minimum_required(VERSION 3.10)

project(PulsarDoc)

# Building the manpages:
# groff -t -man -Tlatin1 $< > $@

install(FILES COPYING
        DESTINATION .)

install(FILES sujet/sujet.tex AUTHORS
        DESTINATION doc)