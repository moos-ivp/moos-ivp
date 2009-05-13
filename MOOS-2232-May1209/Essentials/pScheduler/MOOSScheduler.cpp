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
// MOOSScheduler.cpp: implementation of the CMOOSScheduler class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include "MOOSScheduler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

#ifdef _WIN32

DWORD WINAPI ScheduleLoopProc( LPVOID lpParameter)
{
    MOOSTrace("starting Scheduler Thread....\n");
    
    CMOOSScheduler* pMe =     (CMOOSScheduler*)lpParameter;
    
    return pMe->Schedule();    
}

#else

void * ScheduleLoopProc( void * lpParameter)
{
    
    MOOSTrace("starting Scheduler Thread....\n");
    CMOOSScheduler* pMe =     (CMOOSScheduler*)lpParameter;
    
    pMe->Schedule();    
    
    return NULL;
}

#endif


bool CMOOSScheduler::StopThreads()
{
    m_bQuit = true;
    
#ifdef _WIN32
    WaitForSingleObject(m_hScheduleThread,INFINITE);
#else
    void * Result;
    pthread_join( (pthread_t)m_nScheduleThreadID,&Result);
#endif
    
    return true;
}

bool CMOOSScheduler::StartThreads()
{
    m_bQuit = false;
    
#ifdef _WIN32
    //this is the main schedule thread
    m_hScheduleThread = ::CreateThread(    NULL,
        0,
        ScheduleLoopProc,
        this,
        CREATE_SUSPENDED,
        &m_nScheduleThreadID);
    ResumeThread(m_hScheduleThread);
        
#else
        
    int Status = pthread_create( (pthread_t*)& m_nScheduleThreadID,NULL,ScheduleLoopProc,this);
    
    if(Status!=0)
    {
        return false;
    }    
    
#endif
    
    return true;
}



CMOOSScheduler::CMOOSScheduler()
{
    m_bQuit = false;
    m_bActive = true;
}

CMOOSScheduler::~CMOOSScheduler()
{

}


bool CMOOSScheduler::Iterate()
{
    
    return true;
}

bool CMOOSScheduler::OnConnectToServer()
{
    m_Comms.Register("RESTART_SCHEDULER",0);
    m_Comms.Register("SCHEDULER_CONTROL",0);

    AddResponses();

    return true;
}

bool CMOOSScheduler::OnStartUp()
{
    Initialise();
    return true;
}

bool CMOOSScheduler::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    if(m_Comms.PeekMail(NewMail,"RESTART_SCHEDULER",Msg,true))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            OnRestart();
        }
    }

    if(m_Comms.PeekMail(NewMail,"SCHEDULER_CONTROL",Msg,true))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            OnControl(Msg.m_sVal);
        }
    }

    //give Events start and stop signals
    EVENT_LIST::iterator q;
    for(q=m_Events.begin();q!=m_Events.end();q++)
    {
        q->OnNewMail(NewMail);
    }


    HandleResponses(NewMail);

    
    
    return true;
}

bool CMOOSScheduler::AddSequences()
{
    // SEQUENCE = SEQ_START      : TRUE @ 3.0
    // SEQUENCE = SAS_INHIBIT : FALSE @ 0.0
    // SEQUENCE = SAS_INHIBIT : TRUE @ 1.9
    // SEQUENCE = DVL_INHIBIT : FALSE @ 1.9
    // SEQUENCE = DVL_INHIBIT : TRUE @ 2.3
    // SEQUENCE = SEQ_END      : TRUE @ 3.0

    STRING_LIST sParams;
    m_MissionReader.GetConfiguration(GetAppName(),sParams);
    STRING_LIST::iterator p;

    EVENT_LIST LocalList;

    double dfTimeMax = -1;
    for(p = sParams.begin();p!=sParams.end();p++)
    {
        string sTok,sVal;
        if(m_MissionReader.GetTokenValPair(*p,sTok,sVal))
        {
            if(!MOOSStrCmp(sTok,"SEQUENCE"))
                continue;

            //get the names
            string sName = MOOSChomp(sVal,":");
            string sNameVal = MOOSChomp(sVal,"@");
            if(sVal.empty())
            {
                MOOSTrace("Missing time in sequence setting!");
                continue;
            }
            double dfOffset = atof(sVal.c_str());

            dfTimeMax = dfOffset>dfTimeMax?dfOffset:dfTimeMax;
                    
            CEvent NewEvent;
            NewEvent.Initialise(sName,sNameVal,-1,dfOffset);

            LocalList.push_front(NewEvent);

        }
    }

    EVENT_LIST::iterator q;
    for(q = LocalList.begin();q!=LocalList.end();q++)
    {
        q->SetPeriod(dfTimeMax);
    }

    m_Events.splice(m_Events.begin(),LocalList);

    return true;
}

bool CMOOSScheduler::AddTimers()
{

    //TIMER = VARNAME @ 0.4, STARTFLAG,STOPFLAG,[VARIABLE_VAL_NAME] -> VALUE

    STRING_LIST sParams;
    m_MissionReader.GetConfiguration(GetAppName(),sParams);
    STRING_LIST::iterator p;

    for(p = sParams.begin();p!=sParams.end();p++)
    {
        string sTok,sVal;
        if(m_MissionReader.GetTokenValPair(*p,sTok,sVal))
        {
            if(!MOOSStrCmp(sTok,"TIMER"))
                continue;

            //get the names
            string sName = MOOSChomp(sVal,"@");
            string sTime = MOOSChomp(sVal,",");
            string sFlags = MOOSChomp(sVal,"->");
            string sNameVal = sVal;           
            string sStartFlag = MOOSChomp(sFlags,",");
            string sEndFlag = MOOSChomp(sFlags,",");
            string sVarVal = MOOSChomp(sFlags,",");

            CEvent NewEvent;

            double dfPeriod = atof(sTime.c_str());

            NewEvent.Initialise(sName,sNameVal,dfPeriod);
            NewEvent.SetEnableFlags(sStartFlag,sEndFlag);

            //waht value do we subscribve to?
            NewEvent.SetNameVal(sVarVal);

            if(!sStartFlag.empty())
                m_Comms.Register(sStartFlag,0);

            if(!sEndFlag.empty())
                m_Comms.Register(sEndFlag,0);

            if(!sVarVal.empty())
                m_Comms.Register(sVarVal,0);

            m_Events.push_front(NewEvent);

        }
    }
    return true;
}

bool CMOOSScheduler::AddResponses()
{
    m_Responses.clear();

    //RESPONSE = ON_COMPLETE[=val] : FLAG_1 @ val1, FLAG_2@ val 2 etc

    STRING_LIST sParams;
    m_MissionReader.GetConfiguration(GetAppName(),sParams);
    STRING_LIST::iterator p;

    for(p = sParams.begin();p!=sParams.end();p++)
    {
        string sTok,sVal;
        if(m_MissionReader.GetTokenValPair(*p,sTok,sVal))
        {
            if(!MOOSStrCmp(sTok,"RESPONSE"))
                continue;

            string sName = MOOSChomp(sVal,":");
            MOOSToUpper(sName);

            RESPONSE_LIST NewList;

            while(!sVal.empty())
            {
                string sChunk = MOOSChomp(sVal,",");
                
                string sFlag =  MOOSChomp(sChunk,"@");
                string sVal = sChunk;

                CResponseMsg NewElement;
                NewElement.m_sName = sFlag;
                NewElement.m_sVal = sVal;

                NewList.push_front(NewElement);
            }

            m_Responses[sName] = NewList;
        }
    }

    //now check for circular source and sinks..
    RESPONSE_LIST_MAP::iterator q,t;

    for(q = m_Responses.begin();q!=m_Responses.end();q++)
    {
        RESPONSE_LIST & rList = q->second;

        RESPONSE_LIST::iterator w;

        for(w = rList.begin();w!= rList.end();w++)
        {
            RESPONSE_LIST_MAP::iterator v = m_Responses.find(w->m_sName);
            if(v!=m_Responses.end())
            {
                MOOSTrace("Response setup error: cannot sink and source %s\n",w->m_sName.c_str());
                t = q++;
                m_Responses.erase(t);
            }
        }
    }


    RegisterResponses();
    
    
    return true;
}



CMOOSScheduler::CEvent::CEvent()
{
    m_bEnabled = true;
    m_dfFireTime = 0 ;
    m_dfPeriod = 1.0;
    m_dfOffset = 0;
}

bool CMOOSScheduler::CEvent::GetOutput(double dfTimeNow, MOOSMSG_LIST &Out)
{
    if(dfTimeNow>m_dfFireTime && m_bEnabled)
    {
        //set up next fire time
        m_dfFireTime+=m_dfPeriod;

        CMOOSMsg Msg(    MOOS_NOTIFY,
                        m_sName.c_str(),
                        m_sVal.c_str(),
                        dfTimeNow);

        Out.push_front(Msg);

        return true;
    }
    else
    {
        return false;
    }
}

bool CMOOSScheduler::CEvent::Enable(bool bEnable,double dfTimeNow)
{
    m_bEnabled = bEnable;
    return true;
}

bool CMOOSScheduler::CEvent::Initialise(string sName, string sVal, double dfPeriod,double dfOffset)
{
    m_sName = sName;
    m_sVal = sVal;
    m_dfPeriod = dfPeriod;
    m_dfOffset = dfOffset;

    return true;
}

bool CMOOSScheduler::CEvent::SetPeriod(double dfPeriod)
{
    m_dfPeriod = dfPeriod;
    return true;
}

bool CMOOSScheduler::CEvent::SetOffset(double dfOffset)
{
    m_dfOffset = dfOffset;
    return true;
}

bool CMOOSScheduler::Schedule()
{
    while(!m_bQuit)
    {
        double dfTimeNow = MOOSTime();

        EVENT_LIST::iterator p;
        MOOSMSG_LIST MailOut;
        for(p = m_Events.begin();p!=m_Events.end();p++)
        {
            CEvent & rEvent = *p;
            if(m_nTicks==0)
                rEvent.SetStartingTime(dfTimeNow);

            rEvent.GetOutput(dfTimeNow,MailOut);
            while(!MailOut.empty())
            {
                if(m_bActive)
                {
                    m_Comms.Post(MailOut.front());
                }
                MailOut.pop_front();
            }
        }

        m_nTicks++;

        //reschedule this thread
        MOOSPause(1);
    }

    MOOSTrace("Scheduler Thread Quits\n");
    return true;
}

bool CMOOSScheduler::OnRestart()
{
    MOOSDebugWrite("Scheduler Restarting....");
    Clean();
    return Initialise();
}

bool CMOOSScheduler::Clean()
{
    StopThreads();
    m_Events.clear();
    m_Responses.clear();
    return true;
}

bool CMOOSScheduler::Initialise()
{
    if(!AddSequences())
        return false;

    if(!AddTimers())
        return false;

    if(!AddResponses())
        return true;

    SetCommsFreq(50);
    SetAppFreq(50);

    m_nTicks = 0;

    if(!StartThreads())
        return false;

    m_bActive = true;

    return true;

}

bool CMOOSScheduler::OnControl(string sControl)
{
    if(sControl.find("ON")!=string::npos)
    {
        m_bActive = true;
        MOOSDebugWrite("Activating scheduler");
    }
    if(sControl.find("OFF")!=string::npos)
    {
        m_bActive = false;
        MOOSDebugWrite("De-activating scheduler");
    }

    return true;
}

bool CMOOSScheduler::HandleResponses(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    for(p = NewMail.begin();p!=NewMail.end();p++)
    {
        string sResponseKey1 = p->GetKey();
        string sResponseKey2 = p->GetKey() + "=" + p->GetString();
        MOOSToUpper(sResponseKey2);

        STRING_LIST lsKeys;
        lsKeys.push_back(sResponseKey1);
        lsKeys.push_back(sResponseKey2);
    
        STRING_LIST::iterator l;
        for (l = lsKeys.begin(); l != lsKeys.end(); ++l)
        {
        
            string sKey = *l;
            RESPONSE_LIST_MAP::iterator q = m_Responses.find(sKey);


            if(q!=m_Responses.end())
            {
                RESPONSE_LIST & rList = q->second;
                
                RESPONSE_LIST::iterator w;
                
                for(w = rList.begin();w!= rList.end();w++)
                {            
                    if(m_bActive)
                    {
                        m_Comms.Notify(w->m_sName,w->m_sVal);
                    }                
                }
            }
        }
    }

    return true;
}

bool CMOOSScheduler::RegisterResponses()
{
    RESPONSE_LIST_MAP::iterator q;
    for(q = m_Responses.begin();q!=m_Responses.end();q++)
    {
        string sResponseKey = q->first;
        string sResponseVar = MOOSChomp(sResponseKey, "=");
        m_Comms.Register(sResponseVar,0);
    }
    return true;
}

bool CMOOSScheduler::CEvent::SetEnableFlags(string sStart, string sStop)
{
    m_sStartFlag = sStart;
    m_sStopFlag = sStop;
    
    if(!m_sStartFlag.empty())
        m_bEnabled = false;


    
    return true;
}

bool CMOOSScheduler::CEvent::OnNewMail(MOOSMSG_LIST  &NewMail)
{
    CMOOSMsg Msg;
    if(!m_sStartFlag.empty() && CMOOSCommClient::PeekMail(NewMail,m_sStartFlag,Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            Enable(true,MOOSTime());
        }
    }
    if(!m_sStopFlag.empty() && CMOOSCommClient::PeekMail(NewMail,m_sStopFlag,Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            Enable(false,MOOSTime());
        }
    }

    if(!m_sVarName.empty() && CMOOSCommClient::PeekMail(NewMail,m_sVarName,Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            if(Msg.IsDataType(MOOS_STRING))
            {
                m_sVal = Msg.GetString();
            }
            else
            {
                m_sVal = Msg.GetAsString();
            }
        }
    }




    
    return true;
}

bool CMOOSScheduler::CEvent::SetStartingTime(double dfTime)
{
    m_dfFireTime = dfTime+m_dfOffset;
    return true;
}

bool CMOOSScheduler::CEvent::SetNameVal(string sValName)
{
    m_sVarName   = sValName;
    return true;
}
