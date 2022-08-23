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
// MOOSScheduler.h: interface for the CMOOSScheduler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSSCHEDULER_H__0417EFA7_3661_4FF3_AAA3_3F1B6B14A2B6__INCLUDED_)
#define AFX_MOOSSCHEDULER_H__0417EFA7_3661_4FF3_AAA3_3F1B6B14A2B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

class CMOOSScheduler : public CMOOSApp  
{
public:
    class CEvent
    {
    public:
        bool SetNameVal(std::string sValName);
        bool SetStartingTime(double dfTime);
        bool OnNewMail(MOOSMSG_LIST &NewMail);
        bool SetEnableFlags(std::string sStart, std::string sStop);
        bool SetOffset(double dfOffset);
        bool SetPeriod(double dfPeriod);
        bool Initialise(std::string sName,std::string sVal,double dfPeriod = 1.0,double dfOffset = 0);

        bool Enable(bool bEnable,double dfTimeNow);
        double m_dfFireTime;
        CEvent();
        bool GetOutput(double dfTimeNow,MOOSMSG_LIST & Out);
        
    protected:
        bool m_bEnabled;
        double m_dfTimer;
        double m_dfPeriod;
        double m_dfOffset;
        std::string m_sVal;
        std::string m_sName;

        std::string m_sVarName;        
        std::string m_sStartFlag;
        std::string m_sStopFlag;

    };
    typedef std::list<CEvent> EVENT_LIST;


    class CResponseMsg
    {
    public:
        std::string m_sName;
        std::string m_sVal;
    };    
    typedef std::list<CResponseMsg> RESPONSE_LIST;
    typedef std::map<std::string,RESPONSE_LIST> RESPONSE_LIST_MAP;



    CMOOSScheduler();
    virtual ~CMOOSScheduler();

    /** virtual overide of base class CMOOSApp member. Here we do all the processing and IO*/
    bool Iterate();

    /** virtual overide of base class CMOOSApp member. Here we register for data we wish be
    informed about*/
    bool OnConnectToServer();

    /** called before we start main application loop*/
    bool OnStartUp();

    /** Called when ever mail comes in */
    bool OnNewMail(MOOSMSG_LIST &NewMail);

    /** the whole point **/
    bool Schedule();
    


protected:
    bool RegisterResponses();
    bool HandleResponses(MOOSMSG_LIST & NewMail);
    bool OnControl(std::string sControl);
    bool Initialise();
    bool Clean();
    bool OnRestart();
    bool m_bQuit;
    bool m_bActive;

    /** Win32 handle to  thread */
    #ifdef _WIN32
        HANDLE m_hScheduleThread;
    #endif

    /** ID of  thread */
    unsigned long        m_nScheduleThreadID;

    bool StartThreads();
    bool StopThreads();

    bool AddResponses();
    bool AddTimers();
    bool AddSequences();
    EVENT_LIST m_Events;
    RESPONSE_LIST_MAP m_Responses;
    int m_nTicks;

};

#endif // !defined(AFX_MOOSSCHEDULER_H__0417EFA7_3661_4FF3_AAA3_3F1B6B14A2B6__INCLUDED_)
