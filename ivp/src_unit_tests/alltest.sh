#!/bin/bash 

txtrst=$(tput setaf 0)  # Reset
txtred=$(tput setaf 1)  # Red
txtgrn=$(tput setaf 2)  # Green
txtylw=$(tput setaf 3)  # Yellow
txtblu=$(tput setaf 4)  # Blue
txtpur=$(tput setaf 5)  # Purple
txtcyn=$(tput setaf 6)  # Cyan
txtwht=$(tput setaf 7)  # White

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
HELP="no"

for ARGI; do
    if [ "${ARGI}" = "--help" ] ; then
	HELP="yes"
    elif [ "${ARGI}" = "-h" ] ; then
	HELP="yes"
    fi
done

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]             \n" $0
    printf "Switches:                 \n" 
    printf "  --help, -h              \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 2: Go into each of the subdirectories and repeat. 
#-------------------------------------------------------

ALL_OK=0

for file in *; do
   if [ -d $file ]; then
       cd $file; 
       if [ -f cases.utf ]; then
	   echo "==========================================="
	   echo "Checking: "$file"/cases.utf"
	   echo "==========================================="
	   utest cases.utf -v
	   if [ $? != 0 ]; then
	       ALL_OK=1
	   fi
       else
	   echo "==========================================="
	   echo "NOTE: "$file " has no cases.utf file to check"
	   echo "==========================================="
       fi
       cd .. 
   fi
done

exit $ALL_OK
