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
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

// LimitAltitude.cpp: implementation of the CLimitAltitude class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
using namespace std;
#include "LimitAltitude.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define ON_ALTITUDE_EXCEEDED_ELEVATOR MOOSDeg2Rad(-30.00)

CLimitAltitude::CLimitAltitude()
{
       m_nPriority = 0;
       m_bDoneVerbalNotify = false;
}

CLimitAltitude::~CLimitAltitude()
{
    
}
//returns false if we haven't received data in a while..bad news!
bool CLimitAltitude::RegularMailDelivery(double dfTimeNow)
{
   
    return !m_AltitudeDOF.IsStale(dfTimeNow,GetStartTime());
}

bool CLimitAltitude::Run(CPathAction &DesiredAction)
{
    if(m_AltitudeDOF.IsValid())
    {
        if(m_AltitudeDOF.GetError()>0)
        {
            
            DesiredAction.Set(  ACTUATOR_ELEVATOR,
                                ON_ALTITUDE_EXCEEDED_ELEVATOR,
                                m_nPriority,
                                "Altitude Limit");

            //we may have some flags to set...
            OnEvent("LimitAltitude::Below Altitude",!m_bDoneVerbalNotify);
            m_bDoneVerbalNotify = true;
            
        }
        else
        {
            m_bDoneVerbalNotify = false;
        }
    }
    return true;
}

bool CLimitAltitude::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    if(PeekMail(NewMail,"NAV_ALTITUDE",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_AltitudeDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }

    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);


    return true;
}


bool CLimitAltitude::GetRegistrations(STRING_LIST &List)
{
    List.push_front("NAV_ALTITUDE");

    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);


    return true;
}

bool CLimitAltitude::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(sParam=="MINIMUMALTITUDE")
        {
     
            double dfMinAltitude=atof(sVal.c_str());
            m_AltitudeDOF.SetDesired(dfMinAltitude);
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
