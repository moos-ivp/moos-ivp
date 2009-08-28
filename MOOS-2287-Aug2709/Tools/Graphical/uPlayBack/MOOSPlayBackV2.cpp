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
// MOOSPlayBack.cpp: implementation of the CMOOSPlayBackV2 class.
//
//////////////////////////////////////////////////////////////////////
#if (_MSC_VER == 1200)
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)
#endif

#include "MOOSPlayBackV2.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define MAX_CHOKE_TIME 2.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSPlayBackV2::CMOOSPlayBackV2()
{
    m_dfLastMessageTime = 0;
    m_nCurrentLine = 0;
    m_dfTickTime = 0.01;
    m_dfLastClientProcessedTime = -1;
    m_bWaitingForClientCatchup = false;
}

CMOOSPlayBackV2::~CMOOSPlayBackV2()
{

}

bool CMOOSPlayBackV2::Initialise(const string &sFileName)
{

    if(m_ALog.IsOpen())
        m_ALog.Close();


    m_nCurrentLine = 0;
    m_dfLastClientProcessedTime = -1;
    m_sFileName = sFileName;

    m_ALog.Open(sFileName);
    
    return m_ALog.IsOpen();

}

bool CMOOSPlayBackV2::IsOpen()
{
    return m_ALog.IsOpen();
}


int CMOOSPlayBackV2::GetSize()
{
    return m_ALog.GetLineCount();
}

double CMOOSPlayBackV2::GetTimeNow()
{
    return m_dfLastMessageTime-m_dfLogStart;
}

int CMOOSPlayBackV2::GetCurrentLine()
{
    return m_nLastLine;
}

double CMOOSPlayBackV2::GetStartTime()
{
    if(!m_ALog.IsOpen() || m_ALog.GetLineCount()<0)
        return 0;

    return m_ALog.GetEntryTime(0);

}

double CMOOSPlayBackV2::GetFinishTime()
{
    if(!m_ALog.IsOpen() || m_ALog.GetLineCount()<0)
        return 0;
    
    return m_ALog.GetEntryTime(GetSize()-1);

}

bool CMOOSPlayBackV2::IsEOF()
{
    if(!m_ALog.IsOpen())
        return true;

    return m_nCurrentLine >= m_ALog.GetLineCount();
}


bool CMOOSPlayBackV2::Iterate(MOOSMSG_LIST &Output)
{
    if(IsEOF())
        return false;

    double dfStartTime = m_dfLastMessageTime+1e-6;
    double dfStopTime = m_dfLastMessageTime+m_dfTickTime;
    
    bool bDone = false;
    
    while(!bDone && !IsEOF() )    
    {
        CMOOSMsg NewMsg;
        double dfTNext = m_ALog.GetEntryTime(m_nCurrentLine);


        //are we in a mode in which we are slaved to a client
        //via its publishing of MOOS_CHOKE?
        m_dfClientLagTime = MOOSTime() - m_dfLastClientProcessedTime;
        if (m_dfLastClientProcessedTime != -1 &&
             m_dfClientLagTime > MAX_CHOKE_TIME)
        {
            m_bWaitingForClientCatchup = true;    
            bDone = true;
            dfStopTime = dfTNext;
            continue;
        } 
        else
        {
            //normal sequential processing under our own steam
            m_bWaitingForClientCatchup = false;

            if(dfTNext<=dfStopTime)
            {
                
                if(MessageFromLine(m_ALog.GetLine(m_nCurrentLine),NewMsg))
                {
                    if(!IsFiltered(NewMsg.GetSource()))
                    {
                        Output.push_front(NewMsg);
                    }
                }                             
				
				// arh moved this out of the loop above, because a failed
				// call to MessageFromLine would make uPlayback hang in
				// an infinite loop
				m_nCurrentLine++;
            }
            else
            {
                bDone = true;
            }

        }                         
    }

    m_dfLastMessageTime = dfStopTime;



    return true;
}


bool CMOOSPlayBackV2::Reset()
{
    if(!m_ALog.IsOpen())
        return false;
    
    m_nCurrentLine = 0;
    m_dfLastMessageTime = m_ALog.GetEntryTime(0)-1e-6;
    
    return true;
}

bool CMOOSPlayBackV2::SetTickInterval(double dfInterval)
{
    m_dfTickTime = dfInterval;
    return true;
}

bool CMOOSPlayBackV2::IsFiltered(const std::string & sSrc)
{
    return m_SourceFilter.find(sSrc)!=m_SourceFilter.end();
}

bool CMOOSPlayBackV2::Filter(const std::string & sSrc, bool bWanted)
{
    if(bWanted)
    {
        //remove from our filter things that are wanted
        STRING_SET::iterator p = m_SourceFilter.find(sSrc);

        if(p!=m_SourceFilter.end())
        {
            m_SourceFilter.erase(p);
        }
    }
    else
    {
        //we don;t want this type of message (from thsinclient)
        //so add it to our filter
        m_SourceFilter.insert(sSrc);
        MOOSTrace("Filtering messages from %s\n",sSrc.c_str());
    }
    return true;
}

bool CMOOSPlayBackV2::ClearFilter()
{
    m_SourceFilter.clear();
    return true;
}

bool CMOOSPlayBackV2::MessageFromLine(const std::string & sLine, CMOOSMsg &Msg)
{
    int n = 0;
    std::string sTime,sKey,sSrc;
    
    m_ALog.GetNextToken(sLine,n,sTime);
    m_ALog.GetNextToken(sLine,n,sKey);
    m_ALog.GetNextToken(sLine,n,sSrc);

    int nData = sLine.find_first_not_of(" \t",n);
    if(nData==string::npos)
        return false;

    std::string sData = sLine.substr(nData,sLine.size()-nData);


    Msg.m_dfTime = MOOSTime();

    if(MOOSIsNumeric(sData))
    {
        Msg.m_dfVal = atof(sData.c_str());
        Msg.m_cDataType  = MOOS_DOUBLE;
        Msg.m_sVal = "";
    }
    else
    {
        Msg.m_dfVal = 0.0;
        Msg.m_cDataType  = MOOS_STRING;
        Msg.m_sVal = sData;
    }
    Msg.m_sSrc = sSrc;
    Msg.m_sKey = sKey;
    Msg.m_cMsgType = MOOS_NOTIFY;


    return true;
}

double CMOOSPlayBackV2::GetLastMessageTime()
{
    return m_dfLastMessageTime;
}

bool CMOOSPlayBackV2::SetLastTimeProcessed(double dfTime)
{
    m_dfLastClientProcessedTime = dfTime;
    double diff = MOOSTime() - dfTime;
    return true;
}

bool CMOOSPlayBackV2::IsWaitingForClient()
{
    return m_bWaitingForClientCatchup;
}

string CMOOSPlayBackV2::GetStatusString()
{
    return MOOSFormat("%s Client Lag %g",
        m_bWaitingForClientCatchup ? "Waiting for Client CATCHUP":"Playing just fine", 
        m_dfClientLagTime);
}

bool CMOOSPlayBackV2::GotoTime(double dfT)
{
    int n =  m_ALog.SeekToFindTime(dfT);

    if(n!=-1)
    {
        m_nCurrentLine = n;
        m_dfLastClientProcessedTime = -1;
        m_bWaitingForClientCatchup = false;
        m_dfLastMessageTime = m_ALog.GetEntryTime(m_nCurrentLine)-1e-6;    
        return true;
    }
    else
    {
        return false;
    }
}
