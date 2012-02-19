#!/bin/bash 

warp=$1

# First check that not more than one argument provided
if [ $# -gt 1 ]; then
    echo "Usage: launch.sh [1,50]"; 
    exit 127
fi

# Second check that the one argument is numerical
if [ "${warp//[^0-9]/}" != "$warp" ]; then 
    echo "Usage: launch.sh [1,50]"; 
    exit 127
fi

# Third check that the numerical argument is range [0,50]
if [ $warp -lt 1  -o  $warp -gt 50 ]; then
    echo "Usage: launch.sh [1,50]"; 
    exit 127
fi

# If no arguments provided, just launch without warp 
if [ $# -ne 1 ]; then
    pAntler unicorn.moos &
    sleep 1
    pAntler charlie.moos &
    sleep 1
    pAntler shoreside.moos &
    exit 0
fi

pAntler unicorn.moos --MOOSTimeWarp=$1 &
sleep 1
pAntler charlie.moos --MOOSTimeWarp=$1 &
sleep 1
pAntler shoreside.moos --MOOSTimeWarp=$1 &

