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
// MOOSActuationDriver.cpp: implementation of the CMOOSActuationDriver class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSActuationDriver.h"
#define TELEGRAM_PAUSE 0.5
#include <iostream>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSActuationDriver::CMOOSActuationDriver()
{
    m_bVerbose =false;
    m_dfRPM = 0;
    m_dfRudderOffset=0;
    m_dfElevatorOffset=0;

}

CMOOSActuationDriver::~CMOOSActuationDriver()
{
    
}

bool CMOOSActuationDriver::SetZeroElevator()
{
    return false;
}

bool CMOOSActuationDriver::SetZeroRudder()
{
    return false;
}

bool CMOOSActuationDriver::SetElevator(double dfAng)
{
    return false;
}

bool CMOOSActuationDriver::SetRudder(double dfAng)
{
    return false;
}

bool CMOOSActuationDriver::SetThrust(double dfPercent)
{
    return false;
}

bool CMOOSActuationDriver::Initialise()
{
    
    return true;
}

bool CMOOSActuationDriver::SetPort(CMOOSSerialPort *pPort)
{
#ifdef _WIN32
    m_pPort =dynamic_cast<CMOOSNTSerialPort*>(pPort);
#else
    m_pPort =dynamic_cast<CMOOSLinuxSerialPort*>(pPort);
#endif

    //if poirt is verboe then so are we!
    m_bVerbose = m_pPort->IsVerbose();

    return m_pPort!=NULL;
    
}

bool CMOOSActuationDriver::SendAndAck(const string & sCmd,string &sReply,bool bWait)
{


    if(m_pPort==NULL)
        return false;
    
    if(m_bVerbose)
    {
        MOOSTrace("Send: %s\n",sCmd.c_str());
    }
    
    m_pPort->Write((char*)sCmd.c_str(),
        sCmd.size());
    
        
    //if we are required to read a reply
    if(bWait)
    {
        if(!m_pPort->GetTelegram(sReply,TELEGRAM_PAUSE))
        {
            MOOSTrace("no terminated reply to \"%s\" from actuation hardware\n",sCmd.c_str());
            return false;
        }
    }
    else
    {
        //wait fort answer an ignore..

        MOOSPause((int)(0.05*TELEGRAM_PAUSE*1000));

        //Simply flush...
        m_pPort->Flush();
    }
    
    if(m_bVerbose)
    {
        if(bWait)
        {
            MOOSTrace("Rx: %s\n",sReply.c_str());
        }
        else
        {
            MOOSTrace("Rx: No wait requested\n");
        }
    }
    
    
    
    return true;
    
}

bool CMOOSActuationDriver::SetRudderOffset(double dfAng)
{
    m_dfRudderOffset  = dfAng;
    return true;
}

bool CMOOSActuationDriver::SetElevatorOffset(double dfAng)
{
    m_dfElevatorOffset = dfAng;
    return true;
}
