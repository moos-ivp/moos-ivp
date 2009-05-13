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
//   This file is part of a  MOOS Basic (Common) Application. 
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
// SeabedTrack.cpp: implementation of the CSeabedTrack class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif




#include <math.h>
#include <iostream>
using namespace std;

#include "MOOSTaskDefaults.h"
#include "SeabedTrack.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSeabedTrack::CSeabedTrack()
{


    m_nPriority = 3;

    m_AltitudeDOF.SetDesired(6.0);

    m_bInitialised = false;

}

CSeabedTrack::~CSeabedTrack()
{

}

//returns false if we haven't received data in a while..bad news!
bool CSeabedTrack::RegularMailDelivery(double dfTimeNow)
{
   
    return !m_AltitudeDOF.IsStale(dfTimeNow,GetStartTime());
    
}


bool CSeabedTrack::Run(CPathAction &DesiredAction)
{

    if(!m_bInitialised)
    {
        Initialise();
    }

    if(m_AltitudeDOF.IsValid())
    {
        double dfError = m_AltitudeDOF.GetError();
        double dfCmd= 0;

        //this is for logging purposes only
        m_ZPID.SetGoal(m_AltitudeDOF.GetDesired());

        if(m_ZPID.Run(  dfError,
                        m_AltitudeDOF.GetErrorTime(),
                        dfCmd,
                        m_PitchDOF.GetCurrent(),
                        m_PitchDOF.GetErrorTime()))
        {
            //OK we need to change something
            DesiredAction.Set(  ACTUATOR_ELEVATOR,
                                dfCmd,
                                m_nPriority,
                                "Seabed Track");

        }
    }
    return true;
}

bool CSeabedTrack::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;

    if(PeekMail(NewMail,"NAV_ALTITUDE",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_AltitudeDOF.SetCurrent(Msg.m_dfVal, Msg.m_dfTime);  
        }
    }

    if(PeekMail(NewMail,"NAV_PITCH",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_PitchDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);

    return true;
}

bool CSeabedTrack::GetNotifications(MOOSMSG_LIST & List)
{
    if(m_bActive)
    {
        CMOOSMsg Msg(MOOS_NOTIFY,
             "DESIRED_PITCH",
            m_ZPID.GetPitchDesired());

        List.push_back(Msg);
    }

    return CMOOSBehaviour::GetNotifications(List);
}



bool CSeabedTrack::GetRegistrations(STRING_LIST &List)
{

    List.push_front("NAV_ALTITUDE");
    List.push_front("NAV_PITCH");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}


bool CSeabedTrack::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(sParam=="ALTITUDE")
        {
            double dfDesiredAltitude=atof(sVal.c_str());
            m_AltitudeDOF.SetDesired(dfDesiredAltitude);
        }
        else
        {
            //hmmm - it wasn't for us at all: base class didn't understand either
            MOOSTrace("Param \"%s\" not understood!\n",sParam.c_str());
            return false;
        }
    }

    return true;

}

bool CSeabedTrack::Initialise()
{
    //we control altitude NOT depth
    m_ZPID.SetAsDepthController(false);


    m_bInitialised = true;

    return true;
}
