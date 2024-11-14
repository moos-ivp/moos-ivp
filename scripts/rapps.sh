#!/bin/bash 
#---------------------------------------------------------------
#  Script: rapps.sh
#  Author: Michael Benjamin
#  LastEd: November 2nd, 2024
#---------------------------------------------------------------                       
#  Part 1: Define a convenience function for producing terminal                        
#          debugging/status output depending on the verbosity.                         
#---------------------------------------------------------------                       
vecho() { if [ "$VERBOSE" != "" ]; then echo "$1"; fi }

#---------------------------------------------------------------                       
#  Part 2: Set global var defaults                                                     
#---------------------------------------------------------------
VERBOSE=""
KILLALL=""

rst=$(tput setaf 0)  # Reset                                                    
red=$(tput setaf 1)  # Red                                                      
grn=$(tput setaf 2)  # Green                                                    
blu=$(tput setaf 4)  # Blue                                                     
pur=$(tput setaf 5)  # Purple                                                   
cyn=$(tput setaf 6)  # Cyan                                                     
bold=$(tput bold)
normal=$(tput sgr0)

COMMON_APPS=("MOOSDB" "pLogger" "pAntler" "pHelmIvP" "pNodeReporter")
COMMON_APPS+=("uFldNodeComms" "uFldShoreBroker" "uFldNodeBroker")
COMMON_APPS+=("pShare" "uProcessWatch" "pRealm" "pHostInfo")
COMMON_APPS+=("uMayFinish" "pAutoPoke" "pMarineViewer")
COMMON_APPS+=("pMarinePID" "pMarinePIDV22" "uLoadWatch")
COMMON_APPS+=("pContactMgrV20" "uMemWatch")
COMMON_APPS+=("pEchoVar" "uDelve")

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    ALL_ARGS+=$ARGI" "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "rapps.sh [OPTIONS] [time_warp]     "
	echo "Synopsis:                                                " 
	echo "  Check a list of apps to see if they are running, using " 
	echo "  the pgrep command.                                     " 
	echo "                                                         " 
	echo "Options:                                                 " 
	echo "  --help, -h         Show this help message              " 
	echo "  --verbose, -v      Show the results for each app       " 
	echo "  --killall, -k      Send killall to any dangling apps   " 
	echo "                                                         " 
	echo "Options:                                                 " 
	echo "  0 if none of the given apps are running                " 
	echo "  1 if one or more of the given apps are running         " 
	exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE="yes"
    elif [ "${ARGI}" = "--killall" -o "${ARGI}" = "-k" ]; then
        KILLALL="yes"
    fi
done

#-------------------------------------------------------
#  Part 3: Iterate through the apps
#-------------------------------------------------------

if [ "${KILLALL}" = "" ]; then
    vecho "Status   App Name"
    vecho "-------  ---------"
else
    vecho "Status   Action      NewStat   App Name"
    vecho "-------  ---------   -------   ---------"
fi

# Exit val success=0 until unresolvable dangling app found
EXIT_VAL=0
for app in "${COMMON_APPS[@]}"
do
    REPORT="cleared "
    pgrep -x $app > /dev/null

    # If a dangling app is encountered:
    if [ $? = 0 ]; then
	REPORT="${red}${bold}RUNNING${rst}${normal}"

	# If action is to be taken to kill dangling app:
	if [ "${KILLALL}" != "" ]; then 	    
	    REPORT+="${bold}${blu}  killall-9 ${normal}"
	    killall -9 $app
	    pgrep -x $app > /dev/null
	    if [ $? = 0 ]; then
		REPORT+="${red}${bold}  RUNNING  ${rst}${normal}"
		EXIT_VAL=1
	    else
		REPORT+="${grn}${bold}  CLEARED  ${rst}${normal}"
	    fi
	else
	    REPORT+=" ---         ---      "
	    EXIT_VAL=1
	fi
    else
	if [ "${KILLALL}" != "" ]; then 	    
	    REPORT+=" "
	fi
    fi
    vecho "$REPORT $app"
done

exit $EXIT_VAL
