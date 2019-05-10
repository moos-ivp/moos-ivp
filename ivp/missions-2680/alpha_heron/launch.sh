#!/bin/bash -e
#---------------------------------------------------------
# File: launch.sh
# Name: Mike Benjamin
# Date: May 8th, 2019
# Note: Goal of general pavilion vehicle launch script
#---------------------------------------------------------
#  Part 1: Initialize configurable variables with defaults
#---------------------------------------------------------
V01="no"
V02="no"
V03="no"
V04="no"
V05="no"
V06="no"
V07="no"
V08="no"
V09="no"
V10="no"
V11="no"
V12="no"
TIME_WARP=""
JUST_MAKE=""
HELP="no"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	HELP="yes"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = "" ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
        JUST_MAKE="yes"
    elif [ "${ARGI}" = "-v1"  -o "${ARGI}" = "-a" ]; then V01="yes"
    elif [ "${ARGI}" = "-v2"  -o "${ARGI}" = "-b" ]; then V02="yes"
    elif [ "${ARGI}" = "-v3"  -o "${ARGI}" = "-c" ]; then V03="yes"
    elif [ "${ARGI}" = "-v4"  -o "${ARGI}" = "-d" ]; then V04="yes"
    elif [ "${ARGI}" = "-v5"  -o "${ARGI}" = "-e" ]; then V05="yes"
    elif [ "${ARGI}" = "-v6"  -o "${ARGI}" = "-f" ]; then V06="yes"
    elif [ "${ARGI}" = "-v7"  -o "${ARGI}" = "-g" ]; then V07="yes"
    elif [ "${ARGI}" = "-v8"  -o "${ARGI}" = "-H" ]; then V08="yes"
    elif [ "${ARGI}" = "-v9"  -o "${ARGI}" = "-i" ]; then V09="yes"
    elif [ "${ARGI}" = "-v10" -o "${ARGI}" = "-J" ]; then V10="yes"
    elif [ "${ARGI}" = "-v11" -o "${ARGI}" = "-k" ]; then V11="yes"
    elif [ "${ARGI}" = "-v12" -o "${ARGI}" = "-l" ]; then V12="yes"

    else 
        echo "Bad arg:" $ARGI "Run with -h for help. Exiting (1)."
        exit 1
    fi
done

#---------------------------------------------------------
#  Part 3: Produce the help message if it was requested
#---------------------------------------------------------
if [ ${HELP} = "yes" ] ; then
    echo "$0 [SWITCHES]"
    echo "  --help, -h         Show this help message            "        
    echo "  --just_make, -j    Just make targ files, dont launch "
    echo "   -v1, or -a        Abe vehicle           "
    echo "   -v2, or -b        Ben vehicle           "
    echo "   -v3, or -c        Cal vehicle           "
    echo "   -v4, or -d        Deb vehicle           "
    echo "   -v5, or -e        Evan vehicle          "
    echo "   -v6, or -f        Felix vehicle         "
    echo "   -v7, or -g        Gus vehicle           "
    echo "   -v8, or -H        Hal vehicle           "
    echo "   -v9, or -i        Ida vehicle           "
    echo "  -v10, or -J        Jing vehicle          "
    echo "  -v11, or -k        Kirk vehicle          "
    echo "  -v12, or -l        Luke vehicle          "
    exit 0;
fi

#-------------------------------------------------------
#  Part 4: Launch any and all vehicle communities
#-------------------------------------------------------
COMMON_ARGS="--sim --noui $TIME_WARP $JUST_MAKE --shoreip=localhost"

if [ "${V01}" = "yes" ]; then ./launch_vehicle.sh -v1  $COMMON_ARGS; fi
if [ "${V02}" = "yes" ]; then ./launch_vehicle.sh -v2  $COMMON_ARGS; fi
if [ "${V03}" = "yes" ]; then ./launch_vehicle.sh -v3  $COMMON_ARGS; fi
if [ "${V04}" = "yes" ]; then ./launch_vehicle.sh -v4  $COMMON_ARGS; fi
if [ "${V05}" = "yes" ]; then ./launch_vehicle.sh -v5  $COMMON_ARGS; fi
if [ "${V06}" = "yes" ]; then ./launch_vehicle.sh -v6  $COMMON_ARGS; fi
if [ "${V07}" = "yes" ]; then ./launch_vehicle.sh -v7  $COMMON_ARGS; fi
if [ "${V08}" = "yes" ]; then ./launch_vehicle.sh -v8  $COMMON_ARGS; fi
if [ "${V09}" = "yes" ]; then ./launch_vehicle.sh -v9  $COMMON_ARGS; fi
if [ "${V10}" = "yes" ]; then ./launch_vehicle.sh -v10 $COMMON_ARGS; fi
if [ "${V10}" = "yes" ]; then ./launch_vehicle.sh -v12 $COMMON_ARGS; fi
if [ "${V10}" = "yes" ]; then ./launch_vehicle.sh -v13 $COMMON_ARGS; fi

#-------------------------------------------------------
#  Part 5: Launch the shoreside MOOS community
#-------------------------------------------------------
echo "Launching Shoreside...."
./launch_shoreside.sh $TIME_WARP $JUST_MAKE


