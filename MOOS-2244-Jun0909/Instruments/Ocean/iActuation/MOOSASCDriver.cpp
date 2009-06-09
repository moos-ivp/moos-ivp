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
// MOOSASCDriver.cpp: implementation of the CMOOSASCDriver class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
#include "MOOSASCDriver.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define ASC_THRUSTER_RANGE 255
#define ASC_THRUSTER_MAX   228
#define ASC_THRUSTER_MIN    28
#define ASC_THRUSTER_ZERO  128

CMOOSASCDriver::CMOOSASCDriver()
{
    MOOSTrace("Creating ASC Driver\n");
}

CMOOSASCDriver::~CMOOSASCDriver()
{

}


bool CMOOSASCDriver::Initialise()
{

    if(m_pPort != NULL )
    {
    m_pPort->SetTermCharacter(0x03);

    STRING_LIST List;

    //Initialization isn't really needed, but it would be nice to
    //see the result of this. The TT8 will spit out several lines of
    //initialization data, which can be turned off as needed.

    List.push_back("#WD\r\n");
    List.push_back("#WD\r\n");
    List.push_front("#WD\r\n");

    STRING_LIST::iterator p;

    for(p = List.begin();p!=List.end();p++)
    {
        string sCmd = *p;

        MOOSTrace("iActuation Init() : Sending %s \n",sCmd.c_str());

        string sReply;
        if(!SendAndAck(sCmd,sReply))
        {
        MOOSTrace("Failed command\n");
        }
        MOOSPause(100);

    }

    }

    return true;
}


bool CMOOSASCDriver::SetElevator(double dfAng)
{
    // Elevator commands ignored
    return false;
}

bool CMOOSASCDriver::SetRudder(double dfAng)
{
    bool   bResult = DoFinControl("#WD RA ",dfAng);
    //this pause stops things happening to fast for the ASC uP.
    MOOSPause(100);

    return bResult;

}

bool CMOOSASCDriver::SetZeroElevator()
{
    // Elevator commands ignored
    return false;
}

bool CMOOSASCDriver::SetZeroRudder()
{
    MOOSTrace("Setting home for Rudder\n");
    string sReply;
    return SendAndAck("#WD RA 0\r\n",sReply);

}

bool CMOOSASCDriver::SetThrust(double dfPercent)
{

    int nThrust;

    if(dfPercent>100)
    {
    dfPercent = 100.0;
    }
    if(dfPercent<-100)
    {
    dfPercent = -100.0;
    }

    nThrust = (int)(dfPercent);
    nThrust += ASC_THRUSTER_ZERO;

    if(m_pPort!=NULL)
    {
    char sCmd[20];

    sprintf(sCmd,"#WD TH %d\r\n",
        nThrust);

    string sReply;
    if(!SendAndAck(sCmd,sReply))
    {
        return false;
    }

    //this pause stops things happening to fast for the ASC uP.
    MOOSPause(100);
    }

    return true;
}


bool CMOOSASCDriver::DoFinControl(const char *sAddress, double dfAng)
{
    int nAngle = (int)(dfAng);

    if(m_pPort!=NULL)
    {
    char sCmd[20];

    sprintf(sCmd,"%s%d\r\n",
        sAddress,
        nAngle);

    string sReply;
    return  SendAndAck(sCmd,sReply);

    }
    else
    {
    return false;
    }
}

