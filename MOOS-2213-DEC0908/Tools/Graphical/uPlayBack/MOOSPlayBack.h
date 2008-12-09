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
// MOOSPlayBack.h: interface for the CMOOSPlayBack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSPLAYBACK_H__326FF11A_6E11_425E_8626_C016072CF8CC__INCLUDED_)
#define AFX_MOOSPLAYBACK_H__326FF11A_6E11_425E_8626_C016072CF8CC__INCLUDED_

#include <MOOSGenLib/MOOSGenLib.h>
#include <MOOSLIB/MOOSLib.h>

using namespace std;

#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <set>


typedef set<string> STRING_SET;
class CMOOSPlayBack  
{
public:
    
    class CPlaybackEntry
    {
    public:
        double m_dfTime;
        string m_sWho;
        string m_sWhat;
        double m_dfVal;
        string m_sVal;
        bool   m_bNumeric;

        bool operator <(const CPlaybackEntry & Entry) const
        {
            if(m_dfTime < Entry.m_dfTime)
                return true;
            
            if(m_dfTime > Entry.m_dfTime)
                return false;
            //mus be equal;
            return m_sVal<Entry.m_sVal;


        };

    protected:

    };

    
    bool SetLastTimeProcessed(double dfTime);
    string GetStatusString();
    bool IsWaitingForClient();
    double GetLastMessageTime();
    bool ClearFilter();
    bool Filter(string sSrc, bool bFilter);
    bool SetTickInterval(double dfInterval);
    bool Reset();
    bool IsEOF();
    bool Iterate(MOOSMSG_LIST & Output);
    double GetFinishTime();
    double GetStartTime();
    int GetCurrentLine();
    double GetTimeNow();
    int GetSize();
    bool IsOpen();
    bool Initialise(const string & sFileName);
    CMOOSPlayBack();
    virtual ~CMOOSPlayBack();


    typedef vector<CPlaybackEntry> PLAYBACKLIST;
    PLAYBACKLIST::iterator m_pListIterator;
    PLAYBACKLIST m_PlayBackList;
    string m_sFileName;
    ifstream m_InputFile;
    double m_dfLastMessageTime;
    int m_nLastLine;
    double m_dfTickTime;
    double m_dfLastClientProcessedTime;
    STRING_SET m_Sources;
    STRING_SET m_SourceFilter;
    double m_dfClientLagTime;

protected:
    bool m_bWaitingForClientCatchup;
    double m_dfLogStart;
    bool CrackAndAdjustEntry(CPlaybackEntry & rEntry);
    bool Entry2Message(CPlaybackEntry  rEntry,CMOOSMsg & Msg);    
    bool m_bEOF;
    bool ParseFile();
public:
    bool GotoTime(double dfT);
};

#endif // !defined(AFX_MOOSPLAYBACK_H__326FF11A_6E11_425E_8626_C016072CF8CC__INCLUDED_)
