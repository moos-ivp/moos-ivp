#!/bin/bash

#=============================================================
# Mike Benjamin, December 26th, 2017
#=============================================================
# This script will determine the lines of C++ code for for all
# subdirectories.
# 
# Each line will be of the form:
#    pFoobar   <number>
#
# It is intended that the user will likely generate this output
# to a file by simply re-directing the output which otherwise
# goes to the terminal:
#   $ ./build-info.sh > filename
#
# Initial motivation for this is to generate stats to be used
# by the manifest_wiki utility.
#
# The redirection of stderr in sloccount is to suppress MD5 error
# messages often found on MacOS where the md5sum utility is
# typically not installed.

for D in *; do
    if [ -d "${D}" ]; then
        LOC=`sloccount "${D}" 2> /dev/null | fgrep "cpp:" | awk -F' ' '{print $2}'`
        FOC=`sloccount --filecount "${D}" 2> /dev/null | fgrep "cpp:" | awk -F' ' '{print $2}'`

	WYR=`sloccount "${D}" 2> /dev/null | fgrep "Development" | awk -F' ' '{print $7}'`
	printf "module=${D}, loc=${LOC}, foc=${FOC}, wkyrs=${WYR}\n"

    fi
done

exit 0

