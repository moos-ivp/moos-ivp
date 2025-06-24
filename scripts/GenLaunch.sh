#!/bin/bash -e
#----------------------------------------------------------
#   Script: Genlaunch.sh
#   Author: M.Benjamin
#   LastEd: Oct 2024
#------------------------------------------------------------
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGINT (ctrl-c).
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#---------------------------------------------------------- 
#  Part 2: Set global variable default values
#----------------------------------------------------------
ME=`basename "$0"`
CMD_ARGS=""
VERBOSE=""
MISSION="generic_mission"
AUTHOR="Marvin T. Moose"
DATE=`date "+%b %d %Y"`
OUTPUT="launch_new.sh"
MTASC="no"

#----------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                      " 
	echo "                                               "
	echo "Options:                                       "
	echo "  --help, -h         Show this help message    " 
	echo "  --verbose, -v      Verbose, confirm launch   "
	echo "  --output=<filenam> Direct output to filename "
	echo "  --replace, -r      Same: --output=launch.sh  "
	exit 0
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI 
    elif [ "${ARGI}" = "--replace" -o "${ARGI}" = "-r" ]; then
        OUTPUT="launch.sh"
    elif [ "${ARGI}" = "--mtasc" -o "${ARGI}" = "-m" ]; then
        MTASC="yes" 
    elif [ "${ARGI:0:9}" = "--output=" ]; then
        OUTPUT="${ARGI#--output=*}"
    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done


cat > $OUTPUT <<EOF
#!/bin/bash -e
#------------------------------------------------------------
#   Script: launch.sh
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
#  Part 2: Set global variable default values
#------------------------------------------------------------
ME=\`basename "\$0"\`
CMD_ARGS=""
TIME_WARP=1
VERBOSE=""
JUST_MAKE=""
LOG_CLEAN=""
VAMT="1"
MAX_VAMT="20"
RAND_VPOS=""
MAX_SPD="2"
MMOD=""

# Monte
XLAUNCHED="no"
NOGUI=""

EOF


if [ "${MTASC}" = "yes" ]; then
cat >> $OUTPUT <<EOF
# MTASC
SHORE_IP="localhost"
MTASC_USE="no"
MTASC_SUBNET="192.168.7"
MTASC_USE_CACHE=""
MTASC_MISSION="ufld_encircle"

EOF
fi

cat >> $OUTPUT <<EOF
# Custom

#------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" \${ARGI}"
    if [ "\${ARGI}" = "--help" -o "\${ARGI}" = "-h" ]; then
	echo "\$ME [OPTIONS] [time_warp]                      "
	echo "                                               "
	echo "Options:                                       "
	echo "  --help, -h         Show this help message    " 
	echo "  --verbose, -v      Verbose, confirm launch   "
	echo "  --just_make, -j    Only create targ files    " 
	echo "  --log_clean, -lc   Run clean.sh bef launch   " 
	echo "  --amt=N            Num vehicles to launch    "
	echo "  --rand, -r         Rand vehicle positions    "
	echo "  --max_spd=N        Max helm/sim speed        "
        echo "  --mmod=<mod>       Mission variation/mod     "
	echo "                                               "
	echo "Options (monte):                               "
	echo "  --xlaunched, -x    Launched by xlaunch       "
	echo "  --nogui, -ng       Headless launch, no gui   "
	echo "                                               "
EOF

if [ "${MTASC}" = "yes" ]; then
cat >> $OUTPUT <<EOF
	echo "Options (MTASC):                               "
	echo "  --mtasc, -m        Launch over MTASC cluster " 
	echo "  --shore=<ipaddr>   Declare Shore IP addr     " 
	echo "  --cache, -c        Use ~/.pablos_ipfs cache  " 
	echo "                                               "
EOF
fi

cat >> $OUTPUT <<EOF
	echo "Options (custom):                              "
	exit 0
    elif [ "\${ARGI//[^0-9]/}" = "\$ARGI" -a "\$TIME_WARP" = 1 ]; then 
        TIME_WARP=\$ARGI
    elif [ "\${ARGI}" = "--verbose" -o "\${ARGI}" = "-v" ]; then
	VERBOSE=\$ARGI
    elif [ "\${ARGI}" = "--just_make" -o "\${ARGI}" = "-j" ]; then
	JUST_MAKE=\$ARGI
    elif [ "\${ARGI}" = "--log_clean" -o "\${ARGI}" = "-lc" ]; then
	LOG_CLEAN=\$ARGI
    elif [ "\${ARGI:0:6}" = "--amt=" ]; then
        VAMT="\${ARGI#--amt=*}"
	if [ \$VAMT -lt 1 -o \$VAMT -gt \$MAX_VAMT ]; then
	    echo "\$ME: Veh amt range: [1, \$MAX_VAMT]. Exit Code 2."
	    exit 2
	fi
    elif [ "\${ARGI}" = "--rand" -o "\${ARGI}" = "-r" ]; then
        RAND_VPOS=\$ARGI
    elif [ "\${ARGI:0:10}" = "--max_spd=" ]; then
        MAX_SPD="\${ARGI#--max_spd=*}"
    elif [ "\${ARGI:0:7}" = "--mmod=" ]; then
        MMOD=\$ARGI

    elif [ "\${ARGI}" = "--xlaunched" -o "\${ARGI}" = "-x" ]; then
	XLAUNCHED="yes"
    elif [ "\${ARGI}" = "--nogui" -o "\${ARGI}" = "-ng" ]; then
	NOGUI="--nogui"
EOF

if [ "${MTASC}" = "yes" ]; then
cat >> $OUTPUT <<EOF
    elif [ "\${ARGI}" = "--mtasc" -o "\${ARGI}" = "-m" ]; then
	MTASC="yes"
    elif [ "\${ARGI:0:8}" = "--shore=" ]; then
        SHORE_IP="\${ARGI#--shore=*}"
    elif [ "\${ARGI}" = "--cache" -o "\${ARGI}" = "-c" ]; then
        USE_CACHE="--cache"

EOF
fi

cat >> $OUTPUT <<EOF
    else
	echo "\$ME: Bad arg:" \$ARGI "Exit Code 1."
        exit 1
    fi
done

#------------------------------------------------------------
#  Part 4: Set starting positions, speeds, vnames, colors
#------------------------------------------------------------
INIT_VARS=" --amt=\$VAMT \$RAND_VPOS \$VERBOSE "
./init_field.sh \$INIT_VARS

VEHPOS=(\`cat vpositions.txt\`)
SPEEDS=(\`cat vspeeds.txt\`)
VNAMES=(\`cat vnames.txt\`)
VCOLOR=(\`cat vcolors.txt\`)

EOF

#******************** BEGIN MTASC MODE ***********************
if [ "${MTASC}" = "yes" ]; then
cat >> $OUTPUT <<EOF
#-------------------------------------------------------------
#  Part 4B: In MTASC mode, the shore IP address must be: 
#          (1) a non-localhost IP address. 
#          (2) a currently active IP interface for this machine
#          (3) an IP address on the local MTASC network
#-------------------------------------------------------------
if [ "\$MTASC" = "yes" ]; then
    vecho "Verifying Shoreside IP Address for MTASC mode"

    ADDR=""
    if [ "\$SHORE_IP" = "localhost" ]; then
        ADDR=\`ipmatch.sh --match=\$MTASC_SUBNET\`
    else
	ADDR=\`ipmatch.sh --match=\$SHORE_IP\`
    fi
    
    if [ "\$ADDR" != "" ]; then
	SHORE_IP="\$ADDR"
    else
	echo "Provide active Shore IP addr on MTASC network. Exit Code 4."
	exit 4
    fi
    verify_ssh_key.sh --pablo
fi

EOF
fi
#******************** BEGIN MTASC MODE ***********************

cat >> $OUTPUT <<EOF
#------------------------------------------------------------
#  Part 5: If verbose, show vars and confirm before launching
#------------------------------------------------------------
if [ "\${VERBOSE}" != "" ]; then
    echo "============================================"
    echo "  \$ME SUMMARY                   (ALL)       "
    echo "============================================"
    echo "CMD_ARGS =      [\${CMD_ARGS}]               "
    echo "TIME_WARP =     [\${TIME_WARP}]              "
    echo "JUST_MAKE =     [\${JUST_MAKE}]              "
    echo "LOG_CLEAN =     [\${LOG_CLEAN}]              "
    echo "VAMT =          [\${VAMT}]                   "
    echo "MAX_VAMT =      [\${MAX_VAMT}]               "
    echo "RAND_VPOS =     [\${RAND_VPOS}]              "
    echo "MAX_SPD =       [\${MAX_SPD}]                "
    echo "MMOD =          [\${MMOD}]                   "
    echo "--------------------------------(VProps)----"
    echo "VNAMES =        [\${VNAMES[*]}]              "
    echo "VCOLORS =       [\${VCOLOR[*]}]              "
    echo "START_POS =     [\${VEHPOS[*]}]              "
    echo "--------------------------------(Monte)-----"
    echo "XLAUNCHED =     [\${XLAUNCHED}]              "
    echo "NOGUI =         [\${NOGUI}]                  "
    echo "--------------------------------(Custom)----"
    echo "                                            "
    echo -n "Hit any key to continue launch           "
    read ANSWER
fi

#------------------------------------------------------------
#  Part 6: Launch the Vehicles
#------------------------------------------------------------
VARGS=" --sim --auto --max_spd=\$MAX_SPD \$MMOD "
VARGS+=" \$TIME_WARP \$JUST_MAKE \$VERBOSE "
for IX in \`seq 1 \$VAMT\`;
do
    IXX=\$((\$IX - 1))
    IVARGS="\$VARGS --mport=900\${IX}  --pshare=920\${IX} "
    IVARGS+=" --start_pos=\${VEHPOS[\$IXX]} "
    IVARGS+=" --stock_spd=\${SPEEDS[\$IXX]} "
    IVARGS+=" --vname=\${VNAMES[\$IXX]} "
    IVARGS+=" --color=\${VCOLOR[\$IXX]} "
    vecho "Launching vehicle: \$IVARGS"

    CMD="./launch_vehicle.sh \$IVARGS"    
EOF

if [ "${MTASC}" = "yes" ]; then
cat >> $OUTPUT <<EOF
    if [ "\${MTASC}" = "yes" ]; then
	SSH_OPTIONS="-o StrictHostKeyChecking=no -o LogLevel=QUIET "
	SSH_OPTIONS+="-o ConnectTimeout=1 -o ConnectionAttempts=1 "
	SSH_OPTIONS+="-o BatchMode=yes "

	if [ ! -f "\$HOME/.pablo_names" ]; then
	    echo "\$ME: Could not find ~/.pablo_names. Exit Code 3."
	    exit 3
	fi
	
	PNAME=\${PABLOS[\$ARRAY_INDEX]}
	IP=\`find_pablo_ip_by_name.sh \$USE_CACHE \$PNAME | tr -d '\r' \`
	
	vecho "pablo[\$PNAME] IP is: \$IP"
	VARGS+=" \$CLOCK " #custom
	IVARGS+=" \$LOG_CLEAN" # only sent to vehicles in mtasc mode
	IVARGS+=" --mission=\$MISSION --shore=\$SHORE_IP "
	CMD="ssh student2680@\$IP \$SSH_OPTIONS mlaunch.sh \$IVARGS &"
    fi
EOF
fi

cat >> $OUTPUT <<EOF
    eval \$CMD
    sleep 0.5
done

#------------------------------------------------------------
#  Part 7: Launch the Shoreside mission file
#------------------------------------------------------------
SARGS=" --auto --mport=9000 --pshare=9200 \$NOGUI --vnames=abe:ben "
SARGS+=" \$TIME_WARP \$JUST_MAKE \$VERBOSE "
SARGS+=" \$MMOD "
vecho "Launching shoreside: \$SARGS"
./launch_shoreside.sh \$SARGS 

if [ "\${JUST_MAKE}" != "" ]; then
    echo "\$ME: Targ files made; exiting without launch."
    exit 0
fi

#------------------------------------------------------------
#  Part 8: Unless auto-launched, launch uMAC until mission quit
#------------------------------------------------------------
if [ "\${XLAUNCHED}" != "yes" ]; then
    uMAC --paused targ_shoreside.moos
    trap "" SIGINT
    echo; echo "\$ME: Halting all apps"
    kill -- -\$\$
fi

exit 0
EOF

chmod ugo+x $OUTPUT
echo "$OUTPUT generated"
