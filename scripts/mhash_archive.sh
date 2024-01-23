#!/bin/bash 
#---------------------------------------------------------------
#   Script: mhash_archive.sh
#   Author: Mike Benjamin
#   LastEd: May 2023
#---------------------------------------------------------------
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#---------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$BUF$ME: $1"; fi }

#---------------------------------------------------------------
#  Part 2: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
DIR=$(pwd)
VERBOSE=""
LEVEL=0
DRYRUN=""
FORCE=""
MAX_AGE=0
MIN_ODO=0
MIN_DUR=0

LANG=en_US.utf-8
LC_ALL=en_US.utf-8

rst=$(tput setaf 0)  # Reset
red=$(tput setaf 1)  # Red
grn=$(tput setaf 2)  # Green
blu=$(tput setaf 4)  # Blue
pur=$(tput setaf 5)  # Purple
cyn=$(tput setaf 6)  # Cyan

#---------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS]                                     "
	echo "                                                  "
	echo "Synopsis:                                         "
	echo "  o $ME is run in a dir containing an .alog       "
	echo "    file and perhaps already an .mhash file.      "
	echo "  o If no .mhash file, it will create one.        "
	echo "  o A compressed tar (.tgz) file will be created, "
	echo "    sent to archive server, using mhash_send.sh   "
	echo "  o Optional args can be used to filter/ignore    "
	echo "    log files that do not meet criteria based on  "
	echo "    age, mission duration, or vehicle odometry    "
	echo "                                                  "
	echo "Options:                                          "
	echo "  --help, -h         Show this help message       "
	echo "  --verbose, -v      Verbose output               "
	echo "  --dryrun, -d       Dry run, no actions          "
	echo "  --force, -f        Re-send regardless           "
	echo "  --max_age=<N>      Ingore logs  > N hours old   "
	echo "  --today, -t        Same as --max_age=16         "
	echo "                                                  "
	echo "  --min_odo=<N>      Ingore logs with odometry <N "
	echo "  --odo_two, -o      Same as --min_odo=2          "
	echo "                                                  "
	echo "  --min_dur=<N>      Ingore logs with duration <N "
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
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="-v"
    elif [ "${ARGI}" = "--dryrun" -o "${ARGI}" = "-d" ]; then
	DRYRUN="yes"
    elif [ "${ARGI}" = "--force" -o "${ARGI}" = "-f" ]; then
	FORCE="--force"
    elif [ "${ARGI:0:10}" = "--max_age=" ]; then
        MAX_AGE="${ARGI#--max_age=*}"
    elif [ "${ARGI}" = "--today" -o "${ARGI}" = "-t" ]; then
        MAX_AGE=16
    elif [ "${ARGI:0:10}" = "--min_odo=" ]; then
        MIN_ODO="${ARGI#--min_odo=*}"
    elif [ "${ARGI}" = "--odo_two" -o "${ARGI}" = "-o" ]; then
        MIN_ODO=2
    elif [ "${ARGI:0:10}" = "--min_dur=" ]; then
        MIN_DUR="${ARGI#--min_dur=*}"
    elif [ "${ARGI}" = "--5" -o "${ARGI}" = "-5" ]; then
        MIN_DUR=30
    elif [ "${ARGI}" = "--10"  -o "${ARGI}" = "-10" ]; then
        MIN_DUR=60
    elif [ "${ARGI}" = "--30"  -o "${ARGI}" = "-30" ]; then
        MIN_DUR=30
    elif [ "${ARGI}" = "--60" -o "${ARGI}" = "-60" ]; then
        MIN_DUR=60
    elif [ "${ARGI}" = "--x" -o "${ARGI}" = "-x" ]; then
        MIN_DUR=5
        MIN_ODO=5
        MAX_AGE=16
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

# create string of blanks with string length $LEVEL 
BUF=`printf %${LEVEL}s`

#---------------------------------------------------------------
#  Part 4: Get the XHash from the MISSION_HASH
#          Example MHASH: 
#          mhash=230512-2147I-DEAF-YURI,odo=1507.5,duration=451.3,name=abe
#          Becomes XHASH:
#          230512-2147I-DEAF-YURI-ABE-451D-999
#---------------------------------------------------------------
mhash_tag.sh $VERBOSE $FORCE $LEVEL
if [ $? != 0 ]; then
    echo "$ME: Unable to find or create an .mhash file. Exit 1."
    exit 1
fi

MHASH_FILE=`mhash_query.sh -t`
MHASH=`mhash_query.sh -m`
XHASH=`mhash_query.sh -x`
AGEH=`mhash_query.sh --ageh`
MDUR=`mhash_query.sh --duration`
MODO=`mhash_query.sh --odo`
FHASH="${MHASH}-${XHASH}"

#---------------------------------------------------------------
#  Part 5: Determine if already archived
#---------------------------------------------------------------
if [ -e "${FHASH}.march" ]; then
    if [ "${FORCE}" = "" ]; then
	vecho "$cyn $MHASH archived previously. Skipping. $rst"
	exit 0
    fi
fi

#---------------------------------------------------------------
#  Part 6A Determine if too old to archive
#---------------------------------------------------------------
if [ "$MAX_AGE" -ne 0 ]; then
    if [ $AGEH -gt $MAX_AGE ]; then
	vecho "$cyn $MHASH too old. Skipping. $rst"
	exit 0
    fi
fi
#---------------------------------------------------------------
#  Part 6B Determine if min duration is not met
#---------------------------------------------------------------
if [ "$MIN_DUR" -ne 0 ]; then
    if [ ! $MDUR -gt $MIN_DUR ]; then
	vecho "$cyn $MHASH duration to short. Skipping. $rst"
	exit 0
    fi
fi
#---------------------------------------------------------------
#  Part 6C Determine if min duration is not met
#---------------------------------------------------------------
if [ "$MIN_ODO" -ne 0 ]; then
    if [ ! $MODO -gt $MIN_ODO ]; then
	vecho "$cyn $MHASH odometry to short. Skipping. $rst"
	exit 0
    fi
fi


#---------------------------------------------------------------
#  Part 7: Create the compressed tar file (.tgz file)
#---------------------------------------------------------------
mkdir "${FHASH}"

cp *._txt *.alog *._moos *.blog *.mhash *.ylog *._bhv "${FHASH}" >& /dev/null

tar cvfz "${FHASH}.tgz" "${FHASH}" >& /dev/null
TAR_OK=$?

vecho "TAR_OK:[${TAR_OK}]"

if [ $TAR_OK != 0 ]; then
    vecho "Unable to make tar file: [${FHASH}.tgz]. Exiting."
    exit 1
fi

rm -rf "${FHASH}"

#---------------------------------------------------------------
#  Part 8: Handle early exit and report if only a dry run
#---------------------------------------------------------------
if [ "${DRYRUN}" = "yes" ]; then
    echo -n $blu
    echo "${BUF} MHASH: [${MHASH_FILE}] "
    echo "${BUF} MHASH: [${MHASH}]      "
    echo "${BUF} XHASH: [${XHASH}]      "
    echo "${BUF} FHASH: [${FHASH}]      "
    echo "${BUF} TGZ:   [${FHASH}.tgz]  "
    echo "${BUF} Dry Run: Nothing sent. "
    echo $rst
    exit 0
fi

#---------------------------------------------------------------
#  Part 9: Send the file and report if unsuccessful
#---------------------------------------------------------------
mhash_send.sh $VERBOSE --dir="${MHASH}" "${FHASH}.tgz" 
SEND_OK=$?

if [ $SEND_OK != 0 ]; then
    vecho "Unable to send tar file: [${FHASH}.tgz]. Exiting."
    exit 1
else
    vecho "File is archived: [${FHASH}.tgz]"
fi

#---------------------------------------------------------------
#  Part 10: Send the npos file and report if unsuccessful
#---------------------------------------------------------------
NPOS_FILE="${FHASH}.npos"
if [ -e $NPOS_FILE ]; then
    vecho "Found .npos file: [${NPOS_FILE}]"
    mhash_send.sh $VERBOSE --dir="${MHASH}" "${NPOS_FILE}" 
    SEND_OK=$?

    if [ $SEND_OK != 0 ]; then
	vecho "Unable to send tar file: [${NPOS_FILE}]. Exiting."
	exit 1
    else
	vecho "File is archived: [${NPOS_FILE}]"
    fi
fi

#---------------------------------------------------------------
#  Part 11: Create the .march file
#  FHASH,UTC
#  230518-1137W-CURT-BEER-ABE-138S-113M,utc=1684542381
#---------------------------------------------------------------
DATE_UTC=`date +%s`
echo "${FHASH},utc=$DATE_UTC" > "${FHASH}.march"


#---------------------------------------------------------------
#  Part 12: Compare mhash to ~/.mash_latest.mhash
#           If older than this one, replace with this one
#---------------------------------------------------------------
YOUNGEST_MHASH="$HOME/.mhash_youngest.mhash"
MHASH_FILE=`mhash_query.sh -t`

if [ -e $YOUNGEST_MHASH ]; then
    AGE1=`mhash_query.sh $YOUNGEST_MHASH --ages -t`
    AGE2=`mhash_query.sh $MHASH_FILE --ages -t`
    if [ "$AGE1" -lt "$AGE2" ]; then
	exit 0
    fi
fi

cp -f $MHASH_FILE $YOUNGEST_MHASH >& /dev/null
exit 0



