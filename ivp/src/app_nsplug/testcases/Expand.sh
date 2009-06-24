#!/bin/sh

hello=123

nsplug meta_vehicle.bhv ./kilo.bhv -f -p ~/Research \
     RETURN_PT=0,0 FOO=$hello BAR=$hello

