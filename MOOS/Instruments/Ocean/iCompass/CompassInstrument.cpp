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
// CompassInstrument.cpp: implementation of the CCompassInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>


#include <iostream>
using namespace std;
#include "CompassInstrument.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompassInstrument::CCompassInstrument()
{
    m_dfMagneticOffset = 0;
}

CCompassInstrument::~CCompassInstrument()
{

}


/////////////////////////////////////////////
///this is where it all happens..
bool CCompassInstrument::Iterate()
{
    if(GetData())
    {
        PublishData();
    }

    return true;
}

////////////////////////////////////////////////////////////
// tell the world
bool CCompassInstrument::PublishData()
{
    return PublishFreshMOOSVariables();
    
}


bool CCompassInstrument::OnStartUp()
{
    CMOOSInstrument::OnStartUp();
    
    //here we make the variables that we are managing
    double dfHeadingPeriod = 0.5;

    //Compass update @ 2Hz
    AddMOOSVariable("Heading",  "SIM_HEADING",  "COMPASS_HEADING",  dfHeadingPeriod);
    AddMOOSVariable("Yaw",      "",             "COMPASS_YAW",      dfHeadingPeriod);
    AddMOOSVariable("Raw",      "",             "COMPASS_RAW",      dfHeadingPeriod);

    GetMagneticOffset();

    if(IsSimulateMode())
    {
        //not much to do...
        RegisterMOOSVariables();
    }
    else
    {
        //try to open 
        if(!SetupPort())
        {
            return false;
        }
            
        //try 10 times to initialise sensor
        if(!InitialiseSensorN(10,"COMPASS"))
        {
            return false;
        }          
    }


    return true;
}



bool CCompassInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{

    CMOOSMsg Msg;

    if(m_Comms.PeekMail(NewMail,"SIM_HEADING",Msg,true))
    {
        return UpdateWithMagneticDegrees(Msg.m_dfVal);
    }
    else
    {
        return UpdateMOOSVariables(NewMail);
    }
    return true;
}




bool CCompassInstrument::OnConnectToServer()
{
    if(IsSimulateMode())
    {
        //not much to do...
        return RegisterMOOSVariables();    
    }
    else
    {

    }
    return true;
}


///////////////////////////////////////////////////////////////////////////
// here we initialise the sensor, giving it start up values
bool CCompassInstrument::InitialiseSensor()
{    
    
    return true;

}

bool CCompassInstrument::GetData()
{

    if(!IsSimulateMode())
    {
        //here we actually access serial ports etc
    
        string sWhat;
        
        double dfWhen;
        
        if(m_Port.IsStreaming())
        {
            if(!m_Port.GetLatest(sWhat,dfWhen))
            {
                return false;
            }        
        }
        else
        {
            if(!m_Port.GetTelegram(sWhat,0.5))
            {
                return false;
            }
        }
        
        if(PublishRaw())
        {
            SetMOOSVar("Raw",sWhat,MOOSTime());
        }

        ParseCompassData(sWhat);
        
        
        
    }
    else
    {
        //in simulated mode there is nothing to do..all data
        //arrives via comms.
    }
    
    return true;
    
}


bool CCompassInstrument::ParseCompassData(string &sWhat)
{
    MOOSChomp(sWhat,"%");
    if(!sWhat.empty())
    {
        double dfAngle = atof(sWhat.c_str());
        dfAngle/=10;

        UpdateWithMagneticDegrees(dfAngle);

    }

    return true;
}



double CCompassInstrument::Magnetic2True(double dfMagnetic)
{
    return dfMagnetic+m_dfMagneticOffset;
}

double CCompassInstrument::True2Yaw(double dfTrueHeading)
{
    return -dfTrueHeading;
}

bool CCompassInstrument::UpdateWithMagneticDegrees(double dfMagDegrees)
{
    //convert to true north
    double dfAngle=Magnetic2True(dfMagDegrees);

    //publish this in degrees
    SetMOOSVar("Heading",dfAngle,MOOSTime());

    //publish in yaw domain
    dfAngle = True2Yaw(dfAngle);

    //and in radians
    dfAngle*=PI/180.0;

    //wrapped...
    dfAngle = MOOS_ANGLE_WRAP(dfAngle);

    //publish..
    SetMOOSVar("Yaw",dfAngle,MOOSTime());

    return true;
}
