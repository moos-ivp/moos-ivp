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
//   This file is part of a  MOOS Utility Component. 
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
// DBImage.h: interface for the CDBImage class.

//

//////////////////////////////////////////////////////////////////////

#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif


#if !defined(AFX_DBIMAGE_H__E4F9EC4D_7049_4EC8_86BA_0AF21BBCA23C__INCLUDED_)

#define AFX_DBIMAGE_H__E4F9EC4D_7049_4EC8_86BA_0AF21BBCA23C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSLIB/MOOSLib.h>
#include <string>
#include <map>
#include <vector>
//dfgdfgljdsfg
//dfgdfglkj


class CDBImage
{
public:
    bool Clear();

    CDBImage()
    {
        m_bShowPending = false;
    }

    struct ProcInfo
    {
        std::string m_sName;
        STRING_LIST m_Published;
        STRING_LIST m_Subscribed;
    };

    typedef std::map<std::string,ProcInfo> PROC_INFO_MAP;

    class CVar

    {
    public:
        CVar()
        {
            m_dfTime = -1;
        }

        CVar(CMOOSMsg & rMsg)
        {
            m_dfTime = rMsg.m_dfTime;
            switch(rMsg.m_cDataType)
            {
            case MOOS_STRING:
                m_eType = STR;
                break;
            case MOOS_DOUBLE:
                m_eType = DBL;
                break;
            default:
                m_eType = UNKNOWN;
            }
            m_sStr = rMsg.GetAsString();
            m_sCommunity = rMsg.m_sOriginatingCommunity;
            m_sName = rMsg.m_sKey;
            m_sFreq = MOOSFormat("%.1f",rMsg.m_dfVal2);
            m_sSource = rMsg.m_sSrc;
            m_bChanged = false;
        }

        std::string GetTime(){return MOOSFormat("%.3f",m_dfTime);};
        double GetTimeVal(){return m_dfTime;};
        std::string GetValue(){return m_eType==UNKNOWN?"":m_sStr;};
        std::string GetCommunity(){return m_sCommunity;};
        std::string GetName(){return m_sName;};
        std::string GetFrequency(){return m_sFreq;};
        std::string GetSource(){return m_sSource;};
        bool IsChanged(){return m_bChanged;};
        void Changed(bool b){m_bChanged = b;};
        std::string GetType()
        {
            switch(m_eType)
            {
            case STR:
                return "$";
            case DBL:
                return "D";
            default:
                return "?";
            }
        }


    protected:
        double m_dfTime;
        std::string m_sStr;
        enum Type{ STR,DBL,UNKNOWN};
        Type m_eType;
        std::string m_sCommunity;
        std::string m_sSrc;
        std::string m_sName;
        std::string m_sFreq;
        std::string m_sSource;
        bool m_bChanged;
    };

    bool Set(MOOSMSG_LIST & InMail);
    bool Get(CVar & rVar, int n);
    bool HasChanged(int n);
    bool SetProcInfo(MOOSMSG_LIST & InMail) ;
    bool GetProcesses(STRING_LIST & sProcs);
    int GetNumVariables(){return m_DBData.size();}
    bool GetProcInfo(const std::string & sWhat,STRING_LIST & sSubs,STRING_LIST & sPubs);
    bool SetMask(std::set<std::string> Mask);
    bool ShowPending(bool b){m_bShowPending = b;return true;};
protected:
    int GetIndex(const std::string & sName);
    std::vector<CVar> m_DBData;
    std::map<std::string,int> m_IndexMap;
    std::set<std::string> m_Mask;

    bool m_bShowPending ;
    int m_nClients;
    int m_nVariables;
    PROC_INFO_MAP m_Processes;
    CMOOSLock m_Lock;
};


#endif // !defined(AFX_DBIMAGE_H__E4F9EC4D_7049_4EC8_86BA_0AF21BBCA23C__INCLUDED_)

