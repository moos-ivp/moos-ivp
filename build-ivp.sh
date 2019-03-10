#!/bin/bash

INVOCATION_ABS_DIR="`pwd`"

./configure-ivp.sh Debug

cd build

echo ""
echo "Invoking make..."
echo ""

make -j8 $@

RESULT=$?

cd ${INVOCATION_ABS_DIR}

exit $RESULT
