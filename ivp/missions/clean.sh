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
#  Part 1: Determine the current directory name
#-------------------------------------------------------

DIR=$(basename `pwd`)



#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
HELP="no"
RECURSE="no"

for ARGI; do
    if [ "${ARGI}" = "-r" ] ; then
        RECURSE="yes"
    elif [ "${ARGI}" = "--recurse" ] ; then
	RECURSE="yes"
    elif [ "${ARGI}" = "--help" ] ; then
	HELP="yes"
    elif [ "${ARGI}" = "-h" ] ; then
	HELP="yes"
    else
	DIR=$ARGI/$DIR	
    fi
done

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]             \n" $0
    printf "Switches:                 \n" 
    printf "  --recurse, -r           \n" 
    printf "  --help, -h              \n" 
    exit 0;
fi



#-------------------------------------------------------
#  Part 3: Announce where we are cleaning
#-------------------------------------------------------

echo Cleaning ${txtblu} $DIR ${txtrst}

#-------------------------------------------------------
#  Part 4: Look for and potentially use a makefile. By 
#          default, if a makefile is found we don't go
#          into subdirectories, unless --recurse invoked.
#-------------------------------------------------------

if [ -e makefile ]; then
    echo $BUF${txtgrn} Found makefile, executing make clean  ${txtrst}
    make clean
    if [ "${RECURSE}" = "no" ]; then
	exit; # Assume that a makefile handles things recursively.
    fi
fi

#-------------------------------------------------------
#  Part 5: Look for and potentially use a clean.sh script.
#          By default, if a script is found we don't go
#          into subdirectories, unless --recurse invoked.
#-------------------------------------------------------

if [ -e clean.sh ]; then
    echo $BUF${txtgrn} Found and executed clean.sh ${txtrst}
    ./clean.sh
    if [ "${RECURSE}" = "no" ]; then
	exit; # Assume that clean.sh handles things recursively.
    fi
fi

#-------------------------------------------------------
#  Part 6: Go into each of the subdirectories and repeat. 
#-------------------------------------------------------

for file in *; do
   if [ -d $file ]; then
      cd $file; 
      rclean.sh $DIR
      cd .. 
   fi
done
