///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Core Component. 
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
// MOOSDBVar.cpp: implementation of the CMOOSDBVar class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSDBVar.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSDBVar::CMOOSDBVar()
{
    m_dfVal = -1.0;
    m_dfTime = -1.0;
    m_dfWriteFreq = 0.0;
    m_sVal = "";
    m_sWhoChangedMe = "";
    m_nWrittenTo = 0;
    m_dfWrittenTime = -1;
    m_nOverTicks = 0;
}


CMOOSDBVar::CMOOSDBVar(const string & sName)
{
    m_sName = sName;
    m_dfVal = -1.0;
    m_dfTime = -1.0;
    m_sVal = "";
    m_sWhoChangedMe = "";
    m_nWrittenTo = 0;
    m_dfWriteFreq = 0;
    m_dfWrittenTime = -1;
    m_nOverTicks = 0;

}

CMOOSDBVar::~CMOOSDBVar()
{

}

bool CMOOSDBVar::AddSubscriber(string &sClient, double dfPeriod)
{
    CMOOSRegisterInfo Info;

    if(sClient.empty())
    {
       MOOSTrace("failed to add subscription to \"%s\" for a client with empty name \n ",m_sName.c_str());
       return false;
    }
    Info.m_dfPeriod = dfPeriod;
    Info.m_sClientName = sClient;

    m_Subscribers[sClient] = Info;

    MOOSTrace("adding subscription of \"%s\" to \"%s\"\n ",sClient.c_str(),m_sName.c_str());

    return true;

}

void CMOOSDBVar::RemoveSubscriber(string &sWho)
{

    REGISTER_INFO_MAP::iterator p = m_Subscribers.find(sWho);
    if(p!=m_Subscribers.end())
    {
    MOOSTrace("removing \"%s\"'s subscription to \"%s\"\n",sWho.c_str(),m_sName.c_str());
    m_Subscribers.erase(p);
    }
}

bool CMOOSDBVar::Reset()
{
    m_dfTime = -1;
    m_dfVal = -1;
    m_sVal = "";
    m_sWhoChangedMe = "";
    m_Writers.clear();
    m_dfWrittenTime = -1;
    m_nWrittenTo = 0;
    m_dfWriteFreq = 0;

    return true;
}
