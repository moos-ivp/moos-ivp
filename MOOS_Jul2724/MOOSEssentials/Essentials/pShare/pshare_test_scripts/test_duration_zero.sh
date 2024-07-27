#!/bin/bash

#check tmux is installed
command -v tmux >/dev/null 2>&1 || { echo >&2 "tmux not installed.  Aborting."; exit 1; }
command -v tmuxinator >/dev/null 2>&1 || { echo >&2 "tmuxinator not installed.  Aborting."; exit 1; }


#in your terminal or shell reosource file you will want something like this
#export MOOS_CORE_BINARY_PATH="/Users/pnewman/code/consulting/moos/core-moos/build/bin/"
#export MOOS_ESSENTIALS_BINARY_PATH="/Users/pnewman/code/consulting/moos/essential-moos/build/bin/"


#check we have environment knowledge about MOOS CORE PATHS
if [[ -z "${MOOS_CORE_BINARY_PATH}" ]]; then
    echo >&2 "MOOS_CORE_BINARY_PATH not defined as a env vartiable "; exit 1;
fi

if [[ -z "${MOOS_ESSENTIALS_BINARY_PATH}" ]]; then
    echo >&2 "MOOS_ESSENTIALS_BINARY_PATH not defined as a env vartiable "; exit 1;
fi


export PATH="$MOOS_CORE_BINARY_PATH:$MOOS_ESSENTIALS_BINARY_PATH:$PATH"


# run a tmuxinator script to test zero test duration=0 special case.
tmuxinator start -p test_duration_tmux.yml