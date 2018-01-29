#!/bin/sh

LIST=`ipcs | grep $USER | grep " 600 " | awk '{print $2}'`

if  uname | grep -q "inux"
then
    ipcrm shm $LIST
else
    for a in $LIST; do ipcrm -m $a;done
fi
