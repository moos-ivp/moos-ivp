#!/bin/bash 

#=====================================================================
# Go into each directory. If it has a clean.sh script, then clean
#=====================================================================
for dir in ./*;
do
    if [ -d "$dir" ] ; then
        cd "$dir"
        if [ -f "clean.sh" ] ; then
            echo Cleaning $dir
            ./clean.sh
        fi
        cd - >& /dev/null
    fi
done
rm -f *~ 
