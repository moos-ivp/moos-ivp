#!/bin/sh -e

# This script's job is to ensure that all of the source-related text files in
# some ivp/src/* directory tree, have the Unix end-of-line markers "\n", rather
# than the Windows' markers "\r\n".
#
# If we don't do this, then Subversion thinks there are whole-file merge conflicts
# when it sees the differing line endings.


if [ $# -ne 1 ]
then
    echo 'Usage: enfore-unix-lineberaks-in-ivp-code.sh  <path to ivp/src directory>'
    exit 1
fi

IVP_DIR=${1}

if [ $(find ${IVP_DIR} -maxdepth 1 -name lib_geometry -type d | wc -l) -ne 1 ]
then
    echo "The specified directory doesn't look valid, because it lacks a lib_geometry subdirectory."
    exit 1
fi

find ${IVP_DIR} -name "*.cpp" -or -name "*.c" -or -name "*.h" -or -name CMakeLists.txt | grep -v "lib_fltk-1.1.7/" | xargs dos2unix
