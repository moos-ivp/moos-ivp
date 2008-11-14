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

#include <MOOSUtilityLib/MOOSMemoryMapped.h>

typedef set<string> STRING_SET;
class CMOOSPlayBackV2  
{
public:
    
    CMOOSMemMappedAlogFile m_ALog;
    
    bool SetLastTimeProcessed(double dfTime);
    string GetStatusString();
    bool IsWaitingForClient();
    double GetLastMessageTime();
    bool ClearFilter();
    bool Filter(const std::string & sSrc, bool bWanted);
    bool IsFiltered(const std::string & sSrc);
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
    STRING_SET GetSources(){return m_ALog.GetSourceNames();};
    CMOOSPlayBackV2();
    virtual ~CMOOSPlayBackV2();

    string m_sFileName;
    ifstream m_InputFile;
    double m_dfLastMessageTime;
    int m_nLastLine;
    double m_dfTickTime;
    double m_dfLastClientProcessedTime;
    double m_dfClientLagTime;

protected:
    bool m_bWaitingForClientCatchup;
    double m_dfLogStart;

    bool MessageFromLine(const std::string & sLine,CMOOSMsg & Msg);    
    STRING_SET m_SourceFilter;

    int m_nCurrentLine;
public:
    bool GotoTime(double dfT);
};

#endif // !defined(AFX_MOOSPLAYBACK_H__326FF11A_6E11_425E_8626_C016072CF8CC__INCLUDED_)
