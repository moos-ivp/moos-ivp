#!/bin/sh

nsplug meta_vehicle.moos ../charlie.moos \
    VID=1 VNAME=charlie VNAME_OTHER=unicorn VEHI_TYPE=KAYAK  \
    SIM_START_POS=10,0,180,0

nsplug meta_vehicle.moos ../unicorn.moos \
    VID=2 VNAME=unicorn VNAME_OTHER=charlie VEHI_TYPE=AUV    \
    SIM_START_POS=-10,0,180,0

nsplug meta_shoreside.moos ../shoreside.moos


