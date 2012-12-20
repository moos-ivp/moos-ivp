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
// DBImage.cpp: implementation of the CDBImage class.



//


//////////////////////////////////////////////////////////////////////

#include "DBImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool CDBImage::Set(MOOSMSG_LIST & InMail)
{
    MOOSMSG_LIST::iterator p;


    m_Lock.Lock();
    //MOOSTrace("Rx'd %d \n",InMail.size());

    for(p = InMail.begin();p!=InMail.end();p++)
    {
        CMOOSMsg & rMsg= *p;
        //MOOSTrace("Rx'd %s \n",rMsg.m_sKey.c_str());

        if(!m_Mask.empty() && m_Mask.find(rMsg.GetSource())!=m_Mask.end())
        {
            continue;
        }

        if(rMsg.IsDataType(MOOS_NOT_SET) && !m_bShowPending)
        {
            continue;
        }

        int nNdx = GetIndex(rMsg.m_sKey);        
        if(nNdx<0)
        {
            m_IndexMap[rMsg.m_sKey] = m_DBData.size();
            m_DBData.push_back(CVar(rMsg));
        }
        else
        {
            bool bChanging = !( rMsg.GetTime()==m_DBData[nNdx].GetTimeVal());
            m_DBData[nNdx]= CVar(rMsg);
            m_DBData[nNdx].Changed(bChanging);
        }                
    }
    m_Lock.UnLock();

    return true;

}








int CDBImage::GetIndex(const std::string & sName)
{
    std::map<std::string,int>::iterator q = m_IndexMap.find(sName);    

    if(q!=m_IndexMap.end())
    {
        int n = q->second; 
        return n;
    }
    else
    {
        return -1;
    }

}

bool CDBImage:: HasChanged(int n)
{
    if(n >= static_cast<int> (m_DBData.size()) || n<0)
        return false;
    return m_DBData[n].IsChanged();
}


bool CDBImage::Get(CVar & rVar, int n)
{
    m_Lock.Lock();

    if(n>=static_cast<int> (m_DBData.size()) || n<0)
    {
        m_Lock.UnLock();
        return false;
    }

    rVar = m_DBData[n];

    m_Lock.UnLock();

    return true;    
}


bool CDBImage::SetProcInfo(MOOSMSG_LIST & InMail) 
{
    m_Lock.Lock();
    // TODO: Add your control notification handler code here
    m_nClients = InMail.size();
    m_Processes.clear();
    MOOSMSG_LIST::iterator p;
    for(p=InMail.begin();p!=InMail.end();p++)
    {

        CMOOSMsg & rMsg = *p;
        std::string sTmp =  rMsg.m_sVal;
        std::string sWho = MOOSChomp(sTmp,":");

        if(m_Processes.find(sWho)==m_Processes.end())
        {
            ProcInfo Info;
            m_Processes[sWho] = Info;
        }

        std::string sSubscribed,sPublished;
        sSubscribed = MOOSChomp(sTmp,"PUBLISHED=");
        sPublished = sTmp;
        MOOSChomp(sSubscribed,"SUBSCRIBED=");

        while(!sSubscribed.empty())
        {
            sTmp = MOOSChomp(sSubscribed,",");
            if(!sTmp.empty())
            {
                m_Processes[sWho].m_Subscribed.push_front(sTmp);
            }
        }

        while(!sPublished.empty())
        {
            sTmp = MOOSChomp(sPublished,",");
            if(!sTmp.empty())
            {
                m_Processes[sWho].m_Published.push_front(sTmp);
            }
        }
    }
    m_Lock.UnLock();
    return true;
}

bool CDBImage::GetProcInfo(const std::string & sWhat,STRING_LIST & sSubs,STRING_LIST & sPubs)
{
    bool bRes = false;
    m_Lock.Lock();
    PROC_INFO_MAP::iterator q = m_Processes.find(sWhat);
    if(q!=m_Processes.end())
    {
        sSubs = q->second.m_Subscribed;
        sPubs = q->second.m_Published;
        bRes = true;
    }
    m_Lock.UnLock();
    return bRes;

}

bool CDBImage::GetProcesses(STRING_LIST & sProcs)
{
    m_Lock.Lock();

    sProcs.clear();    

    PROC_INFO_MAP::iterator q;
    for(q=m_Processes.begin();q!=m_Processes.end();q++)
    {
        sProcs.push_back(q->first.c_str());    
    }
    m_Lock.UnLock();

    return true;
}


bool CDBImage::SetMask(std::set<std::string> Mask)
{
    m_Mask = Mask;
    return true;
}

bool CDBImage::Clear()
{
    m_Lock.Lock();
    m_DBData.clear();
    m_IndexMap.clear();
    m_Lock.UnLock();

    return true;
}
