#!/bin/bash 

ROOT_DIR=$PWD

for dirs in *LOG*/ ; do 
    cd $dirs
    pwd
    alogcd *pared.alog --tfile=tally.txt
    cd $ROOT_DIR
    pwd
    echo "============================================================"
done

echo "Done Tallying." 
exit 0

