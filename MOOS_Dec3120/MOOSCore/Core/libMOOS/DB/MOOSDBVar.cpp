///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of 
//   Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and 
//   the University of Oxford 2003-2013 
//   
//   email: pnewman@robots.ox.ac.uk. 
//              
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Public License
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/gpl.txt
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
// MOOSDBVar.cpp: implementation of the CMOOSDBVar class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include "MOOS/libMOOS/DB/MOOSDBVar.h"
#include <iostream>
#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSDBVar::CMOOSDBVar() :
    m_cDataType(MOOS_NOT_SET),
    m_sName(),
    m_dfTime(-1.0),
    m_dfVal(-1.0),
    m_dfWriteFreq(0.0),
    m_dfWrittenTime(-1.0),
    m_sVal(),
    m_sWhoChangedMe(),
    m_sSrcAux(),
    m_sOriginatingCommunity(),
    m_Stats(),
    m_nWrittenTo(0),
    m_Subscribers(),
    m_Writers()
{}


CMOOSDBVar::CMOOSDBVar(const string & sName) :
    m_cDataType(MOOS_NOT_SET),
    m_sName(sName),
    m_dfTime(-1.0),
    m_dfVal(-1.0),
    m_dfWriteFreq(0.0),
    m_dfWrittenTime(-1.0),
    m_sVal(),
    m_sWhoChangedMe(),
    m_sSrcAux(),
    m_sOriginatingCommunity(),
    m_Stats(),
    m_nWrittenTo(0),
    m_Subscribers(),
    m_Writers()
{}

CMOOSDBVar::~CMOOSDBVar()
{

}

bool CMOOSDBVar::GetUpdatePeriod(const string & sClient, double & dfPeriod){
    if(!HasSubscriber(sClient))
        return false;
    dfPeriod = m_Subscribers[sClient].m_dfPeriod;
    return true;
}

bool CMOOSDBVar::AddSubscriber(const string &sClient, double dfPeriod)
{

    if(sClient.empty())
    {
       MOOSTrace("[X] Failed to add subscription to \"%s\" for a client with empty name \n",m_sName.c_str());
       return false;
    }

    CMOOSRegisterInfo Info;
    Info.m_sClientName = sClient;
    Info.m_dfPeriod = dfPeriod;
    m_Subscribers[sClient] = Info;

    return true;
}

bool CMOOSDBVar::HasSubscriber(const string & sClient)
{
	if(m_Subscribers.empty())
		return false;
	else
		return m_Subscribers.find(sClient)!=m_Subscribers.end();

}


void CMOOSDBVar::RemoveSubscriber(string &sWho)
{

    REGISTER_INFO_MAP::iterator p = m_Subscribers.find(sWho);
    if(p!=m_Subscribers.end())
    {
    //MOOSTrace("MOOSDB: Removing \"%s\"'s subscription to \"%s\"\n",sWho.c_str(),m_sName.c_str());
    	m_Subscribers.erase(p);
    	//MOOSTrace("- subs of \"%s\" to \"%s\" \n",sWho.c_str(),m_sName.c_str());

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
