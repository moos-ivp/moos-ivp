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
// LimitBox.cpp: implementation of the CLimitBox class.
//
//////////////////////////////////////////////////////////////////////

#include "LimitBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define DEFAULT_BOX_X  500
#define DEFAULT_BOX_Y  500
#define DEFAULT_BOX_DEPTH  20
#define DEFAULT_IGNORE_TIME 10.0
#define ON_BOX_EXCEEDED_ELEVATOR MOOSDeg2Rad(-30.00)

CLimitBox::CLimitBox()
{
    m_dfXMax = DEFAULT_BOX_X;
    m_dfXMin = -DEFAULT_BOX_X;
    m_dfYMax = DEFAULT_BOX_Y;
    m_dfYMin = -DEFAULT_BOX_Y;
    m_dfDepthMax = DEFAULT_BOX_DEPTH;
    m_dfIgnoreTime = DEFAULT_IGNORE_TIME;
    m_bWatching = false;
    m_dfWatchStart = 0;

    m_nPriority = 0;
    m_bDoneVerbalNotify = false;

}

CLimitBox::~CLimitBox()
{

}

bool CLimitBox::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);

    if(!CMOOSBehaviour::SetParam(sParam,sVal))
    {
        //alwasy make it very important
        m_nPriority = 0;


        //this is for us...
        if(MOOSStrCmp(sParam,"XMIN"))
        {
            m_dfXMin=atof(sVal.c_str());
        }
        else if(MOOSStrCmp(sParam,"XMAX"))
        {
            m_dfXMax=atof(sVal.c_str());
        }
        else if(MOOSStrCmp(sParam,"YMIN"))
        {
            m_dfYMin=atof(sVal.c_str());
        }
        else if(MOOSStrCmp(sParam,"YMAX"))
        {
            m_dfYMax=atof(sVal.c_str());
        }
        else if(MOOSStrCmp(sParam,"DEPTHMAX"))
        {
            m_dfDepthMax=atof(sVal.c_str());
        }
        else if(MOOSStrCmp(sParam,"IGNORETIME"))
        {
            m_dfIgnoreTime=atof(sVal.c_str());
        }
        else
        {
            MOOSTrace("CLimitBox::unknown param %s",sParam.c_str());
            return false;
        }
    }    
    return true;
}

//returns false if we haven't received data in a while..bad news!
bool CLimitBox::RegularMailDelivery(double dfTimeNow)
{
 
    return (!m_DepthDOF.IsStale(dfTimeNow,GetStartTime())&&
            !m_XDOF.IsStale(dfTimeNow,GetStartTime()) &&
            !m_YDOF.IsStale(dfTimeNow,GetStartTime()));
    
}


bool CLimitBox::Run(CPathAction &DesiredAction)
{
    if(m_DepthDOF.IsValid())
    {
        if(m_DepthDOF.IsValid() && m_DepthDOF.GetCurrent()>m_dfDepthMax  ||
           m_XDOF.IsValid()     && m_XDOF.GetCurrent()>m_dfXMax  ||
           m_XDOF.IsValid()     && m_XDOF.GetCurrent()<m_dfXMin  ||
           m_YDOF.IsValid()     && m_YDOF.GetCurrent()>m_dfYMax  ||
           m_YDOF.IsValid()     && m_YDOF.GetCurrent()<m_dfYMin)
        {

            if(m_bWatching == false)
            {
                m_bWatching = true;
                m_dfWatchStart = MOOSTime();
            }
            else
            {
                if(MOOSTime()-m_dfWatchStart>m_dfIgnoreTime)
                {
                    DesiredAction.Set(  ACTUATOR_ELEVATOR,
                                        ON_BOX_EXCEEDED_ELEVATOR,
                                        m_nPriority,
                                        "Bounding Box");

                    DesiredAction.Set(  ACTUATOR_THRUST,
                                        0,
                                        m_nPriority,
                                        "Bounding Box");

                    DesiredAction.Set(  ACTUATOR_RUDDER,
                                        0,
                                        m_nPriority,
                                        "Bounding Box");

                    //we may have some flags to set...
                    string sTxt = MOOSFormat("Box Limits exceeded for more than %f seconds",m_dfIgnoreTime);
                    OnEvent(sTxt,!m_bDoneVerbalNotify);
                    m_bDoneVerbalNotify = true;
                }
            }
            
        }
        else
        {
            m_bDoneVerbalNotify = false;
            m_bWatching = false;
        }
    }
    return true;
}


bool CLimitBox::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    if(PeekMail(NewMail,"NAV_X",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_XDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }
    if(PeekMail(NewMail,"NAV_Y",Msg))
    {
        if(!Msg.IsSkewed(GetTimeNow()))
        {
            m_YDOF.SetCurrent(Msg.m_dfVal,Msg.m_dfTime);
        }
    }
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

bool CLimitBox::GetRegistrations(STRING_LIST &List)
{
    List.push_front("NAV_DEPTH");
    List.push_front("NAV_X");
    List.push_front("NAV_Y");


    //always call base class version
    CMOOSBehaviour::GetRegistrations(List);

    return true;
}

