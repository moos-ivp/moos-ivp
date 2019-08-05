#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'
# Define local echo cmds to do nothing unless verbose or debug mode
myecho() { : ; }
dbecho() { : ; }


#==================================================================
# Part 1: Handle Command Line Arguments
#==================================================================
FIND_APP=""
VERBOSE=""
FORCE=""
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "=========================================================" 
	echo "zkill.sh: APPNAME [OPTIONS]                              "
	echo "=========================================================" 
	echo "                                                         "
	echo "SYNOPSIS                                                 "
	echo "  The zkill script will find the directory of the given  "
	echo "  MOOSApp and then attempt to kill all running instances "
	echo "  of any binary found in that bin/ directory.            "
	echo "                                                         "
	echo "Options:                                                 "
	echo "  --help, -h       Display this help message.            "
	echo "  --verbose, -v    Show which processes killed           "
	echo "  --debug, -d      Show full debugging messages          "
	echo "  --force, -f      Continue even if system dir detected  "
	exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	myecho() { echo "$@" ; }
    elif [ "${ARGI}" = "--debug" -o "${ARGI}" = "-d" ] ; then
	dbecho() { echo "$@" ; }
    elif [ "${ARGI}" = "--force" -o "${ARGI}" = "-f" ] ; then
	FORCE="yes"
    else
        FIND_APP=$ARGI
    fi
done

#==================================================================
# Part 2: Using the given FIND_APP, determine if it exists in the
#         shell path, and if so, get the directory name. If it seems
#         to be a system directory, quit unless --force is used.
#==================================================================

if [[ $FIND_APP == "" ]]; then
    dbecho "No FIND_APP provided. Exiting."
    exit 1
fi

FULL_PATH="$(which $FIND_APP)"
if [[ $FULL_PATH == "" ]]; then
    dbecho "FIND_APP not found in the current shell path. Exiting."
    exit 1
fi
dbecho "Full Path found: ["$FULL_PATH"]" 

BIN_DIR=$(dirname "${FULL_PATH}") 
dbecho "Full directory: ["$BIN_DIR"]" 

# If it seems to be a system directory, quit zkill unless --force is used.
if [[ "$FORCE" != "yes" ]] && [[ $FULL_PATH = *"/usr/"* ]]; then
  dbecho "The app ["$FIND_APP"] detected as a system utility. zkill exiting."
  exit 1
fi


#==================================================================
# Part 3: Determine if pidof is installed. Not a dealbreaker if not
#         but it makes a nicer script if so. Installed by default
#         in GNU/Linux. Optional port install in OSX.  
#==================================================================
PIDOF_DIR="$(which pidof)"

HAS_PIDOF="yes"
if [[ $PIDOF_DIR == "" ]]; then
    HAS_PIDOF="no"
    dbecho "Binary pidof was no found. Consider installing. " 
else
    dbecho "Binary pidof was found: ["$PIDOF_DIR"]" 
fi



#==================================================================
# Part 4: Go into the bin directory and handle each executable
#==================================================================

cd $BIN_DIR >& /dev/null

FILES=$BIN_DIR/*
for file in $FILES
do
    xfile=$(basename "${file%.*}")
    #xfile=$(basename "${file}")


    # Ignore non-executables and files that had a suffix, e.g., foo.sh
    if [[ ! -x "$xfile" ]]
    then
	dbecho "File '$xfile' is not executable, ignoring"
	continue
    fi

    # Ignoring directories
    if [[ -d "$xfile" ]]
    then
	dbecho "'$xfile' is a directory, ignoring"
	continue
    fi

    # If the directory of "which app" is not the BIN_DIR, then we ignore
    # this app. We don't want to be duped into killing a system app of
    # the same name.
    THIS_PATH_NAME="$(which ${xfile})"
    THIS_DIR=$(dirname "${THIS_PATH_NAME}") 
    if [[ "$THIS_DIR" != "$BIN_DIR" ]]; then
	dbecho "Ignoring executable $file since it is ambiguous"
	continue
    fi

    # Ignore certain named apps, just because we can
    if [[ $xfile == "GenMOOSApp" ]]; then
	continue
    fi
    if [[ $xfile == "GenMOOSApp_AppCasting" ]]; then
	continue
    fi

    #===================================================================
    # If we have pidof installed, let's kill by PID and make nice report
    #===================================================================
    if [[ $HAS_PIDOF == "yes" ]]; then
	PIDS="$(pidof $xfile)"
	for pid in $PIDS
	do
	    myecho -e "Killing $RED $xfile $NC (pid: "$pid")"
	    kill -9 $pid
	done
    fi

    #===================================================================
    # If no pidof installed, kill silently by using killall
    #===================================================================
    if [[ $HAS_PIDOF == "no" ]]; then
	myecho "Executing killall -9 on: " $xfile
	killall -9 $xfile >& /dev/null
    fi
done


#==================================================================
# Part 5: Let the user know we're done. Prod user to install pidof
#==================================================================

myecho "Done Killing All Apps." 

if [[ $HAS_PIDOF == "no" ]]; then
    myecho -e $RED "For a smarter and more informative version of zkill," 
    myecho -e " please install the utility \"pidof\"  $NC"
fi

cd - >& /dev/null

exit 0

