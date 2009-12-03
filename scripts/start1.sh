#!/bin/bash -x

# this example for running 
# MOOSDB
# pScanMatchNav
# uTextPlayBack  (takes $arg1)


gnome-terminal --window-with-profile=black  --geometry=80x8+521+1 -t MOOSDB -e "bash -c \"${PWD}/MOOSDB | tee ${PWD}/moosdb.out\""&

sleep .2

gnome-terminal --window-with-profile=green  --geometry=80x8+521+151 -t 
pScanMatchNav -e "bash -c \"${PWD}/pScanMatchNav ${PWD}/er1.moos | tee 
${PWD}/pscanmatchnav.out\""

sleep .2

gnome-terminal --window-with-profile=blue --geometry=60x7+521+301 -t 
uTextPlayBack -e "${PWD}/uTextPlayBack $PWD/$1"&
