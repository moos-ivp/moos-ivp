#!/bin/bash 
#---------------------------------------------------------------
#   Script: mhash_archive_all.sh
#   Author: Mike Benjamin
#   LastEd: May 2023
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$1"; fi }

#---------------------------------------------------------------
#  Part 2: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
DIR=$(pwd)
LEVEL=0
CONFIRM=""
VERBOSE=""
DRYRUN=""
MAX_AGE=""
FLOW_DOWN_ARGS=""

txtrst=$(tput setaf 0)  # Reset
txtred=$(tput setaf 1)  # Red
txtgrn=$(tput setaf 2)  # Green
txtblu=$(tput setaf 4)  # Blue

#---------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [level]                             "
	echo "                                                  "
	echo "Synopsis:                                         "
	echo "  $ME will process the current directory or given "
	echo "  directory, and recursively archive any folders  "
	echo "  containining an alog file.                      "
	echo "                                                  "
	echo "Options:                                          "
	echo "  --help, -h         Show this help message       "
	echo "  --verbose, -v      Verbose output               "
	echo "  --dryrun, -d       Dry run, no actions          "
	echo "  --confirm, -c      Confirm before archiving     "
	echo "  --dir=<dirname>    Begin in given directory     "
	echo "  --max_age=<N>      Ignore logs  > N hours old   "
	echo "  --today, -t        Same as --max_age=16         "
	echo "                                                  "
	echo "  --min_odo=<N>      Ignore logs with odometry <N "
	echo "  --odo_two, -o      Same as --min_odo=2          "
	echo "                                                  "
	echo "  --min_dur=<N>      Ignore logs with duration <N "
	echo "  --5                Same as --min_dur=5          "
	echo "  --10               Same as --min_dur=5          "
	echo "  --30               Same as --min_dur=30         "
	echo "  --60               Same as --min_dur=60         "
	echo "                                                  "
	echo "  -x                 Same as --min_dur=5, --today "
	echo "                             --min_odo=5          "
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" ]; then
        LEVEL=$ARGI
    elif [ "${ARGI:0:6}" = "--dir=" ]; then
        DIR="${ARGI#--dir=*}"

    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"
    elif [ "${ARGI}" = "--dryrun" -o "${ARGI}" = "-d" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"
    elif [ "${ARGI}" = "--confirm" -o "${ARGI}" = "-c" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"

    elif [ "${ARGI:0:10}" = "--max_age=" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"
    elif [ "${ARGI}" = "--today" -o "${ARGI}" = "-t" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"

    elif [ "${ARGI:0:10}" = "--min_odo=" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"
    elif [ "${ARGI}" = "--odo_two" -o "${ARGI}" = "-o" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"

    elif [ "${ARGI:0:10}" = "--min_dur=" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"
    elif [ "${ARGI}" = "--30" -o "${ARGI}" = "--60" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"
    elif [ "${ARGI}" = "--5" -o "${ARGI}" = "--10" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"

    elif [ "${ARGI}" = "--x" -o "${ARGI}" = "-x" ]; then
	FLOW_DOWN_ARGS+=" $ARGI"

    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

# create string of blanks with string length $LEVEL 
BUF=`printf %${LEVEL}s`
FLOW_DOWN_ARGS+=" $((LEVEL+3))"

#---------------------------------------------------------------
#  Part 4: Sanity check that a given directory exits
#---------------------------------------------------------------
if [ ! -d "${DIR}" ]; then
    echo "Directory: [$DIR] does not exist. Exit code 2."
    exit 2
else
    cd $DIR
fi


#---------------------------------------------------------------
#  Part 5: Determine if current directory has an alog file
#---------------------------------------------------------------
HAS_AN_ALOG="no"
ls *.alog >& /dev/null
if [ $? = 0 ]; then
    HAS_AN_ALOG="yes"
fi

if [ "${HAS_AN_ALOG}" = "yes" ]; then
    DO_THE_ARCHIVE="yes"
    if [ "${CONFIRM}" != "" ]; then
       echo "$BUF$ME Found alog in $PWD. Continue with archive? [Y/n]"
       echo -n "$BUF > "
       read ANSWER
       if [ "${ANSWER}" = "n" ]; then
	   DO_THE_ARCHIVE="no"
       fi
    fi
       
    if [ "${DO_THE_ARCHIVE}" = "yes" ]; then
	echo "$BUF$ME Archiving: ${PWD##*/}"
	mhash_archive.sh $FLOW_DOWN_ARGS
    fi

    exit 0
fi

echo "$BUF Examining directory: ${PWD##*/}"

for file in *; do
    if [ "${file}" = "tmp" -o "${file}" = "attic" ]; then
	continue
    fi

    if [ -d $file ]; then
	if [ "${LEVEL}" = "0" -o "${LEVEL}" = "3" ]; then
	    cd $file;
	    mhash_archive_all.sh $FLOW_DOWN_ARGS
	    cd ..
	fi
    fi
done
