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

// HelmApp.h: interface for the CHelmApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HELMAPP_H__F254D4EE_6BA8_423F_8375_4E1A78D5D43F__INCLUDED_)
#define AFX_HELMAPP_H__F254D4EE_6BA8_423F_8375_4E1A78D5D43F__INCLUDED_

#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_SESSION_TIMEOUT 30
typedef map<string, STRING_LIST> PERMISSIONS_MAP;
typedef map<string, double> SESSION_TIMEOUT_MAP;

class CHelmApp : public CMOOSApp  
{
public:

    CHelmApp();
    virtual ~CHelmApp();
    bool Initialise();
    
protected:
    class CTransaction
    {
        protected:
            CMOOSBehaviour    *m_pTransactingTask;
            
            PERMISSIONS_MAP m_PermissionsMap;

            SESSION_TIMEOUT_MAP m_SessionTimeOutMap;
                
            double    m_dfSessionTimeOut;
            double    m_dfTPTaskCompleteTime;
            bool    m_bTransactionIsOpen;
            string    m_sTransactingClient;
            string    m_sHelmAppName;

        public:
            void SetSessionTimeOutMap(SESSION_TIMEOUT_MAP SessionMap);
            void SetPermissionsMap(PERMISSIONS_MAP permissionsMap);
            bool ParseInstruction(string & sInstruction, string &sClient, string & sTaskName);
            double GetTaskCompleteTime();
            double GetSessionTimeOut();
            CMOOSBehaviour * GetTransactingTask();
            void SetTransactionIsOpen(bool bOpen);
            void SetTransactingTask(CMOOSBehaviour *pNewTask);
            void SetTransactingClient(string sClient);
            bool Initialise();
                        
            bool HasPermissions(string sRequest);
            bool HasAccess(string sClient);
            
            CTransaction();
            virtual ~CTransaction();
            void SetTaskCompleteTime(double dfCompleteTime);
            
            void SetSessionTimeOut(double dfTimeOut);
            bool IsOneShot();
            string GetTransactingClient();
            
            
            bool BuildParameterList(string sList, STRING_LIST &ParameterList);
            bool IsTPTaskTimeOutExpired();
            bool IsTPTaskRunning();
            bool IsOpen();
            bool IsTransactingClient(string sClient);
    };

    CTransaction m_Transaction;

    bool InitialiseTransactors();
    bool OnThirdPartyRequest(CMOOSMsg &Msg);
    bool OnTransaction(string sInstruction, double dfTimeNow);
    bool OnTransactionError(string sError, bool bShouldClose = true);
    bool OnTransactionClose(string sClient, bool bAnnounce = true);
    bool OnTransactionOpen(string sClient);

    
    bool PassSafetyCheck();
    bool GetGains();

    bool IsManualOveride();
    bool RestartHelm();
    virtual bool OnStartUp();
    string MakeWayPointsString();
    bool SetThrust(double dfThrust);
    CMOOSTaskReader m_TaskReader;
    bool OnPostIterate();
    bool OnPreIterate();
    bool m_bInitialised;

    // true if in manual mode...
    bool m_bManualOverRide;

    bool OnConnectToServer();
    bool Iterate();
    virtual bool OnNewMail(MOOSMSG_LIST & NewMail);
    
    bool SetElevator(double dfAngleRadians);
    bool SetRudder(double dfAngleRadians);

    CMOOSBehaviour::CControllerGains m_Gains;

    double m_dfCurrentElevator;
    double m_dfCurrentRudder;
    double m_dfCurrentThrust;

    string m_sTaskFile;
    TASK_LIST m_Tasks;
};

#endif // !defined(AFX_HELMAPP_H__F254D4EE_6BA8_423F_8375_4E1A78D5D43F__INCLUDED_)
