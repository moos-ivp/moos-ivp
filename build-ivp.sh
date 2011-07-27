#!/bin/sh

INVOCATION_ABS_DIR="`pwd`"

./configure-ivp.sh Debug

cd build

echo ""
echo "Invoking make..."
echo ""

make $@

cd ${INVOCATION_ABS_DIR}
