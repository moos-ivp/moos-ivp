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
// ThirdPartyRequest.cpp: implementation of the CThirdPartyRequest class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>
#include "ThirdPartyRequest.h"

#include <map>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThirdPartyRequest::CThirdPartyRequest()
{
    m_sDelim    = "|";
    m_sEq        = "=";
    m_sAt        = "@";
    m_sCol        = ":";
    m_sClientInfo = "JOB@CLIENT";
}

CThirdPartyRequest::~CThirdPartyRequest()
{

}

void CThirdPartyRequest::MOOSThirdPartyFormat(CThirdPartyRequest &Request, string sTok, string sVal)
{
    Request.AddProperty(sTok,sVal);

}

void CThirdPartyRequest::AddProperty(string sTok, string sVal)
{
    MOOSToUpper(sTok);
    MOOSToUpper(sVal);
    
    if(m_Properties.find(sTok)==m_Properties.end())
    {
        STRING_LIST NewList;
        m_Properties[sTok] = (NewList);
    }

    m_Properties[sTok].push_back(sVal);
}

string CThirdPartyRequest::ExecuteRequest()
{
    string sFormattedThirdParty;

    STRING_LIST_MAP::iterator p;
    for(p = m_Properties.begin(); p != m_Properties.end(); p++)
    {
        STRING_LIST & rList = p->second;
        STRING_LIST::iterator q;
        for(q = rList.begin();q!=rList.end();q++)
        {
            string sVal = *q;
            string sValuePair = p->first + m_sEq + sVal;
            sFormattedThirdParty += sValuePair + m_sDelim;    
        }
    }

    return (m_sClientInfo + m_sCol + "TASK=" + m_sTaskType + m_sDelim + sFormattedThirdParty);
}

bool CThirdPartyRequest::ClearProperties()
{
    m_Properties.clear();
    return true;
}

string CThirdPartyRequest::MOOSThirdPartyExecute(CThirdPartyRequest &Request)
{
    return Request.ExecuteRequest();
}

bool CThirdPartyRequest::MOOSThirdPartyClear(CThirdPartyRequest &Request)
{
    return Request.ClearProperties();
}

void CThirdPartyRequest::SetClientInfo(string sJob, string sClient)
{
    m_sClientInfo = sJob + m_sAt + sClient;
}

string CThirdPartyRequest::Close()
{
    return m_sClientInfo + m_sCol + "CLOSE";
}

bool CThirdPartyRequest::SetTaskInfo(string sTaskType)
{
    m_sTaskType = sTaskType;
    return true;
}
