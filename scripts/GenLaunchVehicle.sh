#!/bin/bash
#------------------------------------------------------------
#   Script: GenlaunchVehicle.sh
#   Author: M.Benjamin
#   LastEd: Oct 2024
#------------------------------------------------------------
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGINT (ctrl-c).
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#------------------------------------------------------------ 
#  Part 2: Set global variable default values
#------------------------------------------------------------
ME=`basename "$0"`
VERBOSE="no"
MISSION="generic_mission"
AUTHOR="Marvin T. Moose"
DATE=`date "+%b %d %Y"`
OUTPUT="launch_vehicle_new.sh"

#------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#------------------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                      " 
	echo "                                               "
	echo "Options:                                       "
	echo "  --help, -h         Show this help message    " 
	echo "  --verbose, -v      Verbose, confirm launch   "
	echo "  --output=<filenam> Direct output to filename "
	echo "  --replace, -r      Same: --output=launch_vehicle.sh "
	exit 0
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI 
    elif [ "${ARGI}" = "--replace" -o "${ARGI}" = "-r" ]; then
        OUTPUT="launch_vehicle.sh"
    elif [ "${ARGI:0:9}" = "--output=" ]; then
        OUTPUT="${ARGI#--output=*}"
    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
        exit 1
    fi
done

cat > $OUTPUT <<EOF
#!/bin/bash
#------------------------------------------------------------
#   Script: launch_vehicle.sh
#  Mission: $MISSION
#   Author: $AUTHOR
#   LastEd: $DATE
#------------------------------------------------------------
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGINT (ctrl-c).
#------------------------------------------------------------
vecho() { if [ "\$VERBOSE" != "" ]; then echo "\$ME: \$1"; fi }
on_exit() { echo; echo "\$ME: Halting all apps"; kill -- -\$\$; }
trap on_exit SIGINT

#------------------------------------------------------------
#  Part 2: Declare global var defaults
#------------------------------------------------------------
ME=\`basename "\$0"\`
CMD_ARGS=""
TIME_WARP=1
VERBOSE=""
JUST_MAKE=""
LOG_CLEAN="no"
AUTO_LAUNCHED="no"

IP_ADDR="localhost"
MOOS_PORT="9001"
PSHARE_PORT="9201"
SHORE_IP="localhost"
SHORE_PSHARE="9200"

VNAME="abe"
COLOR="yellow"
XMODE="M300"
START_POS="x=0,y=0,heading=0"  
STOCK_SPD="1.4"
MAX_SPD="2"

# Custom

#------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" \${ARGI}"
    if [ "\${ARGI}" = "--help" -o "\${ARGI}" = "-h" ]; then
	echo "\$ME [OPTIONS] [time_warp]                        "
	echo "                                                 " 
	echo "Options:                                         "
	echo "  --help, -h             Show this help message  "
	echo "  --just_make, -j        Only create targ files  "
	echo "  --verbose, -v          Verbose, confirm launch "
	echo "  --log_clean, -lc       Run clean.sh bef launch "
        echo "  --auto, -a             Script launch, no uMAC  "
	echo "                                                 "
	echo "  --ip=<localhost>       Veh Default IP addr     "
	echo "  --mport=<9001>         Veh MOOSDB port         "
	echo "  --pshare=<9201>        Veh pShare listen port  "
	echo "  --shore=<localhost>    Shoreside IP to try     "
	echo "  --shore_pshare=<9200>  Shoreside pShare port   "
	echo "                                                 "
	echo "  --vname=<abe>          Veh name given          "
	echo "  --color=<yellow>       Veh color given         "
	echo "  --sim, -s              Sim, not fld vehicle    "
	echo "  --start_pos=<X,Y,Hdg>  Sim start pos/hdg       "
	echo "  --stock_spd=<m/s>      Default vehicle speed   "
	echo "  --max_spd=<m/s>        Max Sim and Helm speed  "
	echo "                                                 "
	echo "Options (custom):                                "
	exit 0
    elif [ "\${ARGI//[^0-9]/}" = "\$ARGI" -a "\$TIME_WARP" = 1 ]; then 
        TIME_WARP=\$ARGI
    elif [ "\${ARGI}" = "--verbose" -o "\${ARGI}" = "-v" ]; then
        VERBOSE="yes"
    elif [ "\${ARGI}" = "--just_make" -o "\${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "\${ARGI}" = "--log_clean" -o "\${ARGI}" = "-lc" ]; then
	LOG_CLEAN="yes"
    elif [ "\${ARGI}" = "--auto" -o "\${ARGI}" = "-a" ]; then
        AUTO_LAUNCHED="yes" 

    elif [ "\${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="\${ARGI#--ip=*}"
    elif [ "\${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="\${ARGI#--mport=*}"
    elif [ "\${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="\${ARGI#--pshare=*}"
    elif [ "\${ARGI:0:8}" = "--shore=" ]; then
        SHORE_IP="\${ARGI#--shore=*}"
    elif [ "\${ARGI:0:15}" = "--shore_pshare=" ]; then
        SHORE_PSHARE="\${ARGI#--shore_pshare=*}"

    elif [ "\${ARGI:0:8}" = "--vname=" ]; then
        VNAME="\${ARGI#--vname=*}"
    elif [ "\${ARGI:0:8}" = "--color=" ]; then
        COLOR="\${ARGI#--color=*}"
    elif [ "\${ARGI}" = "--sim" -o "\${ARGI}" = "-s" ]; then
        XMODE="SIM"
    elif [ "\${ARGI:0:12}" = "--start_pos=" ]; then
        START_POS="\${ARGI#--start_pos=*}"
    elif [ "\${ARGI:0:12}" = "--stock_spd=" ]; then
        STOCK_SPD="\${ARGI#--stock_spd=*}"
    elif [ "\${ARGI:0:10}" = "--max_spd=" ]; then
        MAX_SPD="\${ARGI#--max_spd=*}"

    else 
	echo "\$ME: Bad Arg:[\$ARGI]. Exit Code 1."
	exit 1
    fi
done

#------------------------------------------------------------
#  Part 4: If Heron hardware, set key info based on IP address
#------------------------------------------------------------
if [ "\${XMODE}" = "M300" ]; then
    COLOR=\`get_heron_info.sh --color --hint=\$COLOR\`
    IP_ADDR=\`get_heron_info.sh --ip\`
    FSEAT_IP=\`get_heron_info.sh --fseat\`
    VNAME=\`get_heron_info.sh --name\`
    if [ \$? != 0 ]; then
	echo "\$ME: Problem getting Heron Info. Exit Code 2"
	exit 2
    fi
fi
     
#------------------------------------------------------------
#  Part 5: If verbose, show vars and confirm before launching
#------------------------------------------------------------
if [ "\${VERBOSE}" = "yes" ]; then 
    echo "============================================"
    echo "     launch_vehicle.sh SUMMARY        \$VNAME"
    echo "============================================"
    echo "\$ME                               "
    echo "CMD_ARGS =      [\${CMD_ARGS}]     "
    echo "TIME_WARP =     [\${TIME_WARP}]    "
    echo "JUST_MAKE =     [\${JUST_MAKE}]    "
    echo "AUTO_LAUNCHED = [\${AUTO_LAUNCHED}]"
    echo "LOG_CLEAN =     [\${LOG_CLEAN}]    "
    echo "----------------------------------"
    echo "IP_ADDR =       [\${IP_ADDR}]      "
    echo "MOOS_PORT =     [\${MOOS_PORT}]    "
    echo "PSHARE_PORT =   [\${PSHARE_PORT}]  "
    echo "SHORE_IP =      [\${SHORE_IP}]     "
    echo "SHORE_PSHARE =  [\${SHORE_PSHARE}] "
    echo "----------------------------------"
    echo "VNAME =         [\${VNAME}]        "
    echo "COLOR =         [\${COLOR}]        "
    echo "XMODE =         [\${XMODE}]        "
    echo "------------Sim-------------------"
    echo "START_POS =     [\${START_POS}]    "
    echo "STOCK_SPD =     [\${STOCK_SPD}]    "
    echo "MAX_SPD =       [\${MAX_SPD}]      "
    echo "------------Fld-------------------"
    echo "FSEAT_IP =      [\${FSEAT_IP}]     "
    echo "------------Custom----------------"
    echo -n "Hit any key to continue launching \$VNAME "
    read ANSWER
fi

#------------------------------------------------------------
#  Part 6: If Log clean before launch, do it now. 
#------------------------------------------------------------
if [ "\$LOG_CLEAN" = "yes" -a -f "clean.sh" ]; then
    vecho "Cleaning local Log Files"
    ./clean.sh
fi

#------------------------------------------------------------
#  Part 7: Create the .moos and .bhv files. 
#------------------------------------------------------------
NSFLAGS="--strict --force"
if [ "\${AUTO_LAUNCHED}" = "no" ]; then
    NSFLAGS="--interactive --force"
fi

nsplug meta_vehicle.moos targ_\$VNAME.moos \$NSFLAGS WARP=\$TIME_WARP \\
       IP_ADDR=\$IP_ADDR             MOOS_PORT=\$MOOS_PORT \\
       PSHARE_PORT=\$PSHARE_PORT     SHORE_IP=\$SHORE_IP   \\
       SHORE_PSHARE=\$SHORE_PSHARE   VNAME=\$VNAME         \\
       COLOR=\$COLOR                 XMODE=\$XMODE         \\
       START_POS=\$START_POS         MAX_SPD=\$MAX_SPD     \\
       FSEAT_IP=\$FSEAT_IP

nsplug meta_vehicle.bhv targ_\$VNAME.bhv \$NSFLAGS         \\
       START_POS=\$START_POS         VNAME=\$VNAME         \\
       STOCK_SPD=\$STOCK_SPD                               \\

if [ "\${JUST_MAKE}" = "yes" ]; then
    echo "\$ME: Targ files made; exiting without launch."
    exit 0
fi

#------------------------------------------------------------
#  Part 8: Launch the vehicle mission
#------------------------------------------------------------
echo "Launching \$VNAME MOOS Community. WARP="\$TIME_WARP
pAntler targ_\$VNAME.moos >& /dev/null &
echo "Done Launching \$VNAME MOOS Community"

#------------------------------------------------------------
#  Part 9: If launched from script, we're done, exit now
#------------------------------------------------------------
if [ "\${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

#------------------------------------------------------------
# Part 10: Launch uMAC until the mission is quit
#------------------------------------------------------------
uMAC targ_\$VNAME.moos
trap "" SIGINT
kill -- -\$\$
EOF

chmod ugo+x $OUTPUT
echo "$OUTPUT generated"
