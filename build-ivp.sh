#!/bin/bash

INVOCATION_ABS_DIR="`pwd`"
CONFIG="Release"
CLEAN="no"
CMD_ARGS=""

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        printf "%s [SWITCHES]  \n" $0
        printf "  --help, -h            \n"
        printf "  --debug,   -d         \n"
        printf "  --release, -r         \n"
        printf "  clean                 \n"
        exit 0;
    elif [ "${ARGI}" = "--debug" -o "${ARGI}" = "-d" ] ; then
        CONFIG="Debug"
    elif [ "${ARGI}" = "--release" -o "${ARGI}" = "-r" ] ; then
        CONFIG="Release"
    elif [ "${ARGI}" = "clean" ] ; then
        CLEAN="yes"
    else
	CMD_ARGS=$CMD_ARGS" "$ARGI
    fi
done

./configure-ivp.sh ${CONFIG}

cd build

echo ""
echo "Invoking make..."
echo ""

RESULT=0
if [ "${CLEAN}" = "yes" ] ; then
    make clean
    RESULT=$?
else
    make -j12 ${CMD_ARGS}
    RESULT=$?
fi

cd ${INVOCATION_ABS_DIR}

exit $RESULT
