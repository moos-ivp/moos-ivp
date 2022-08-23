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
// MOOSPlayBack.cpp: implementation of the CMOOSPlayBack class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "MOOSPlayBack.h"
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

CMOOSPlayBack::CMOOSPlayBack()
{
    m_dfLastMessageTime = 0;
    m_nLastLine = 0;
    m_bEOF = true;
    m_dfTickTime = 0.01;
    m_dfLastClientProcessedTime = -1;
    m_bWaitingForClientCatchup = false;
}

CMOOSPlayBack::~CMOOSPlayBack()
{

}

bool CMOOSPlayBack::Initialise(const string &sFileName)
{
    m_sFileName = sFileName;
    if(!ParseFile())
        return false;

    return true;

}

bool CMOOSPlayBack::IsOpen()
{
    return m_InputFile.is_open();
}

bool CMOOSPlayBack::ParseFile()
{
    
    m_bEOF = false;

    //if open then close
    if(m_InputFile.is_open())
    {
        m_InputFile.close();
        m_InputFile.clear();
    }

    //now open
    m_InputFile.open(m_sFileName.c_str());

    //check
    if(!m_InputFile.is_open())
    {
        //bad news
        return false;
    }

    m_bEOF = false;

    m_Sources.clear();
    m_PlayBackList.clear();

    char Tmp[10000];
        
    string sLogStart;

    int nLine = 0;
    m_dfLogStart = 0.0;
    while(!m_InputFile.eof())
    {
        m_InputFile.getline(Tmp,sizeof(Tmp));

        string sLine = string(Tmp);

        //if we haven't found it look for start of log file...
        if(m_dfLogStart==0.0 && sLine.find("LOGSTART")!=string::npos)
        {
            MOOSChomp(sLine,"LOGSTART");
            m_dfLogStart = atof(sLine.c_str());
            continue;
        }

        MOOSTrimWhiteSpace(sLine);

        //is it a comment? or maybe there is nothing on the line!
        if(sLine[0]=='%' || sLine.size()==0)
            continue;
        
        if(nLine==0)
        {
            sLogStart = sLine;
        }
        else
        {
            CPlaybackEntry NewEntry;
            
            NewEntry.m_dfTime = atof(MOOSChomp(sLine," ").c_str())+m_dfLogStart;
            MOOSTrimWhiteSpace(sLine);
            
            NewEntry.m_sWhat  = MOOSChomp(sLine," ");
            MOOSTrimWhiteSpace(sLine);
            
            NewEntry.m_sWho  = MOOSChomp(sLine," ");
            MOOSTrimWhiteSpace(sLine);

            string sData = sLine;
            MOOSTrimWhiteSpace(sData);
            
            //is this a string or not?
            if(MOOSIsNumeric(sData))
            {
                NewEntry.m_dfVal = atof(sData.c_str());
                NewEntry.m_sVal = "";
                NewEntry.m_bNumeric = true;
            }
            else
            {
                NewEntry.m_sVal = sData;
                NewEntry.m_dfVal = 0;
                NewEntry.m_bNumeric = false;
            }

            m_PlayBackList.push_back(NewEntry);
            //MOOSTrace("size is %d\n",m_PlayBackList.size());

            m_Sources.insert(NewEntry.m_sWho);

        }        

        nLine++;
    


    }

    sort(m_PlayBackList.begin(),m_PlayBackList.end());
    //m_PlayBackList.sort();


    return true;
}

int CMOOSPlayBack::GetSize()
{
    return m_PlayBackList.size();
}

double CMOOSPlayBack::GetTimeNow()
{
    return m_dfLastMessageTime-m_dfLogStart;
}

int CMOOSPlayBack::GetCurrentLine()
{
    return m_nLastLine;
}

double CMOOSPlayBack::GetStartTime()
{
    if(m_PlayBackList.empty())
        return 0;

    return m_PlayBackList.front().m_dfTime;
}

double CMOOSPlayBack::GetFinishTime()
{
    if(m_PlayBackList.empty())
        return 0;

    return m_PlayBackList.back().m_dfTime;
}

bool CMOOSPlayBack::Iterate(MOOSMSG_LIST &Output)
{
    if(m_bEOF)
        return false;
    
    double dfStartTime = m_dfLastMessageTime+1e-6;
    double dfStopTime = m_dfLastMessageTime+m_dfTickTime;
    
    bool bDone = false;

    while(m_pListIterator!=m_PlayBackList.end() && !bDone)
    {    
        CPlaybackEntry & rEntry = *m_pListIterator;

        
        m_dfClientLagTime = MOOSTime() - m_dfLastClientProcessedTime;
        if (m_dfLastClientProcessedTime != -1 &&
             m_dfClientLagTime > MAX_CHOKE_TIME)
        {
            m_bWaitingForClientCatchup = true;    
            bDone = true;
            dfStopTime = rEntry.m_dfTime;
            continue;
        } 
        else
        {
            m_bWaitingForClientCatchup = false;
        }

        if(rEntry.m_dfTime>dfStopTime)
        {
            m_pListIterator;
            bDone = true;
            continue;
        }
        else
        {
            if(rEntry.m_dfTime>=dfStartTime)
            {
                CMOOSMsg Msg;

                Entry2Message(rEntry,Msg);

                if(m_SourceFilter.find(Msg.m_sSrc)==m_SourceFilter.end())
                {
                    Output.push_front(Msg);
                }
            }
            m_nLastLine++;
        }        
        m_pListIterator++;
    }

    //look for end of data..
    if(m_pListIterator==m_PlayBackList.end())
    {
        m_bEOF = true;
        return false;
    }
    
    m_dfLastMessageTime = dfStopTime;

    return true;
}

bool CMOOSPlayBack::IsEOF()
{
    return m_bEOF;
}

bool CMOOSPlayBack::Reset()
{
    if(m_PlayBackList.empty())
        return false;

    //reset important things..
    m_pListIterator = m_PlayBackList.begin();
    m_bEOF = false;
    m_nLastLine = 0;
    m_dfLastMessageTime = m_pListIterator->m_dfTime-1e-6;

    return true;
}

bool CMOOSPlayBack::SetTickInterval(double dfInterval)
{
    m_dfTickTime = dfInterval;
    return true;
}

bool CMOOSPlayBack::Filter(string sSrc, bool bWanted)
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

bool CMOOSPlayBack::ClearFilter()
{
    m_SourceFilter.clear();
    return true;
}

bool CMOOSPlayBack::Entry2Message(CPlaybackEntry Entry, CMOOSMsg &Msg)
{
    if(!CrackAndAdjustEntry(Entry))
        return false;

    Msg.m_dfTime = MOOSTime();
    Msg.m_dfVal = Entry.m_dfVal;
    Msg.m_sVal = Entry.m_sVal;
    Msg.m_sSrc = Entry.m_sWho;
    Msg.m_sKey = Entry.m_sWhat;
    Msg.m_cDataType = Entry.m_bNumeric ? MOOS_DOUBLE : MOOS_STRING;
    Msg.m_cMsgType = MOOS_NOTIFY;

    return true;
}

bool CMOOSPlayBack::CrackAndAdjustEntry(CPlaybackEntry &rEntry)
{
    //return true; //added Jan 2005 PMN - exisiting code was intolerable!
    //client must fix 
    
    //this is horrible but if the original message included timing information
    //then we have to adjust it here....
    /*
    if(rEntry.m_sWhat=="LBL_TOF")
    {
        string sOldStart = MOOSChomp(rEntry.m_sVal,",");
        MOOSChomp(sOldStart,"=");
        double dfOldTxTime = atof(sOldStart.c_str());
        double dfAdjustment = MOOSTime()-rEntry.m_dfTime;
        string sNewStart = MOOSFormat("Tx=%.3f,",dfOldTxTime+dfAdjustment);
        
        rEntry.m_sVal = sNewStart+rEntry.m_sVal;
    }*/

    return true;
}

double CMOOSPlayBack::GetLastMessageTime()
{
    return m_dfLastMessageTime;
}

bool CMOOSPlayBack::SetLastTimeProcessed(double dfTime)
{
    m_dfLastClientProcessedTime = dfTime;
    double diff = MOOSTime() - dfTime;
    return true;
}

bool CMOOSPlayBack::IsWaitingForClient()
{
    return m_bWaitingForClientCatchup;
}

string CMOOSPlayBack::GetStatusString()
{
    return MOOSFormat("%s Client Lag %g",
        m_bWaitingForClientCatchup ? "Waiting for Client CATCHUP":"Playing just fine", 
        m_dfClientLagTime);
}

bool CMOOSPlayBack::GotoTime(double dfT)
{
    int nTmp = 0;

    PLAYBACKLIST::iterator q = m_pListIterator;
    for(q=m_PlayBackList.begin();
        q!=m_PlayBackList.end();
        q++)
    {
        nTmp++;
        if(q->m_dfTime>dfT)
        {
            m_pListIterator=q;
            m_nLastLine = nTmp;
            m_bEOF = false;
            m_dfLastClientProcessedTime = -1;
            m_bWaitingForClientCatchup = false;
            m_dfLastMessageTime = m_pListIterator->m_dfTime-1e-6;    
            return true;
        }
    }
    return false;
}
