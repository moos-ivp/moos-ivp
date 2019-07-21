#!/bin/bash 

ROOT_DIR=$PWD

for dirs in *LOG*/ ; do 
    cd $dirs
    pwd
    alogsplit --verbose *pared.alog
    alogsplit --verbose *LOG*.alog
    cd $ROOT_DIR
    pwd
    echo "============================================================"
done

echo "Done Splitting...." 
exit 0

