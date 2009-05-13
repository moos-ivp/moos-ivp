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
// MOOSBehaviour.h: interface for the CMOOSBehaviour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSBEHAVIOUR_H__1C10DD47_7690_4AEF_9174_0B0EA068A77D__INCLUDED_)
#define AFX_MOOSBEHAVIOUR_H__1C10DD47_7690_4AEF_9174_0B0EA068A77D__INCLUDED_

#include <MOOSLIB/MOOSLib.h>
#include <MOOSUtilityLib/PitchZPID.h>
#include "PathAction.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSTaskDefaults.h"
#include <string>
using namespace std;

typedef  list<string> STRING_LIST;

//unless set all task will time out after this time...
//very important for safety
#define MOOS_DEFUALT_TIMEOUT 100

class CMOOSBehaviour  
{
public:
    class CXYPoint
    {
    
    public:
        double GetY();
        double GetX();
        void SetY(double dfY);
        void SetX(double dfX);
        CXYPoint();
        double m_dfY;
        double m_dfX;
    };
    class CControllerGains
    {
    public:
        CControllerGains();
        //yaw controller gains
        double m_dfYawKp;
        double m_dfYawKd;
        double m_dfYawKi;
        double m_dfYawKiMax;
        
        //Z controller gains
        double m_dfZToPitchKp;
        double m_dfZToPitchKd;
        double m_dfZToPitchKi;
        double m_dfZToPitchKiMax;
        
        //Pitch contoller gains
        double m_dfPitchKp;
        double m_dfPitchKd;
        double m_dfPitchKi;
        double m_dfPitchKiMax;
        
        //endstops
        double m_dfMaxPitch;
        double m_dfMaxRudder;
        double m_dfMaxElevator;
        double m_dfMaxThrust;
        
    };
public:
    virtual void SetTime(double dfTimeNow);
    virtual bool ReInitialise();
    bool Start();
    void SetPriority(int nPriority);
    int GetPriority();
    void SetName(string sName);
    bool SetGains(CControllerGains NewGains);
    double GetCreationTime();
    double GetStartTime();
    bool HasNewRegistration();
    void SetMissionFileReader(CProcessConfigReader* pMissionFileReader);
    string GetName();
    virtual bool SetParam(string sParam, string sVal);
    virtual bool GetNotifications(MOOSMSG_LIST & List);
    virtual bool GetRegistrations(STRING_LIST &List);
    virtual bool OnNewMail(MOOSMSG_LIST & NewMail);
    virtual bool RegularMailDelivery(double dfTimeNow)=0;
    virtual bool Run(CPathAction & DesiredAction);
    CMOOSBehaviour();
    virtual ~CMOOSBehaviour();
    bool ShouldRun();
    

    

protected:
    virtual bool OnStart();
    bool OnError(string sReason);
    bool DebugNotify(const string & sStr);
    double m_dfIterateTime;
    double GetTimeNow(){return m_dfIterateTime;};
    virtual bool OnTimeOut();
    virtual bool OnEvent(const string & sReason="",bool bVerbalNotify = true);
    virtual void Stop(const string & sReason="DONE");

    virtual bool OnComplete();
    bool PeekMail(MOOSMSG_LIST & Mail,const string & sKey,CMOOSMsg & Msg);
    unsigned int m_nPriority;

    //list of messages to be sent to the outside world
    //when given the chance
    MOOSMSG_LIST m_Notifications;
    

    CProcessConfigReader* m_pMissionFileReader;

    class ControlledDOF
    {
    public:
        ControlledDOF();
        bool IsStale(double dfTimeNow,double dfTaskStartTime,double dfTimeOut=10.0);
        bool IsValid();
        double GetCurrent();
        double GetDesired();
        
        double GetErrorTime();
        double GetError();
        void SetTolerance(double dfTol);
        void SetDesired(double dfDesired);
        double GetDT();
        void SetCurrent(double dfCurrent, double dfTime);
        
    protected:
        double  m_dfDesired;
        double  m_dfCurrent;
        double  m_dfCurrentTime;
        double  m_dfTolerance;
        
    };
    
    //status variables
    string      m_sName;
    string      m_sLogPath;
    bool        m_bActive;
    bool        m_bComplete;
    bool        m_bNewRegistrations;
    
    STRING_LIST      m_StartFlags;
    STRING_LIST      m_CompleteFlags;
    STRING_LIST      m_EventFlags;
    double      m_dfTimeOut;
    double      m_dfStartTime;
    double      m_dfCreationTime;
    
    //PID controllers
    CPitchZPID m_ZPID;
    CScalarPID m_YawPID;
    
    CControllerGains m_Gains;
    
    
};

typedef list<CMOOSBehaviour*> TASK_LIST;


#endif // !defined(AFX_MOOSBEHAVIOUR_H__1C10DD47_7690_4AEF_9174_0B0EA068A77D__INCLUDED_)
