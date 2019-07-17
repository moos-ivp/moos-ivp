#!/bin/bash 
#---------------------------------------------------------
# File: launch.sh
# Name: Mike Benjamin
# Date: May 8th, 2019
# Note: Goal of general pavilion vehicle launch script
#---------------------------------------------------------
#  Part 1: Initialize configurable variables with defaults
#---------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
CLOCKWISE="false"
RANDSTART="false"
AMT=1

#---------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#---------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp]     "
	echo "  --just_make, -j                    " 
	echo "  --help, -h                         " 
	echo "  --amt=N          (default is 1)    " 
	echo "  --clockwise, -c                    " 
	echo "  --norand                           " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--clockwise" -o "${ARGI}" = "-c" ] ; then
	CLOCKWISE="true"
    elif [ "${ARGI}" = "--rand" -o "${ARGI}" = "-r" ] ; then
	RANDSTART="true"
    elif [ "${ARGI:0:6}" = "--amt=" ] ; then
        AMT="${ARGI#--amt=*}"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

if [ ! $AMT -ge 1 ] ; then
    echo "Vehicle amount must be >= 1. Exiting now."
    exit 1
fi

#-------------------------------------------------------------
# Part 3: Build the Shoreside mission file
#-------------------------------------------------------------
SHORE=localhost:9300
nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   VNAME="shoreside" 

#-------------------------------------------------------------
# Part 4: Generate random starting positions, speeds and vnames
#         NEAST Poly: 90,20 : 150,30 : 200,-25 : 160,-65, 90,-15
#         WEST  Poly: -30,-30 : -30,-135 : 15,-135 : 10,-30
#         SEAST Poly: 145,-120 : 170,-135 : 140,-175 : 125,-160
#-------------------------------------------------------------
if [ "${RANDSTART}" = "true" ] ; then
    pickpos --poly="-30,-30:-30,-135:15,-135:10,-30"      \
            --poly="90,20:150,30:200,-25:160,-65:90,-15"  \
            --poly="145,-120:170,-135:140,-175:125,-160"  \
	    --amt=$AMT --hdg=75,-100,0  > vpositions.txt  
    pickpos --amt=$AMT --spd=1:4 > vspeeds.txt
    pickpos --amt=$AMT --vnames  > vnames.txt
    pickpos --amt=$AMT --grps=alpha,alpha,bravo:alt  > vgroups.txt
fi
VEHPOS=(`cat vpositions.txt`)
SPEEDS=(`cat vspeeds.txt`)
VNAMES=(`cat vnames.txt`)
GROOPS=(`cat vgroups.txt`)

#-------------------------------------------------------------
# Part 5: Generate the Vehicle mission files
#-------------------------------------------------------------
for INDEX in `seq 1 $AMT`;
do
    ARRAY_INDEX=`expr $INDEX - 1`
    START_POS=${VEHPOS[$ARRAY_INDEX]}
    VNAME=${VNAMES[$ARRAY_INDEX]}
    SPEED=${SPEEDS[$ARRAY_INDEX]}
    GROUP=${GROOPS[$ARRAY_INDEX]}
    SPEED="${SPEED#speed=*}"
    
    VPORT=`expr $INDEX + 9000`
    LPORT=`expr $INDEX + 9300`
     
    echo "Vehicle:" $VNAME "POS:" $START_POS "V:" $SPEED         \
	 "GROUP:" $GROUP "DB_PORT:" $VPORT "PS_PORT:" $LPORT

    nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP \
	   VNAME=$VNAME   START_POS=$START_POS   SHORE=$SHORE    \
	   VPORT=$VPORT   SHARE_LISTEN=$LPORT                    \
	   VTYPE="kayak"  GROUP=$GROUP
    
    if [ "${VNAME}" = "ben" ] ; then
	nsplug meta_vehicle_convoy.bhv targ_$VNAME.bhv -f VNAME=$VNAME  \
	       START_POS=$START_POS   ORDER=$ORDER  PATTERN=1        \
               CLOCKWISE=$CLOCKWISE   SPEED=$SPEED
    else
	nsplug meta_vehicle.bhv targ_$VNAME.bhv -f  VNAME=$VNAME     \
	       START_POS=$START_POS   ORDER=$ORDER  PATTERN=1        \
               CLOCKWISE=$CLOCKWISE   SPEED=$SPEED
    fi
done

#-------------------------------------------------------------
# Part 5: Allow to exit now if just want to examine the mission
#         files without launching.
#-------------------------------------------------------------
if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------------
# Part 6: Launch the Shoreside community
#-------------------------------------------------------------
printf "Launching Shoreside MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
sleep 0.1

#-------------------------------------------------------------
# Part 7: Launch the Vehicle communities
#-------------------------------------------------------------
for INDEX in `seq 1 $AMT`;
do 
    ARRAY_INDEX=`expr $INDEX - 1`
    VNAME=${VNAMES[$ARRAY_INDEX]}
    printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
    pAntler targ_$VNAME.moos >& /dev/null &
    sleep 0.1
done

#-------------------------------------------------------------
# Part 8: Launch uMac until the mission is quit
#-------------------------------------------------------------

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
mykill
printf "Done killing processes.   \n"


