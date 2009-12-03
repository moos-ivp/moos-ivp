#!/bin/sh

INVOCATION_ABS_DIR="`pwd`"

# We want absolute pathnames, which require a little extra work to get...
SCRIPT_DIR="`dirname ${0}`"
cd "${SCRIPT_DIR}"
SCRIPT_ABS_DIR=`pwd`

SRC_ABS_DIR="${SCRIPT_ABS_DIR}/ivp/src"

################################################################################

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

################################################################################

INTERMEDIATE_FILES_ABS_DIR="${INVOCATION_ABS_DIR}/build"
LIBRARIES_ABS_DIR="${INVOCATION_ABS_DIR}/lib"
PROGRAMS_ABS_DIR="${INVOCATION_ABS_DIR}/bin"

echo ""
echo "Built files will be placed into these directories:"
echo "  Intermediate build files: ${INTERMEDIATE_FILES_ABS_DIR}"
echo "  Libraries:                ${LIBRARIES_ABS_DIR}"
echo "  Programs:                 ${PROGRAMS_ABS_DIR}"
echo ""

mkdir -p "${INTERMEDIATE_FILES_ABS_DIR}"
mkdir -p "${LIBRARIES_ABS_DIR}"
mkdir -p "${PROGRAMS_ABS_DIR}"

cd "${INTERMEDIATE_FILES_ABS_DIR}"

################################################################################

echo "IVP_BUILD_GUI_CODE='"${IVP_BUILD_GUI_CODE}"'"

case "${IVP_BUILD_GUI_CODE}" in
   'ON'|'YES'|'TRUE'|'' )
      CLEANED_IVP_BUILD_GUI_CODE=ON
      ;;
   'OFF'|'NO'|'FALSE' )
      CLEANED_IVP_BUILD_GUI_CODE=OFF
      ;;
   *)
      echo 'Invalid value for the environment variable "IVP_BUILD_GUI_CODE"'
      echo '  If you set it at all, please set it to "ON" or "OFF"'
      return 1
      ;;
esac


echo ""
echo "Environment variable 'IVP_BUILD_GUI_CODE' = '"${IVP_BUILD_GUI_CODE}"'"
echo "   So will invoke cmake with -DIVP_BUILD_GUI_CODE="${CLEANED_IVP_BUILD_GUI_CODE}" argument."

################################################################################

echo ""
echo "The IVP_CMAKE_FLAGS environment variable = '${IVP_CMAKE_FLAGS}'"
echo "   That variable's value(s) will be added as command-line arguments"
echo "   to our invocation of cmake."

################################################################################

echo ""
echo "Invoking cmake..."
echo ""

cmake -DIVP_BUILD_GUI_CODE=${CLEANED_IVP_BUILD_GUI_CODE} \
      -DIVP_LIB_DIRECTORY="${LIBRARIES_ABS_DIR}"         \
      -DIVP_BIN_DIRECTORY="${PROGRAMS_ABS_DIR}"          \
      ${IVP_CMAKE_FLAGS}                                 \
      "${SRC_ABS_DIR}"

echo ""
echo "Invoking make..."
echo ""

make $@

cd ${INVOCATION_ABS_DIR}
