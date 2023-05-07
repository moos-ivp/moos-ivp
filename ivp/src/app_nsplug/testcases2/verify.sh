#!/bin/bash
#-------------------------------------------------------------- 
#   Script: verify.sh    
#   Author: Michael Benjamin   
#   LastEd: May 2023
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#-------------------------------------------------------------- 
#  Part 2: Set Global variables
#-------------------------------------------------------------- 
ME=`basename "$0"`
VERBOSE=""
TEST_FILE_ARRAY=()
TEST_FILE="test_cases.txt"
CURR_FILE=
ALL_OK="yes"
FIRST_FAIL_HALT="no"

#-------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME: [OPTIONS] [time_warp]                          "
	echo "                                                    "
	echo "Options:                                            "
        echo "  --help, -h                                        "
        echo "    Display this help message                       "
        echo "  --verbose, -v                                     "
        echo "    Display verbose output                          "
        echo "  --ffh                                             "
        echo "    First fail halt                                 "
        echo "  --clean, -c                                       "
        echo "    Just clean all the temporary files and exit     "
	exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="--verbose"
    elif [[ $ARGI == test*.txt ]]; then
	TEST_FILE_ARRAY+=("$ARGI")
    elif [[ $ARGI == test*.txt~ ]]; then
	echo "Ignoring:["$ARGI"]"
    elif [ "${ARGI}" = "--clean" -o "${ARGI}" = "-c" ]; then
	rm -f tmp_* *_beg.txt *_tar.txt *_res.txt *~
	exit 0
    elif [ "${ARGI}" = "--ffh" ]; then
	FIRST_FAIL_HALT="yes"
    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 4: Clean all previously generated temp files.
#-------------------------------------------------------
rm -f tmp_* *_beg.txt *_tar.txt *_res.txt *~


#-------------------------------------------------------
#  Part 5: Split into _beg (beginning) and _tar (target) files
#-------------------------------------------------------
for TEST_FILE in "${TEST_FILE_ARRAY[@]}"
do
    echo "Expanding file:[$TEST_FILE]"
    while read -r line;
    do
	#vecho "Line:[$line]"
	# Reject comment and empty lines
	if [[ -z "${line// }" ]]; then
	    continue
	elif [ "${line:0:2}" = "//" ]; then
	    continue
	fi
	
	if [ "${line:0:5}" = "File:" ]; then
	    CURR_FILE="${line#File:*}"
	else
	    if [ "${CURR_FILE}" != "" ]; then
		echo $line >> $CURR_FILE
	    fi
	fi
    done < $TEST_FILE
done


#-------------------------------------------------------
#  Part 6: Create the result files using nsplug 
#-------------------------------------------------------
for beg_file in *_beg.txt
do
    echo -n "Testing file:["$beg_file"]"

    first_line=$(head -n 1 $beg_file)
    if [ "${first_line:0:5}" != "Args:" ]; then
	echo "Skipping beg_file [$file], missing Line 1 Args:"
	continue
    fi

    res_file="${beg_file/beg.txt/res.txt}"
    tar_file="${beg_file/beg.txt/tar.txt}"
    base="${beg_file/_beg.txt/}"
    
    NSPLUG_ARGS="${first_line#Args:*}"

    # Modify result file: remove args line
    tail -n +2 $beg_file > tmp_file && mv -f tmp_file $beg_file

    if [ "$VERBOSE" != "" ]; then
	nsplug $beg_file $res_file $NSPLUG_ARGS
    else
	vecho "nsplug args:["$NSPLUG_ARGS"]"
	vecho "beg_file:["$beg_file"]"
	vecho "res_file:["$res_file"]"
	nsplug $beg_file $res_file $NSPLUG_ARGS >& /dev/null
    fi

    #cat "$res_file" | tail -r | tail -n +2 | tail -r > tmp_file
    #mv -f tmp_file $res_file

    # Modify result file: remove trailing blank line
    #head -n 1 "$res_file" > tmp_file && mv -f tmp_file $res_file

    RESULT=`diff -B -b $res_file $tar_file`
    if [ $? = 0 ]; then
	echo "[Success]"
    else
	echo "[FAIL]"
	ALL_OK="no"
	diff -B -b $res_file $tar_file
    fi

    if [ "${ALL_OK}" = "no" -a "${FIRST_FAIL_HALT}" = "yes" ]; then
	exit 1
    fi

done

if [ "${ALL_OK}" = "no" ]; then
    exit 1
fi

exit 0
 
