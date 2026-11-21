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
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
// MOOSDBVar.h: interface for the CMOOSDBVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSDBVAR_H__EAAB2A16_66EF_49E4_9584_51403C59150D__INCLUDED_)
#define AFX_MOOSDBVAR_H__EAAB2A16_66EF_49E4_9584_51403C59150D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
#include <map>
#include <set>

using namespace std;

typedef set<string> STRING_SET;

#include "MOOSRegisterInfo.h"


typedef map<string,CMOOSRegisterInfo> REGISTER_INFO_MAP;


class CMOOSDBVar  
{
public:
    CMOOSDBVar(const string & sName);
    CMOOSDBVar();
    virtual ~CMOOSDBVar();


    bool Reset();
    void RemoveSubscriber(string & sWho);
    bool AddSubscriber(const string & sClient, double dfPeriod);
    bool HasSubscriber(const string & sClient);
    bool GetUpdatePeriod(const string & sClient, double & dfPeriod);

    char   m_cDataType;
    string m_sName;
    double m_dfTime;
    double m_dfVal;
    double m_dfWriteFreq;
    double m_dfWrittenTime;
    string m_sVal;
    string m_sWhoChangedMe;
    string m_sSrcAux;
    string m_sOriginatingCommunity;

    struct Stats {
        Stats() : m_dfLastStatsTime(-1.0), m_nLastStatsWrites(0) {}
        double m_dfLastStatsTime;
        int m_nLastStatsWrites;
    } m_Stats;

    // number of times written to
    int     m_nWrittenTo;


    REGISTER_INFO_MAP m_Subscribers;
    STRING_SET m_Writers;

};

#endif // !defined(AFX_MOOSDBVAR_H__EAAB2A16_66EF_49E4_9584_51403C59150D__INCLUDED_)
