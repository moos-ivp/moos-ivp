#!/bin/bash -x

# this example for running 
# MOOSDB
# pScanMatchNav
# uTextPlayBack  (takes $arg1)


gnome-terminal --geometry=80x8+521+1 -t MOOSDB \ 
-e "bash -c \"${PWD}/MOOSDB ${PWD}/$1 2>&1| tee ${PWD}/moosdb.out\"" &

sleep .2

gnome-terminal --geometry=80x8+521+151 -t MOOSDump -e "bash -c \"${PWD}/MOOSDump ${PWD}/$1 2>&1 | tee ${PWD}/MOOSDump.out\"" &

sleep .2

gnome-terminal --geometry=80x8+521+151 -t     \
pKakayViewer -e "bash -c \"${PWD}/pKayakViewer ${PWD}/$1 2>&1 | tee            \
${PWD}/pKayakViewer.out\"" &

sleep .2

