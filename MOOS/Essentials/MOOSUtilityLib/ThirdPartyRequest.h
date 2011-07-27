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
// ThirdPartyRequest.h: interface for the CThirdPartyRequest class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
    #pragma warning(disable : 4503)
#endif

#if !defined(AFX_THIRDPARTYREQUEST_H__55C4EEE9_531A_42F4_9483_D7D403C19849__INCLUDED_)
#define AFX_THIRDPARTYREQUEST_H__55C4EEE9_531A_42F4_9483_D7D403C19849__INCLUDED_


#include <map>
#include <string>
#include <list>
//using namespace std;

typedef std::list<std::string> STRING_LIST;
typedef std::map<std::string,STRING_LIST> STRING_LIST_MAP;


class CThirdPartyRequest  
{
public:
    bool SetTaskInfo(std::string sTaskType);
    std::string Close();
    void SetClientInfo(std::string sJob, std::string sClient);
    static bool MOOSThirdPartyClear(CThirdPartyRequest &Request);
    static std::string MOOSThirdPartyExecute(CThirdPartyRequest &Request);
    bool ClearProperties();
    std::string ExecuteRequest();
    void AddProperty(std::string sTok, std::string sVal);
    static void MOOSThirdPartyFormat(CThirdPartyRequest &Request, std::string sTok, std::string sVal);
    CThirdPartyRequest();
    virtual ~CThirdPartyRequest();
protected:
    std::string m_sCol;
    std::string m_sClientInfo;
    std::string m_sAt;
    std::string m_sEq;
    std::string m_sDelim;
    std::string m_sTaskType;


    STRING_LIST_MAP m_Properties;
};

#endif // !defined(AFX_THIRDPARTYREQUEST_H__55C4EEE9_531A_42F4_9483_D7D403C19849__INCLUDED_)
