#!/bin/bash -e
#----------------------------------------------------------
#  Script: zlaunch_simple.sh
#----------------------------------------------------------

xlaunch.sh 20 --com=alpha --spd=1.0 --nogui
xlaunch.sh 20 --com=alpha --spd=1.5 --nogui
xlaunch.sh 20 --com=alpha --spd=2.0 --nogui
xlaunch.sh 20 --com=alpha --spd=2.5 --nogui
xlaunch.sh 20 --com=alpha --spd=3.0 --nogui

# Build a plot using matlab
if [ -f "results.txt" ]; then
    matlab plotx.m -nodisplay -nosplash -r "plotx(\"results.txt\", \"plot.png\")"
fi

