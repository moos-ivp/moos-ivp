#!/bin/sh

INVOCATION_ABS_DIR="`pwd`"

./configure-ivp.sh Debug

cd build

echo ""
echo "Invoking make..."
echo ""

make -j8 $@

cd ${INVOCATION_ABS_DIR}
