#!/bin/bash

INVOCATION_ABS_DIR="`pwd`"

cd proj-4.8.0
./configure
make
make install

cd ${INVOCATION_ABS_DIR}
