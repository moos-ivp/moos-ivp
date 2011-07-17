///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Basic (Common) Application. 
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
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif


// MOOSBehaviour.cpp: implementation of the CMOOSBehaviour class.
//
//////////////////////////////////////////////////////////////////////
#include "math.h"
#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSBehaviour.h"

#include <sstream>
#include <iostream>
using namespace std;


CMOOSBehaviour::CControllerGains::CControllerGains()
{
    
    m_dfPitchKp    =PITCH_PID_KP;
    m_dfPitchKd    =PITCH_PID_KD;
    m_dfPitchKi    =PITCH_PID_KI;
    m_dfPitchKiMax =PITCH_PID_INTEGRAL_LIMIT;


    m_dfYawKp = YAW_PID_KP;
    m_dfYawKd = YAW_PID_KD;
    m_dfYawKi = YAW_PID_KI;
    m_dfYawKiMax = YAW_PID_INTEGRAL_LIMIT;

    m_dfZToPitchKp = Z_TO_PITCH_PID_KP;
    m_dfZToPitchKd = Z_TO_PITCH_PID_KD;
    m_dfZToPitchKi = Z_TO_PITCH_PID_KI;
    m_dfZToPitchKiMax = Z_TO_PITCH_PID_INTEGRAL_LIMIT;

    m_dfMaxPitch = PITCH_MAX;
    m_dfMaxRudder = RUDDER_MAX;
    m_dfMaxElevator = ELEVATOR_MAX;
    m_dfMaxThrust = THRUST_MAX;


}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSBehaviour::CMOOSBehaviour()
{
    m_bComplete = false;
    m_bActive = false;

    m_sName="NotSet";

    m_pMissionFileReader = NULL;

    m_bNewRegistrations = true;

    m_dfTimeOut=MOOS_DEFUALT_TIMEOUT;
    m_dfStartTime=-1;

    m_dfCreationTime = MOOSTime();

    m_dfIterateTime = m_dfCreationTime;

    m_nPriority = 3;

}

CMOOSBehaviour::~CMOOSBehaviour()
{

}

bool CMOOSBehaviour::Run(CPathAction &DesiredAction)
{
    return false;
}

bool CMOOSBehaviour::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    STRING_LIST::iterator p;
    


    for(p=m_StartFlags.begin();p!=m_StartFlags.end();p++)
    {

        if(PeekMail(NewMail,*p,Msg))
        {
            if(Msg.IsType(MOOS_NOTIFY))
            {
                if(!Msg.IsSkewed(GetTimeNow()))
                {
                    if(!m_bActive)
                    {
                        Start();
                    }
                    break;
                }
                else
                {
                    MOOSTrace("stale start flag..too old ignoring. DB still running from previous mission?\n");
                }
            }
        }
    }
    return true;
}

bool CMOOSBehaviour::GetRegistrations(STRING_LIST &List)
{
    //MOOSTrace("Before %s insertion, List is %d big\n ", m_sName.c_str(), List.size());

    List.insert(List.begin(),m_StartFlags.begin(),m_StartFlags.end());
    
    //MOOSTrace("After %s insertion, List is %d big\n ", m_sName.c_str(), List.size());

    m_bNewRegistrations = false;
    
    return true;
}

bool CMOOSBehaviour::PeekMail(MOOSMSG_LIST &Mail, const string &sKey, CMOOSMsg &Msg)
{
    //note we adopt the policy of always taking the youngest message...
    MOOSMSG_LIST::iterator p,q;
    
    q = Mail.end();

    double dfT = -1.0;

    for(p = Mail.begin();p!=Mail.end();p++)
    {
        if(MOOSStrCmp(p->m_sKey,sKey))
        {
            if(dfT<p->GetTime())
            {
                dfT = p->GetTime();
                q = p;
            }
        }
    }

    //did we find one?
    if(    q!=    Mail.end())
    {
        Msg=*q;
        return true;
    }
    else
    {
        return false;
    }
}



CMOOSBehaviour::ControlledDOF::ControlledDOF()
{
    m_dfCurrent = 0;
    m_dfCurrentTime = -1;
    m_dfDesired = 0;
    m_dfTolerance = 0;
}

void CMOOSBehaviour::ControlledDOF::SetCurrent(double dfCurrent, double dfTime)
{
    m_dfCurrent     = dfCurrent;
    m_dfCurrentTime = dfTime;
}


void CMOOSBehaviour::ControlledDOF::SetDesired(double dfDesired)
{
    m_dfDesired  = dfDesired;
}

void CMOOSBehaviour::ControlledDOF::SetTolerance(double dfTol)
{
    m_dfTolerance = dfTol;
}

double CMOOSBehaviour::ControlledDOF::GetError()
{
    double dfErr = m_dfDesired-m_dfCurrent;
    if(fabs(dfErr)<=m_dfTolerance)
    {
        return 0.0;
    }
    else
    {
        return dfErr;
    }

}



double CMOOSBehaviour::ControlledDOF::GetErrorTime()
{
    return m_dfCurrentTime;
}

double CMOOSBehaviour::ControlledDOF::GetDesired()
{
    return m_dfDesired;
}

double CMOOSBehaviour::ControlledDOF::GetCurrent()
{
    return m_dfCurrent;
}



bool CMOOSBehaviour::ControlledDOF::IsValid()
{
    return m_dfCurrentTime!=-1.0;
}

bool CMOOSBehaviour::GetNotifications(MOOSMSG_LIST &List)
{
    List.splice(List.begin(),m_Notifications);

    return true;
}

bool CMOOSBehaviour::OnComplete()
{
    //we are completed!
    //default param is "DONE"
    Stop();
    return true;
}

bool CMOOSBehaviour::ShouldRun()
{
    if(m_bComplete)
        return false; //we have already run once and completed

    if(!m_bActive)
    {
          return false;
    }
     

    if(m_dfStartTime==-1)
    {
        //yep we should go but haven't started as yet
        Start();
    }

    double dfTimeNow = MOOSTime();
    //look for timeouts!!!
    if(m_dfTimeOut!=-1 && ((dfTimeNow -m_dfStartTime)>m_dfTimeOut))
    {
        //hell no! we should have achieved our goal by now..
        OnTimeOut();
        return false;
    }
     
    //look for active task that is not receiving any updates
    //on its watched variables?
    if(!RegularMailDelivery(dfTimeNow))
    {
        ostringstream os;
        os<<"task "<< m_sName.c_str()<<" stops for lack of input"<<endl<<ends;
        string sStr = os.str();
        //os.rdbuf()->freeze(0);

        DebugNotify(sStr);

        //publish error string...
        OnError(sStr);

        Stop(sStr);

        //now figure our what could have gone wrong...
        STRING_LIST Expected;

        GetRegistrations(Expected);

        ostringstream os2;
        os2<<"Not receiving one of:";


        STRING_LIST::iterator p;
        for(p = Expected.begin();p!=Expected.end();p++)
        {
            os2<<p->c_str()<<" ";
        }

        os2<<ends;
        
        sStr = os2.str();

        //os2.rdbuf()->freeze(0);

        //tell the world...
        DebugNotify(sStr);

        return false;
    }

    return true;
}



bool CMOOSBehaviour::Start()
{
    m_bActive = true;
    
    m_dfStartTime = MOOSTime();

    ostringstream os;

    os<<"Task "<<m_sName.c_str()<<" goes active"<<endl<<ends;

    MOOSTrace(os.str());
    
    DebugNotify(os.str());
    
//    os.rdbuf()->freeze(0);
    
    //let derived classes do soething
    return OnStart();

}

void CMOOSBehaviour::Stop(const string & sReason)
{
    m_bComplete = true;
    m_bActive = false;

    STRING_LIST::iterator p;


    for(p=m_CompleteFlags.begin();p!=m_CompleteFlags.end();p++)
    {
        CMOOSMsg DoneMsg(MOOS_NOTIFY,p->c_str(),sReason.c_str());
        m_Notifications.push_front(DoneMsg);

        DebugNotify(sReason);

    }

}

bool CMOOSBehaviour::SetParam(string sParam, string sVal)
{
    MOOSToUpper(sParam);
    MOOSToUpper(sVal);

    if(sParam=="NAME")
    {
        m_sName = sVal;
    }

    else if(sParam=="INITIALSTATE")
    {
        if(sVal =="ON")
        {
            m_bActive = true;
        }
        else
        {
            m_bActive = false;
        }
    }
    else if(sParam=="PRIORITY")
    {
        m_nPriority = atoi(sVal.c_str());
    }
    else if(sParam=="STARTFLAG")
    {
        while(!sVal.empty())
        {
            string sFlag = MOOSChomp(sVal,",");
            m_StartFlags.push_front(sFlag);
        }
    }
    else if(sParam =="FINISHFLAG")
    {
        while(!sVal.empty())
        {
            string sFlag = MOOSChomp(sVal,",");
            m_CompleteFlags.push_front(sFlag);
        }

    }
    else if(sParam=="EVENTFLAG")
    {
        while(!sVal.empty())
        {
            string sFlag = MOOSChomp(sVal,",");
            m_EventFlags.push_front(sFlag);
        }
    }
    else if(sParam == "TIMEOUT")
    {
        if(sVal=="NEVER")
        {
            m_dfTimeOut = -1;
        }
        else
        {
            m_dfTimeOut = atof(sVal.c_str());
            if(m_dfTimeOut==0.0)
            {
                MOOSTrace("warning task set for zero timeout..is this intended?\n");
            }
        }
    }
    else if(sParam=="LOGPID")
    {
        bool bLog = MOOSStrCmp(sVal,"TRUE");
        m_ZPID.SetLog(bLog);
        m_YawPID.SetLog(bLog);
    }
    else
    {
        return false;
    }

    return true;

}

string CMOOSBehaviour::GetName()
{
    return m_sName;
}

bool CMOOSBehaviour::OnEvent(const string & sReason,bool bVerbalNotify)
{

    STRING_LIST::iterator p;

    if(bVerbalNotify)
    {
        DebugNotify(sReason);
    }

    for(p=m_EventFlags.begin();p!=m_EventFlags.end();p++)
    {
        CMOOSMsg EventMsg(MOOS_NOTIFY,p->c_str(),sReason.c_str());
        m_Notifications.push_front(EventMsg);

    }

    


    return true;
}

void CMOOSBehaviour::SetMissionFileReader(CProcessConfigReader* pMissionFileReader)
{
    m_pMissionFileReader = pMissionFileReader;
}

//defualt behaviour....
bool CMOOSBehaviour::OnTimeOut()
{
    ostringstream os;

    os<<"Task "<<m_sName.c_str()<<" timed out after "<<m_dfTimeOut<<" seconds"<<endl<<ends;

    MOOSTrace(os.str());
        
    Stop(os.str());
    
    //os.rdbuf()->freeze(0);

    return true;
}

bool CMOOSBehaviour::HasNewRegistration()
{
    return m_bNewRegistrations;
}

bool CMOOSBehaviour::ControlledDOF::IsStale(double dfTimeNow, double dfTaskStartTime,double dfTimeOut)
{
    //case 1: No mail at all...
    if(!IsValid()&& (dfTimeNow-dfTaskStartTime>dfTimeOut))
        return true;

    //case 2: Had mail but it has stopped..
    if(IsValid() && dfTimeNow-m_dfCurrentTime>dfTimeOut)
        return true;

    //then we are OK...
    return false;
}

double CMOOSBehaviour::GetStartTime()
{
    return m_dfStartTime;
}

double CMOOSBehaviour::GetCreationTime()
{
    return m_dfCreationTime;
}

bool CMOOSBehaviour::DebugNotify(const string &sStr)
{
    CMOOSMsg DebugMsg(MOOS_NOTIFY,"MOOS_DEBUG",sStr.c_str());
    m_Notifications.push_front(DebugMsg);    

    MOOSTrace(sStr);

    return true;
}

bool CMOOSBehaviour::OnError(string sReason)
{
    string sKey = m_sName+"_ERROR";

    CMOOSMsg ErrorMsg(MOOS_NOTIFY,sKey.c_str(),sReason.c_str());

    m_Notifications.push_front(ErrorMsg);

    return true;
}

//set up controller gains...given to each new task...
bool CMOOSBehaviour::SetGains(CMOOSBehaviour::CControllerGains NewGains)
{
    //copy...
    m_Gains = NewGains;

    //set up names
    string sZ = MOOSFormat("ZControl%s",m_sName.c_str());
    string sYaw = MOOSFormat("YawControl%s",m_sName.c_str());

    m_ZPID.SetName(sZ);
    m_YawPID.SetName(sYaw);

    if(m_pMissionFileReader!=NULL)
    {
        //do we have a path global name?
        if(!m_pMissionFileReader->GetValue("GLOBALLOGPATH",m_sLogPath))
        {
            //no do we have a local name?
            m_pMissionFileReader->GetConfigurationParam("PIDLOGPATH",m_sLogPath);
        }
        m_ZPID.SetLogPath(m_sLogPath);
        m_YawPID.SetLogPath(m_sLogPath);
    }



    //configure...
    m_ZPID.SetGains(m_Gains.m_dfZToPitchKp,
                    m_Gains.m_dfZToPitchKd,
                    m_Gains.m_dfZToPitchKi,
                    m_Gains.m_dfPitchKp,
                    m_Gains.m_dfPitchKd,
                    m_Gains.m_dfPitchKi);

    m_ZPID.SetLimits(m_Gains.m_dfMaxPitch,  //maximum outerloop on pitch
                    m_Gains.m_dfMaxElevator,//maximum inner loop on elevator
                    m_Gains.m_dfZToPitchKiMax,
                    m_Gains.m_dfPitchKiMax);


    m_YawPID.SetGains(    m_Gains.m_dfYawKp,
                        m_Gains.m_dfYawKd,
                        m_Gains.m_dfYawKi);

    m_YawPID.SetLimits(m_Gains.m_dfYawKiMax,m_Gains.m_dfMaxRudder);



    return true;
}


CMOOSBehaviour::CXYPoint::CXYPoint()
{
    m_dfX = 0.0;
    m_dfY = 0.0;
}

void CMOOSBehaviour::CXYPoint::SetX(double dfX)
{
    m_dfX = dfX;
}

void CMOOSBehaviour::CXYPoint::SetY(double dfY)
{
    m_dfY = dfY;
}

double CMOOSBehaviour::CXYPoint::GetX()
{
    return m_dfX;
}

double CMOOSBehaviour::CXYPoint::GetY()
{
    return m_dfY;
}

void CMOOSBehaviour::SetName(string sName)
{
    m_sName = sName;
}

int CMOOSBehaviour::GetPriority()
{
    return m_nPriority;
}

void CMOOSBehaviour::SetPriority(int nPriority)
{
    m_nPriority = nPriority;
}

bool CMOOSBehaviour::ReInitialise()
{

    m_bComplete = false;
    m_bActive = false;

    m_sName="NotSet";

    m_pMissionFileReader = NULL;

    m_bNewRegistrations = true;

    m_dfTimeOut=MOOS_DEFUALT_TIMEOUT;
    
    m_dfStartTime=-1;

    m_dfCreationTime = MOOSTime();

    m_dfIterateTime = m_dfCreationTime;

    m_nPriority = 3;

    return true;
}

void CMOOSBehaviour::SetTime(double dfTimeNow)
{
    m_dfIterateTime = dfTimeNow;
}


//overload this to do something as task is starting
bool CMOOSBehaviour::OnStart()
{
    return true;
}
