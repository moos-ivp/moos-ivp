#!/bin/sh

# Note by mikerb (Nov 20th 2008)
#

INVOCATION_ABS_DIR="`pwd`"

# We want absolute pathnames, which require a little extra work to get...
SCRIPT_DIR="`dirname ${0}`"
cd "${SCRIPT_DIR}"
SCRIPT_ABS_DIR=`pwd`

SRC_ABS_DIR="${SCRIPT_ABS_DIR}/ivp/src"
TARGET_ABS_DIR="${INVOCATION_ABS_DIR}/ivp/src"

if [ "${INVOCATION_ABS_DIR}" != "${SCRIPT_ABS_DIR}" ]; then
   if [ -f "${SRC_ABS_DIR}/CMakeCache.txt" ]; then
      echo ""
      echo "The directory ${SRC_ABS_DIR} contains a file named CMakeCache.txt"
      echo "You're trying to perform an out-of-source build, but that's "
      echo "not possible if you have CMakeCache.txt files in your source "
      echo "tree."
      echo ""
      echo "If you really want to do an out-of-source build, you should delete"
      echo "all CMakeCache.txt files from your source tree."
      echo ""
      return 1
   fi
fi

mkdir -p "${TARGET_ABS_DIR}"
cd "${TARGET_ABS_DIR}"

cmake "${SRC_ABS_DIR}"
make

cd ${INVOCATION_ABS_DIR}
