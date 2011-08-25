#!/bin/bash -x

# this example for running 
# MOOSDB
# pScanMatchNav
# uTextPlayBack  (takes $arg1)

echo $1

xterm -T MOOSDB -geometry 60x18+0+0                \
-font -*-courier-medium-r-*-*-12-*-*-*-*-*-*-*     \
-bg DarkOliveGreen -fg white                       \
-e bash -c 'MOOSDB LKSim-Viewer.moos 2>&1 | tee moosdb.out' &

sleep .2

xterm -T MOOSDump -geometry 60x18+0+262             \
-font -*-courier-medium-r-*-*-12-*-*-*-*-*-*-*     \
-bg DarkSlateBlue -fg white                        \
-e bash -c 'MOOSDump LKSim-Viewer.moos 2>&1 | tee moosdump.out' &

xterm -T pKayakViewer -geometry 60x18+0+524        \
-font -*-courier-medium-r-*-*-12-*-*-*-*-*-*-*     \
-bg DarkSlateGray -fg white                        \
-e bash -c 'pKayakViewer LKSim-Viewer.moos' &

sleep .2

