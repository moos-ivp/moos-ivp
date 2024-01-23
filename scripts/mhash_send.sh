#!/bin/bash 
#-------------------------------------------------------------- 
#   Script: mhash_send.sh    
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
TARG_DIR=""
TARG_GRP=""

DIR=$(basename `pwd`)
txtrst=$(tput sgr0)    # Reset
txtred=$(tput setaf 1) # Red
txtgrn=$(tput setaf 2) # Green
txtblu=$(tput setaf 4) # Blue
txtrok=$(tput setaf 7)$(tput setab 4)$(tput rev)$(tput bold)

root="yodacora@oceanai.mit.edu:/home/yodacora/"
timeout="--timeout=3"
declare -a txt_files=()

vecho "======================================================"
vecho "Beginning Remote Reporting Script.  MIT/mikerb (c)2023"
vecho "======================================================"
    
#-------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
        echo "$ME: [OPTIONS] file.tgz                         "
        echo "                                                "
        echo "Synopsis:                                       "
        echo "  Send given file(s) to oceanai archive site    "
        echo "  using rsync and temporary ssh-agent.          "
        echo "                                                "
        echo "Options:                                        "
        echo "  --help, -h                                    "
        echo "    Display this help message                   "
        echo "  --verbose, -v                                 "
        echo "    Increase verbosity                          "
        echo "  --dir=<dirname>                               "
        echo "    Target directory                            "
        echo "  --edu, -e                                     "
        echo "    Target directory is edu/directory           "
        echo "                                                "
        echo "Notes:                                          "
        echo "  1. Target dir group is null by default. If    "
        echo "     non-null, it will be sent to grp/dir       "
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="yes"
    elif [ "${ARGI:0:6}" = "--dir=" ]; then
        TARG_DIR="${ARGI#--dir=*}/"
    elif [ "${ARGI}" = "--edu" -o "${ARGI}" = "-e" ]; then
        TARG_GRP="edu"
    elif [[ $ARGI == *.tgz  ]]; then
	txt_files+=($ARGI)
    elif [[ $ARGI == *.tar  ]]; then
	txt_files+=($ARGI)
    elif [[ $ARGI == *.npos  ]]; then
	txt_files+=($ARGI)
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

#-------------------------------------------------------
#  Part 4: Ensure the proper SSH Key exists
#-------------------------------------------------------
if [ -f ~/.ssh/id_rsa_yco ] ; then    
    vecho $txtblu"Found yodacora ssh key in ~/.ssh/" $txtrst
    chmod -R go-rwx ~/.ssh/id_rsa_yco &> /dev/null
elif [ -f ~/.ssh/foodir/id_rsa_yco ] ; then    
    vecho $txtblu"Found yodacora ssh key in ~/.ssh/foodir" $txtrst
    chmod -R go-rwx ~/.ssh/foodir/id_rsa_yco &> /dev/null
else
    vecho -n $txtred
    vecho -n "yodacora ssh key was not found in ~/.ssh/."
    vecho "Exiting." $txtrst
    exit 1;
fi

#-------------------------------------------------------
#  Part 5: Launch a new temporary ssh-agent and confirm
#-------------------------------------------------------
eval `ssh-agent -s` &> /dev/null
ps -p $SSH_AGENT_PID &> /dev/null
SSH_AGENT_RUNNING=$?

if [ ${SSH_AGENT_RUNNING} = "1" ] ; then
    vecho $txtred"unable to start ssh-agent. Exiting" $txtrst
    exit 1
else
    vecho $txtblu"ssh-agent started ok. PID:" $SSH_AGENT_PID $txtrst
fi

#-------------------------------------------------------
#  Part 6: Add the ssh-key to the ssh-agent and confirm
#-------------------------------------------------------
ok_add=1
if [ -f ~/.ssh/id_rsa_yco ]; then
    ssh-add -t 7200 ~/.ssh/id_rsa_yco 2> /dev/null
    ok_add=$?
elif [ -f ~/.ssh/foodir/id_rsa_yco ]; then
    ssh-add -t 7200 ~/.ssh/foodir/id_rsa_yco 2> /dev/null
    ok_add=$?
fi

# USMA addition
if [ -f ~/.ssh/id_rsa_yco_usma ]; then
    ssh-add -t 7200 ~/.ssh/id_rsa_yco_usma 2> /dev/null
    usma_add=$?
    if [ "$usma_add" -ne "0" ] ; then 
	vecho $txtred"Adding (OPTIONAL) USMA ssh key to ssh-agent: FAIL" $txtrst
    else
	vecho $txtblu"ADDING USMA ssh key to ssh-agent: OK" $txtrst
    fi
fi


if [ "$ok_add" -ne "0" ] ; then 
    vecho $txtred"Adding ssh key to ssh-agent: FAIL" $txtrst
    exit 1
else
    vecho $txtblu"ADDING ssh key to ssh-agent: OK" $txtrst
fi

#-------------------------------------------------------
#  Part 7: Copy all txt files to the destination
#-------------------------------------------------------
COPY_ONE="no"
COPY_FAIL="no"

if [ "${TARG_GRP}" != "" ]; then
    TARG_DIR="${TARG_GRP}/${TARG_DIR}"
fi

for tfile in "${txt_files[@]}"
do
    SCP_OK=1
    if [ "${VERBOSE}" = "yes" ]; then
	rsync -aP $tfile yodacora@oceanai.mit.edu:/home/yodacora/$TARG_DIR
	#scp $tfile yodacora@oceanai.mit.edu:/home/yodacora/ 
    else
	rsync -aP -q $tfile yodacora@oceanai.mit.edu:/home/yodacora/$TARG_DIR
	#scp -q $tfile yodacora@oceanai.mit.edu:/home/yodacora/ 
    fi
    RSYNC_OK=$?

    if [ ${RSYNC_OK} = 0 ]; then
	COPY_ONE="yes"
    else
	COPY_FAIL="yes"
    fi
done

#-------------------------------------------------------
#  Part 8: Kill the temporary ssh-agent
#-------------------------------------------------------
kill -9 $SSH_AGENT_PID

#-------------------------------------------------------
#  Part 9: Return success (0) if at least one file has
#          been copied, and no file copies failed.
#-------------------------------------------------------
if [ ${COPY_ONE} != "yes" ]; then
    vecho $txtred"Failed to copy at least one file." $txtrst
    exit 1
elif [ ${COPY_FAIL} != "no" ]; then
    vecho $txtred"Not all files were successfully copied." $txtrst
    exit 1
fi

exit 0

