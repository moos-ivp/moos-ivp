/*
 * PeriodicEventTest.cpp
 * a simple test for the MOOS::PeriodicEvent class.
 *  Created on: Apr 1, 2013
 *      Author: pnewman
 */

#include "MOOS/libMOOS/Comms/XPCUdpSocket.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include <iostream>
#include <iomanip>
#include <cmath>



int main(int argc, char * argv[])
{
    XPCUdpSocket discovery(9002);
    discovery.vSetBroadcast(true);
    discovery.vBindSocket();

    char  msg[1024];
    while(1)
    {
        int iRx =discovery.iRecieveMessage(msg,1024);
        if(iRx)
            std::cerr<<std::string(msg)<<std::endl;
    }



}
