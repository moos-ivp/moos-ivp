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


// LimitDepth.cpp: implementation of the CLimitDepth class.
//
//////////////////////////////////////////////////////////////////////

#include "LimitDepth.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define ON_DEPTH_EXCEEDED_ELEVATOR MOOSDeg2Rad(-30.0)
#define DEFAULT_DEPTH_LIMIT 50.0
CLimitDepth::CLimitDepth()
{
    m_DepthDOF.SetDesired(DEFAULT_DEPTH_LIMIT);
    m_nPriority = 0;

    m_bDoneVerbalNotify = false;

}

CLimitDepth::~CLimitDepth()
{

}
//returns false if we haven't received data in a while..bad news!
bool CLimitDepth::RegularMailDelivery(double dfTimeNow)
{
 
    return !m_DepthDOF.IsStale(dfTimeNow,GetStartTime());
    
}


bool CLimitDepth::Run(CPathAction &DesiredAction)
{
    if(m_DepthDOF.IsValid())
    {
        if(m_DepthDOF.GetError()<0)
        {

            DesiredAction.Set(  ACTUATOR_ELEVATOR,
                                ON_DEPTH_EXCEEDED_ELEVATOR,
                                m_nPriority,
                                "Depth Limit");

            //we may have some flags to set...
           
            OnEvent("Depth exceeded",!m_bDoneVerbalNotify);
            m_bDoneVerbalNotify = true;
            
        }
        else
        {
            m_bDoneVerbalNotify = false;
        }
    }
    return true;
}

bool CLimitDepth::OnNewMail(MOOSMSG_LIST &NewMail)
{

    CMOOSMsg Msg;
    if(PeekMail(NewMail,"NAV_DEPTH",Msg))
    {
         if(!Msg.IsSkewed(GetTimeNow()))
         {
            m_DepthDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
         }
    }


    //always call base class version
    CMOOSBehaviour::OnNewMail(NewMail);


    return true;
}

bool CLimitDepth::GetRegistrations(STRING_LIST &List)
{
    List.push_front("NAV_DEPTH");


    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}

bool CLimitDepth::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //this is for us...
        if(sParam=="MAXIMUMDEPTH")
        {
            double dfMaxDepth=atof(sVal.c_str());

            m_DepthDOF.SetDesired(dfMaxDepth);

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
