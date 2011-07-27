#!/bin/sh

hello=123

nsplug meta_vehicle2.bhv ./kilo.bhv -f \
     RETURN_PT=0,0 FOO=$hello BAR=$hello

