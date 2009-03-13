///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a  MOOS Instrument.
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//   General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//   02111-1307, USA.
//
//////////////////////////    END_GPL    //////////////////////////////////
// MOOSSAILDriver.cpp: implementation of the CMOOSSAILDriver class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
#include "MOOSSAILDriver.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define SAIL_MAX 255
#define SAIL_GB_RATIO 825
#define SAIL_ENCODER_RESOLUTION 12

CMOOSSAILDriver::CMOOSSAILDriver()
{
    MOOSTrace("Creating SAIL Driver\n");
}

CMOOSSAILDriver::~CMOOSSAILDriver()
{

}


bool CMOOSSAILDriver::Initialise()
{


    list<CSailCommand> List;


    if(m_pPort != NULL )
    {




    List.push_back(CSailCommand("#THR:",true));

        //enable hardware kill...
    List.push_back(CSailCommand("#THB:",true));
//        List.push_back(CSailCommand("#THG:",true));

//        List.push_back("#FRR:");
//        List.push_back("#FRO:");
    List.push_back(CSailCommand("#FRG:",false));
//        List.push_back("#FRZ:");
//        List.push_back("#FRL000000");

//        List.push_back("#FER:");
//        List.push_back("#FEO:");
    List.push_back(CSailCommand("#FEG:",false));

    list<CSailCommand>::iterator p;

    for(p = List.begin();p!=List.end();p++)
    {
        string sCmd = p->m_sCmd;

        MOOSTrace("iActuation Init() : Sending %s\n",sCmd.c_str());
        string sReply;
        SendAndAck(sCmd,sReply,p->m_bWaitForReply);

        MOOSPause(100);
    }


    }

    MOOSTrace("Sail Init complete...\n\n\n");
    return true;
}


bool CMOOSSAILDriver::SetElevator(double dfAng)
{
    return DoFinControl("#FEL",dfAng);
}

bool CMOOSSAILDriver::SetRudder(double dfAng)
{

    return DoFinControl("#FRL",dfAng);

}

bool CMOOSSAILDriver::SetThrust(double dfPercent)
{


    int nSign = 1;

    if(dfPercent!=0)
    {
    nSign= (int)(dfPercent/(fabs(dfPercent)));
    }

    dfPercent = fabs(dfPercent);


    //map dfPercent to 127...
    if(dfPercent>100)
    {
    dfPercent = 100.0;
    }

    unsigned int nFull = 0xE0;

    unsigned int nStart = 0x0;

    //look after asymetry...
    if(nSign>0)
    {
        nStart =0xA8;
    }
    else
    {
        nStart = 0xD0;
    }

    //y =mx+x////
    unsigned char nLevel = (unsigned char)((nFull-nStart)/100.0*dfPercent+nStart);

    //but zero is REALLY ZERO!
    if(dfPercent==0)
    {
        nLevel=0;
    }


    unsigned char byLow = nLevel & 0xF;
    unsigned char byHigh = (nLevel>>4) & 0xF;

    const char* cHEX = "0123456789ABCDEF";

    if(m_pPort!=NULL)
    {
    char sCmd[20];

    sprintf(sCmd,"#TH%c%c%c:",
        nSign==1?'F':'V',
        cHEX[byHigh],
        cHEX[byLow]);

    string sReply;

    if(SendAndAck(sCmd,sReply))
    {

        //now here we look for watchdog time out...

        if(sReply.find('M')!=string::npos)
        {
        MOOSTrace("SAIL Bus Monitor timeout detected\nResetting...\n");
        SendAndAck("#THR:",sReply);
        SendAndAck("#THB:",sReply);
        }
    }



    }

    return true;
}


bool CMOOSSAILDriver::SetZeroElevator()
{
    MOOSTrace("Setting home for Elevator\n");
    string sReply;
    return SendAndAck("#FEZ",sReply);
}

bool CMOOSSAILDriver::SetZeroRudder()
{
    MOOSTrace("Setting home for Rudder\n");
    string sReply;
    return SendAndAck("#FRZ",sReply);
}





bool CMOOSSAILDriver::DoFinControl(const char *sAddress, double dfAng)
{
    int nCount =(int)( (dfAng/360.0*SAIL_GB_RATIO)*SAIL_ENCODER_RESOLUTION);

    nCount = ~nCount;
    nCount +=1;

    const char* cHEX = "0123456789ABCDEF";

    if(m_pPort!=NULL)
    {
    char sCmd[20];

    sprintf(sCmd,"%s%c%c%c%c%c%c",
        sAddress,
        cHEX[(nCount>>20)&0xF],
        cHEX[(nCount>>16)&0xF],
        cHEX[(nCount>>12)&0xF],
        cHEX[(nCount>>8)&0xF],
        cHEX[(nCount>>4)&0xF],
        cHEX[(nCount>>0)&0xF]);


    //    sprintf(sCmd,"#FRL000000");


    string sReply;
    return     SendAndAck(sCmd,sReply);




    }
    else
    {
    return false;
    }
}
