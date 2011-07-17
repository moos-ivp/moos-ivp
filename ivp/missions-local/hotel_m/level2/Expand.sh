#!/bin/sh

splug meta_vehicle.moos ../alpha.moos    \
    VID=1 VNAME=alpha                    \
    SIM_START_POS=-10,-25,180,0

splug meta_vehicle.moos ../bravo.moos    \
    VID=2 VNAME=bravo                    \
    SIM_START_POS=10,-15,180,0

splug meta_ShoreSide.moos ../ShoreSide.moos 

