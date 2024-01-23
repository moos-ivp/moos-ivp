#!/bin/bash 
#---------------------------------------------------------------
#   Script: mhash_archive_grp.sh
#   Author: Mike Benjamin
#   LastEd: Jan 2024
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#---------------------------------------------------------------
#  Part 2: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
DIR="exp"
VERBOSE=""
SEND="no"
CLEAN="no"

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
	echo "  $ME will archive log folders in the current dir "
	echo "  creating common folder based on mission hash for"
	echo "  each folder. Usually N vehicles plus shoreside. "
	echo "                                                  "
	echo "  For each mission a single folder and tar file   "
	echo "  of that folder will be created.                 "
	echo "                                                  "
	echo "  The tar file can optionally be sent to a server "
	echo "  for archiving. The directory on that server may "
	echo "  be specified.                                   "
	echo "                                                  "
	echo "  Optionally the tar file can be removed upon a   "
	echo "  successful send.                                "
	echo "                                                  "
	echo "  NOTE: This tool is used in mission folders where"
	echo "        where the whole N-vehicle simulation took "
	echo "        place, unlike on a vehicle or PABLO where "
	echo "        the log file(s) represent a single node.  "
	echo "                                                  "
	echo "Options:                                          "
	echo "  --help, -h         Show this help message       "
	echo "  --verbose, -v      Verbose output               "
	echo "  --send, -s         Send the tarfiles            "
	echo "  --clean, -c        Remove successfully sent tars"
	echo "  --dir=<dirname>    Location on archive server   "
	echo "                                                  "
	echo "Example:                                          "
	echo "  $ $ME -v -s --dir=exp/encircle                  "
	exit 0
    elif [ "${ARGI:0:6}" = "--dir=" ]; then
        DIR="${ARGI#--dir=*}"

    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="--verbose"
    elif [ "${ARGI}" = "--send" -o "${ARGI}" = "-s" ]; then
	SEND="yes"
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ]; then
	CLEAN="yes"
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

#---------------------------------------------------------------
#  Part 4: Comb current dir and handle any sub-folder with .alog
#          by creating an .tgz of the sub-folder with a name set
#          based on the MISSION_HASH. Then place the .tgz file in
#          a new (if needed) folder based on mission hash.
#---------------------------------------------------------------
for file in *; do
    if [ -d $file ]; then
	vecho "Entering dir: $file"
	
	if [ "${file}" = "tmp" -o "${file}" = "attic" ]; then
	    continue
	elif [ -f "${file}/mission.march" ]; then
	    continue
	elif [ -f "${file}/mission.mhi" ]; then
	    continue
	fi
	
	cd $file;
	mhash_archive.sh --dryrun 
	cd ..

	MHASH=`mhash_query.sh -m $file/*.mhash`
	if [ "${MHASH}" != "" ]; then

	    if [ ! -d $MHASH ]; then
		mkdir $MHASH
	    fi

	    touch "${file}/mission.march"
	    vecho "Copying $file to $MHASH"
	    cp -rp $file/*.tgz $MHASH
	    touch "$MHASH/mission.mhi"
	fi
    fi
done

#---------------------------------------------------------------
#  Part 5: Tar any untar'ed mission folders
#---------------------------------------------------------------
for file in *; do
    if [ -d $file ]; then
	# Don't tar any folders w/out mission.mhi indicator file
	if [ ! -f "$file/mission.mhi" ]; then
	    continue;
	fi

	TARFILE="${file}.tar"
	if [ -f $TARFILE ]; then
	    vecho "Tarfile already exists:" $TARFILE
	    continue
	fi
	
	vecho "Creating tarfile: $TARFILE" 
	tar cvf "${TARFILE}" "${file}" >& /dev/null
    fi
done

#---------------------------------------------------------------
#  Part 6: Send the file and report if unsuccessful
#---------------------------------------------------------------
if [ "${SEND}" != "yes" ]; then
    exit 0
fi

for file in *; do
    if [[ $file == *.tar ]]; then
	
	mhash_send.sh $VERBOSE --dir="${DIR}" "${file}" 
	SEND_OK=$?

	if [ $SEND_OK != 0 ]; then
	    vecho "Unable to send tar file: [${file}]."
	else
	    vecho "File is archived: [${file}]"
	    if [ "${CLEAN}" = "yes" ]; then
		vecho "Cleaning sent file: $file"
		rm -f $file
	    fi
	fi
    fi
done

exit 0
