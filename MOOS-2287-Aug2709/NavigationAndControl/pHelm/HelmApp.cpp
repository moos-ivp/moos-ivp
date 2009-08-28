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
    #pragma warning(disable : 4503)
#endif



// HelmApp.cpp: implementation of the CHelmApp class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include <MOOSTaskLib/MOOSTaskLib.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>


using namespace std;
#include "HelmApp.h"

#define MAX_TASKS 200

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHelmApp::CHelmApp()
{
    m_bInitialised  =   false;

    m_dfCurrentElevator=0;
    m_dfCurrentRudder=0;
    m_dfCurrentThrust=0;  

   
    //some sensible defaults (missionfile can overwrite this)
    SetAppFreq(5);
    SetCommsFreq(15);

    m_bManualOverRide = true;

}

CHelmApp::~CHelmApp()
{

    TASK_LIST::iterator p;

    for(p = m_Tasks.begin();p!=m_Tasks.end();p++)
    {
        delete  *p;

    }
    m_Tasks.clear();

}


bool CHelmApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
 
    CMOOSMsg Msg;
    if(m_Comms.PeekMail(NewMail,"MOOS_MANUAL_OVERIDE",Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            if(MOOSStrCmp(Msg.m_sVal,"TRUE"))
            {
                MOOSTrace("Manual Overide is on\n");
                MOOSDebugWrite("Manual Overide is on");
                m_bManualOverRide = true;
            }
            else
            {
                MOOSTrace("Manual Overide is off\n");
                MOOSDebugWrite("Manual Overide is off!");
                m_bManualOverRide = false;
            }
        }
    }
    else if(m_Comms.PeekMail(NewMail,"RESTART_HELM",Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            if(MOOSStrCmp(Msg.m_sVal,"TRUE"))
            {
                RestartHelm();
            }
        }
    }
    else if(m_Comms.PeekMail(NewMail,"THIRDPARTY_REQUEST",Msg))
    {
        OnThirdPartyRequest(Msg);
    }
    else
    {
        //send to tasks...
        TASK_LIST::iterator p;

        for(p = m_Tasks.begin();p!=m_Tasks.end();p++)
        {
            CMOOSBehaviour* pBehaviour = *p;

            pBehaviour->OnNewMail(NewMail);
        }
    }
    return true;
}


bool CHelmApp::OnConnectToServer()
{

    string sWPs=MakeWayPointsString();
    m_Comms.Notify("WAY_POINTS",sWPs);

    m_Comms.Register("RESTART_HELM",0.1);
    m_Comms.Register("MOOS_MANUAL_OVERIDE",0.1);
    m_Comms.Register("THIRDPARTY_REQUEST",0.1);


    return true;
}


bool CHelmApp::Iterate()
{
    if(m_bInitialised==false)
    {
        MOOSTrace("CHelmApp::Iterate() FAIL : Helm not initialised\n");
        return false;
    }
    

    if(IsManualOveride())
    {
        //we do nothing!
        return true;
    }

    CPathAction DesiredAction;
 
    OnPreIterate();

    TASK_LIST::iterator p;

    double dfTimeNow = MOOSTime();

    for(p = m_Tasks.begin();p!=m_Tasks.end();p++)
    {
        CMOOSBehaviour* pBehaviour = *p;

        pBehaviour->SetTime(dfTimeNow);

        if(pBehaviour->ShouldRun())
        {
            pBehaviour->Run(DesiredAction);
        }

    }

    OnPostIterate();

    if(m_Transaction.IsOpen())
    {
        //if we are OneShot, just close down
        if(!m_Transaction.IsTPTaskRunning() &&
            m_Transaction.IsOneShot())
        {
            OnTransactionClose(m_Transaction.GetTransactingClient());
        }

        //otherwise just need to keep track of when the TPIT died
        //and close the transaction after the sessionTimeOut expires
        if(!m_Transaction.IsTPTaskRunning() &&
            m_Transaction.GetTaskCompleteTime() == -1)
        {
            m_Transaction.SetTaskCompleteTime(MOOSTime());
        }
        else if((m_Transaction.GetTaskCompleteTime() != -1) &&
            (m_Transaction.IsTPTaskTimeOutExpired()))
        {
            OnTransactionClose(m_Transaction.GetTransactingClient());
        }


    }

    //desired is now where we want to go...
    double dfEl = DesiredAction.Get(ACTUATOR_ELEVATOR);
    SetElevator(dfEl);

    double dfRudder = DesiredAction.Get(ACTUATOR_RUDDER);
    SetRudder(dfRudder);

    double dfThrust = DesiredAction.Get(ACTUATOR_THRUST);
    SetThrust(dfThrust);

    return true;
}

bool CHelmApp::Initialise()
{

    m_bInitialised = false;

    m_dfCurrentElevator=0;
    m_dfCurrentRudder=0;
    m_dfCurrentThrust=0;  
 

    //we need to have talked to the DB to get our skew
    //time before this as tasks need a start time
    //this is unusual behaviour for a MOOS process but
    //does mean things are safe...
    int nCount = 0;
    while(!m_Comms.IsConnected())
    {
        
        MOOSPause(1000);
        if(nCount++>30)
        {
            MOOSTrace("Cannot initialise helm without connecting to Server\n");
            MOOSTrace("Waited 30 seconds..quiting\n");
            return false;
        }
    }
        

    if(!m_MissionReader.GetConfigurationParam("TASKFILE",m_sTaskFile))
    {
        m_sTaskFile = m_sMissionFile;
    }

    
    InitialiseTransactors();
    
    
    
    //set up controller gains and limits
    if(!GetGains())
        return false;

    if(!m_TaskReader.Run(   m_sTaskFile.c_str(),
                            &m_MissionReader,
                            m_Gains,
                             m_Tasks))
    {
        MOOSDebugWrite("Error in Mission File while making tasks. Helm quits");
        MOOSPause(4000);
        return false;
    }



    //not too many task allowed (ie hundreds...)
    if(m_Tasks.size()>MAX_TASKS)
    {
        MOOSTrace("no more than %d tasks can be run in parrallel %d were requested ! \n",MAX_TASKS,m_Tasks.size());
        return false;
    }

    //check that we have and EndMission as well as an OverallTimeOut 
    if(!PassSafetyCheck())
    {
        MOOSDebugWrite("Must have both EndMission and OverallTimeOut tasks for Helm to work");
        return false;
    }
    //helpful to tell the world what waypoints have been made
    string sWPs=MakeWayPointsString();
    m_Comms.Notify("WAY_POINTS",sWPs);


    //if we have got to here we are OK
    m_bInitialised = true;

    return m_bInitialised;
}


bool CHelmApp::GetGains()
{

    bool bSuccess = true;

    //get yaw gains....
    if(!m_MissionReader.GetConfigurationParam("YAW_PID_KP",m_Gains.m_dfYawKp))
    {
        MOOSDebugWrite("YAW_PID_KP not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("YAW_PID_KD",m_Gains.m_dfYawKd))
    {
        MOOSDebugWrite("YAW_PID_KD not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("YAW_PID_KI",m_Gains.m_dfYawKi))
    {
        MOOSDebugWrite("YAW_PID_KI not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("YAW_PID_INTEGRAL_LIMIT",m_Gains.m_dfYawKiMax))
    {
        MOOSDebugWrite("YAW_PID_INTEGRAL_LIMIT not found in Mission File");
        bSuccess&=false;
    }

    if(!m_MissionReader.GetConfigurationParam("Z_TO_PITCH_PID_KP",m_Gains.m_dfZToPitchKp))
    {
        MOOSDebugWrite("Z_TO_PITCH_PID_KP not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("Z_TO_PITCH_PID_KD",m_Gains.m_dfZToPitchKd))
    {
        MOOSDebugWrite("Z_TO_PITCH_PID_KD not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("Z_TO_PITCH_PID_KI",m_Gains.m_dfZToPitchKi))
    {
        MOOSDebugWrite("Z_TO_PITCH_PID_KI not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("Z_TO_PITCH_PID_INTEGRAL_LIMIT",m_Gains.m_dfZToPitchKiMax))
    {
        MOOSDebugWrite("Z_TO_PITCH_PID_INTEGRAL_LIMIT not found in Mission File");
        bSuccess&=false;
    }

    if(!m_MissionReader.GetConfigurationParam("PITCH_PID_KP",m_Gains.m_dfPitchKp))
    {
        MOOSDebugWrite("PITCH_PID_KP not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("PITCH_PID_KD",m_Gains.m_dfPitchKd))
    {
        MOOSDebugWrite("PITCH_PID_KD not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("PITCH_PID_KI",m_Gains.m_dfPitchKi))
    {
        MOOSDebugWrite("PITCH_PID_KI not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("PITCH_PID_INTEGRAL_LIMIT",m_Gains.m_dfPitchKiMax))
    {
        MOOSDebugWrite("PITCH_PID_INTEGRAL_LIMIT not found in Mission File");
        bSuccess&=false;
    }

    //end stops:
    if(!m_MissionReader.GetConfigurationParam("MAXPITCH",m_Gains.m_dfMaxPitch))
    {
        MOOSDebugWrite("MAXPITCH not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("MAXELEVATOR",m_Gains.m_dfMaxElevator))
    {
        MOOSDebugWrite("MAXELEVATOR not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("MAXRUDDER",m_Gains.m_dfMaxRudder))
    {
        MOOSDebugWrite("MAXRUDDER not found in Mission File");
        bSuccess&=false;
    }
    if(!m_MissionReader.GetConfigurationParam("MAXTHRUST",m_Gains.m_dfMaxThrust))
    {
        MOOSDebugWrite("MAXTHRUST not found in Mission File");
        bSuccess&=false;
    }

    //now a few human to MOOS conversions
    m_Gains.m_dfMaxRudder = MOOSDeg2Rad(m_Gains.m_dfMaxRudder);
    m_Gains.m_dfMaxElevator = MOOSDeg2Rad(m_Gains.m_dfMaxElevator);
    m_Gains.m_dfMaxPitch = MOOSDeg2Rad(m_Gains.m_dfMaxPitch);

#ifdef VERBOSE
    MOOSDebugWrite(MOOSFormat("***    NEW CONTROLLER GAINS ARE:*******"));
    MOOSDebugWrite(MOOSFormat("YAW_PID_KP             = %f",m_Gains.m_dfYawKp));
    MOOSDebugWrite(MOOSFormat("YAW_PID_KD             = %f",m_Gains.m_dfYawKd));
    MOOSDebugWrite(MOOSFormat("YAW_PID_KI             = %f",m_Gains.m_dfYawKi));
    MOOSDebugWrite(MOOSFormat("YAW_PID_INTEGRAL_LIMIT = %f",m_Gains.m_dfYawKiMax));

    MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KP      = %f",m_Gains.m_dfZToPitchKp));
    MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KD      = %f",m_Gains.m_dfZToPitchKd));
    MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KI      = %f",m_Gains.m_dfZToPitchKi));
    MOOSDebugWrite(MOOSFormat("Z_TO_PITCH_PID_KI_LIMIT= %f",m_Gains.m_dfZToPitchKiMax));

    MOOSDebugWrite(MOOSFormat("PITCH_PID_KP           = %f",m_Gains.m_dfPitchKp));
    MOOSDebugWrite(MOOSFormat("PITCH_PID_KD           = %f",m_Gains.m_dfPitchKd));
    MOOSDebugWrite(MOOSFormat("PITCH_PID_KI           = %f",m_Gains.m_dfPitchKi));
    MOOSDebugWrite(MOOSFormat("PITCH_PID_KI_LIMIT     = %f",m_Gains.m_dfPitchKiMax));
#endif


    return bSuccess;    
}

#define UPDATE_INTERVAL 0.2

bool CHelmApp::OnPreIterate()
{
    TASK_LIST::iterator p;

    STRING_LIST NewResources;
    STRING_LIST::iterator q;

    for(p = m_Tasks.begin();p!=m_Tasks.end();p++)
    {
        CMOOSBehaviour* pBehaviour = *p;

        if(pBehaviour->HasNewRegistration())
        {
            NewResources.clear();

            pBehaviour->GetRegistrations(NewResources);

            for(q = NewResources.begin();q!=NewResources.end();q++)
            {
                if(m_Comms.IsConnected())
                {
                    m_Comms.Register(*q,UPDATE_INTERVAL);
                }
            }
        }
    }

    return true;
}


bool CHelmApp::OnPostIterate()
{
    TASK_LIST::iterator p;

    MOOSMSG_LIST Notifications;

    MOOSMSG_LIST::iterator q;

    for(p = m_Tasks.begin();p!=m_Tasks.end();p++)
    {
        CMOOSBehaviour* pBehaviour = *p;

        Notifications.clear();

        pBehaviour->GetNotifications(Notifications);

        for(q = Notifications.begin();q!=Notifications.end();q++)
        {
            if(m_Comms.IsConnected())
            {
                m_Comms.Post(*q);
            }
        }
    }

    return true;
}





bool CHelmApp::SetElevator(double dfAngleRadians)
{

    m_dfCurrentElevator=MOOSRad2Deg(dfAngleRadians);

    MOOSAbsLimit(m_dfCurrentElevator,MOOSRad2Deg(m_Gains.m_dfMaxElevator));

    m_Comms.Notify("DESIRED_ELEVATOR",m_dfCurrentElevator);

    return true;
}

bool CHelmApp::SetRudder(double dfAngleRadians)
{

    m_dfCurrentRudder=MOOSRad2Deg(dfAngleRadians);

    //the PID should have looked after saturation itself but can never be
    //too sure..!
    MOOSAbsLimit(m_dfCurrentRudder,MOOSRad2Deg(m_Gains.m_dfMaxRudder));

    m_Comms.Notify("DESIRED_RUDDER",m_dfCurrentRudder);

    return true;
}


bool CHelmApp::SetThrust(double dfThrust)
{

    m_dfCurrentThrust=dfThrust;

    MOOSAbsLimit(m_dfCurrentRudder,m_Gains.m_dfMaxThrust);

    m_Comms.Notify("DESIRED_THRUST",m_dfCurrentThrust);

    return true;
}

string CHelmApp::MakeWayPointsString()
{
    TASK_LIST::iterator p;

    string sResult;
    for(p = m_Tasks.begin();p!=m_Tasks.end();p++)
    {
        CMOOSBehaviour * pTask = *p;

        CGoToWayPoint* pGotoWpTask = dynamic_cast<CGoToWayPoint*>(pTask);

        if(pGotoWpTask!=NULL)
        {
            ostringstream os;
            os  <<pGotoWpTask->GetName().c_str()
                <<"="<<pGotoWpTask->m_sLocation.c_str()
                <<","<<pGotoWpTask->m_dfVicinityRadius<<";"<<ends;

            sResult+=os.str();

//            os.rdbuf()->freeze(0);

            /*sResult+=   pGotoWpTask->GetName()
                        +"="
                        +pGotoWpTask->m_sLocation
                        +";";*/
        }

    }
    return sResult;
}

bool CHelmApp::OnStartUp()
{
    if(CHelmApp::Initialise())
    {


        return true;
    }
    else
    {
        return false;
    }


}


bool CHelmApp::RestartHelm()
{
    MOOSTrace("Helm restarting!!!!\n");
    
    //tell the system via debug...
    MOOSDebugWrite("Helm::Restart()");


    //need to reload mission file
    m_MissionReader.SetFile(m_sMissionFile.c_str());


    TASK_LIST::iterator p;

    for(p = m_Tasks.begin();p!=m_Tasks.end();p++)
    {
        delete  *p;

    }
    m_Tasks.clear();

    return Initialise();
}

bool CHelmApp::IsManualOveride()
{
    return m_bManualOverRide;
}

bool CHelmApp::PassSafetyCheck()
{
    bool bFoundEndMission        = false;
    bool bFoundOverallTimeOut    = false;
    TASK_LIST::iterator p;

    for(p = m_Tasks.begin();p!=m_Tasks.end();p++)
    {
        CMOOSBehaviour * pTask = *p;

        CEndMission* pEndMission = dynamic_cast<CEndMission*>(pTask);

        if(pEndMission != NULL)
        {
            bFoundEndMission = true;    
        }
        
        COverallTimeOut* pOverallTimeOut = dynamic_cast<COverallTimeOut*>(pTask);

        if(pOverallTimeOut != NULL)
        {
            bFoundOverallTimeOut = true;
        }
        

    }

    return (bFoundEndMission && bFoundOverallTimeOut);
}

/////////////////////////////
/////Transaction Section/////
bool CHelmApp::OnThirdPartyRequest(CMOOSMsg &Msg)
{

    if(m_bManualOverRide)
    {
        MOOSDebugWrite("Cannot launch Thirdparty Task in manual overide mode (press 'o' to relinquish manual control)");
        return false;
    }

    double dfTimeNow = MOOSTime();
    //FORMAT is
    //JOBNAME@CLIENT:TASK=TASKNAME|Task Parameter 1|Task Parameter 2|etc...
    //JOBNAME@CLIENT:CLOSE
    string sInstruction        = Msg.m_sVal;
    string sClient            = MOOSChomp(Msg.m_sVal,":");
    string sNotify;
    
    if(m_Transaction.IsOpen())
    {
        //check that another client is not requesting a transaction
        if(!m_Transaction.IsTransactingClient(sClient) || 
            m_Transaction.IsOneShot())
        {
            sNotify = sClient + 
                " DENIED Access - Presently Transacting with " + 
                m_Transaction.GetTransactingClient() + 
                " [OneShot: " + (m_Transaction.IsOneShot() ? "true" : "false") +
                "]";
            return OnTransactionError(sNotify, false);
        }
        else if(sInstruction.find("TASK") != string::npos)
        {
            //first close to remove the presently active Task
            OnTransactionClose(sClient, false);
            return OnTransaction(sInstruction, dfTimeNow);
        }
        else if(sInstruction.find("CLOSE")!=string::npos)
        {
            return OnTransactionClose(sClient);
        }
        else
        {
            sNotify = m_Transaction.GetTransactingClient() +
            " has ALREADY opened a transaction - command not understood";
            return OnTransactionError(sNotify, false);
        }
    }
    else
    {
        if(sInstruction.find("TASK") != string::npos)
        {
            return OnTransaction(sInstruction, dfTimeNow);
        }
        else
        {
            sNotify = sClient + " Missing 'TASK=' in THIRDPARTY_REQUEST";
            return OnTransactionError(sNotify, false);
        }
    }

}

bool CHelmApp::CTransaction::HasPermissions(string sRequest)
{
    MOOSToUpper(sRequest);
    STRING_LIST AllowableTasks = m_PermissionsMap[m_sTransactingClient];
    
    STRING_LIST::iterator q;
    for(q = AllowableTasks.begin(); q != AllowableTasks.end(); q++)
    {
        //now see if the sRequest the m_sTransactingClient 
        //wants to accomplish is within its list of permissable 
        //tasks to spawn
        string sAllowedTask = *q;
        if(MOOSStrCmp(sRequest,sAllowedTask))
        {
            return true;
        }
    }
    
        
    //the requesting client's task is not allowed to be launched by them
    return false;
        
}



bool CHelmApp::CTransaction::IsTransactingClient(string sClient)
{
    return MOOSStrCmp(sClient,m_sTransactingClient);
}

//add MOOSDbebug info to these methods
bool CHelmApp::OnTransactionOpen(string sClient)
{
    //FORMAT is
    //JOBNAME@CLIENT
    //check that this jobname@client has access
    MOOSToUpper(sClient);
    if(m_Transaction.HasAccess(sClient))
    {
        m_Transaction.SetTransactionIsOpen(true);
        m_Transaction.SetTransactingClient(sClient);
        m_Transaction.SetTransactingTask(NULL);
        m_Transaction.SetTaskCompleteTime(-1);

        m_Comms.Notify("CURRENT_THIRDPARTY", m_Transaction.GetTransactingClient());
    
        return true;
    }
    else
    {    
        return false;
    }

}

bool CHelmApp::OnTransactionClose(string sClient, bool bAnnounce)
{
    //search thru the tasks for this job@client and remove/delete the task
    TASK_LIST::iterator p;
//    MOOSTrace("Task List is %d big\n", m_Tasks.size());
    string sJob = MOOSChomp(sClient,"@");
    MOOSToUpper(sClient);

    //can't call the Stop() method because this will cause the
    //task to publish FinishFlags, which we don't want to have happen
    if(m_Transaction.GetTransactingTask() != NULL)
        m_Tasks.remove(m_Transaction.GetTransactingTask());
    
//    MOOSTrace("After removal, Task List is %d big\n", m_Tasks.size());
    
    //time to close the connection down
    m_Transaction.SetTransactionIsOpen(false);
    m_Transaction.SetTransactingClient("NONE");
    m_Transaction.SetTransactingTask(NULL);
    
    if(bAnnounce)
    {
        m_Comms.Notify("CURRENT_THIRDPARTY", 
            m_Transaction.GetTransactingClient());
    }

    return true;
}

bool CHelmApp::OnTransactionError(string sError, bool bShouldClose)
{
    if(bShouldClose)
        OnTransactionClose(m_Transaction.GetTransactingClient());

    //create a debug message and post to the DB
    MOOSDebugWrite(sError);
    
    return false;
}

bool CHelmApp::OnTransaction(string sInstruction, double dfTimeNow)
{
    string sClient, 
        sTaskName, 
        sError;
    bool bError = false;

    m_Transaction.ParseInstruction(sInstruction, sClient, sTaskName);
    
    if(!OnTransactionOpen(sClient))
    {
        sError = sClient + " is not ALLOWed to transact";
        bError = true;
    }
    else if(!m_Transaction.HasPermissions(sTaskName))
    {
        sError = m_Transaction.GetTransactingClient() + 
            " Lacks Permissions for: " + 
            sTaskName;
        bError = true;
    }

    CMOOSBehaviour * pNewTask = NULL;
    pNewTask = m_TaskReader.MakeNewTask(sTaskName);    
    
    STRING_LIST ParameterList;
    if(pNewTask != NULL)
    {
        //task built correctly, now figure out its parameters
        m_Transaction.BuildParameterList(sInstruction, ParameterList);
        pNewTask->SetGains(m_Gains);
        pNewTask->SetName(sTaskName);

        STRING_LIST::iterator p;
        for(p = ParameterList.begin(); p != ParameterList.end(); p++)
        {
            string sLine = *p;
            MOOSToUpper(sLine);

            string sTok;
            string sVal;
            
            if(m_MissionReader.GetTokenValPair(sLine,sTok,sVal))
            {
                if(!pNewTask->SetParam(sTok, sVal))
                {
                    string sError = "THIRDPARTY parse fail on " + sLine;
                    bError = true;               
                }
            }
        
        }
    }
    else
    {
        sError = "THIRDPARTY TASK creation FAIL";
        bError = true;
    }

    

    //let us know if we were successful
    if(!bError)
    {
        //keep track of this task with our pointer
        m_Transaction.SetTransactingTask(pNewTask);

        //necessary?
        //pNewTask->SetMissionFileReader(pMissionFileReader);

        //ask task about its priority - if(priority < 2) -> set to 2
        //only make tasks with max priority of 2
        if(pNewTask->GetPriority() < 2)
            pNewTask->SetPriority(2);

        //always make the initialstate ON
        pNewTask->SetParam("INITIALSTATE", "ON");
        m_Tasks.push_front(pNewTask);

        return true;
    }
    else
    {
        delete pNewTask;
        return OnTransactionError(sError);
    }
    
}

bool CHelmApp::InitialiseTransactors()
{
    //a CTransaction object is responsible for knowing about transactions
    m_Transaction.Initialise();
    
    //FORMAT is
    //ALLOW = JOBNAME@CLIENT:TASK1,TASK2|SessionTimeOut=value 
    STRING_LIST Params;
    PERMISSIONS_MAP PermissionsMap;
    SESSION_TIMEOUT_MAP SessionTimeOutMap;

    if(m_MissionReader.GetConfiguration(m_sAppName,Params))
    {
        
        STRING_LIST::iterator p;
        for(p=Params.begin();p!=Params.end();p++)
        {
            string sParam    = *p;
            //get rid of the whitespace
            MOOSRemoveChars(sParam," ");

            string sWhat = MOOSChomp(sParam,"=");
            MOOSToUpper(sWhat);
            
            if(MOOSStrCmp(sWhat,"ALLOW"))
            {
                //Tasks that can be fired by the JOB@CLIENT
                string sClientInfo = MOOSChomp(sParam,"|");
                string sWho = MOOSChomp(sClientInfo,":");
                MOOSToUpper(sWho);
                
                STRING_LIST    Tasks;

                while(!sClientInfo.empty())
                {
                    string sUpperParam = MOOSChomp(sClientInfo,",");
                    MOOSToUpper(sUpperParam);
                    Tasks.push_front(sUpperParam);
                }
                
                PermissionsMap[sWho] = Tasks;
                
                //SessionTimeOuts for these particular Tasks
                string sVar = MOOSChomp(sParam, "=");
                MOOSToUpper(sVar);
                if(MOOSStrCmp(sVar, "SESSIONTIMEOUT"))
                {
                    double dfVal = atof(sParam.c_str());
                    if((dfVal > 0) && (dfVal < MAX_SESSION_TIMEOUT))
                    {
                        SessionTimeOutMap[sWho] = dfVal;
                    }
                    else if((dfVal > 0) && (dfVal > MAX_SESSION_TIMEOUT))
                    {
                        SessionTimeOutMap[sWho] = MAX_SESSION_TIMEOUT;
                    }
                }
            }
            
        }

        //make sure the Transacation knows about these maps
        m_Transaction.SetPermissionsMap(PermissionsMap);
        m_Transaction.SetSessionTimeOutMap(SessionTimeOutMap);
    }

    //we initially are not talking to anyone
    m_Comms.Notify("CURRENT_THIRDPARTY", "NONE");

    return true;
}

bool CHelmApp::CTransaction::BuildParameterList(string sList, STRING_LIST &ParameterList)
{
    //remove the whitespace
    MOOSRemoveChars(sList," ");
    //sList has format:
    //LOCATION=0,0,0|RADIUS=5
    while(!sList.empty())
    {
        string sWhat = MOOSChomp(sList,"|");
        MOOSToUpper(sWhat);
        ParameterList.push_front(sWhat);
    }
    

    return true;
}


bool CHelmApp::CTransaction::HasAccess(string sClient)
{
    //although this method seems redundant, it is responsible for
    //being the first layer of security and prevents us from
    //opening a connection to a JOB that we should not be dealing
    //with in the first place

    MOOSToUpper(sClient);
    PERMISSIONS_MAP::iterator p = m_PermissionsMap.find(sClient);

    if(p != m_PermissionsMap.end())
    {
        return true;
    }
    else
        return false;
}


bool CHelmApp::CTransaction::IsOpen()
{
    return m_bTransactionIsOpen;
}

bool CHelmApp::CTransaction::IsTPTaskRunning()
{
    if(m_pTransactingTask != NULL)
        return m_pTransactingTask->ShouldRun();
    else
        return false;
}

bool CHelmApp::CTransaction::IsTPTaskTimeOutExpired()
{
    return ((MOOSTime() - GetTaskCompleteTime()) > GetSessionTimeOut());
}

bool CHelmApp::CTransaction::IsOneShot()
{
    return (m_dfSessionTimeOut == 0.0);
}


CHelmApp::CTransaction::CTransaction()
{
    
}

CHelmApp::CTransaction::~CTransaction()
{
    
}

bool CHelmApp::CTransaction::Initialise()
{
    SetTransactingClient("NONE");
    SetTransactionIsOpen(false);
    SetTransactingTask(NULL);
    SetSessionTimeOut(0.0);//assume OneShot 
    SetTaskCompleteTime(-1);

    return true;
}

void CHelmApp::CTransaction::SetTaskCompleteTime(double dfCompleteTime)
{
    m_dfTPTaskCompleteTime = dfCompleteTime;
}

void CHelmApp::CTransaction::SetTransactingClient(string sClient)
{
    m_sTransactingClient = sClient;

    if(m_sTransactingClient != "NONE")
        SetSessionTimeOut(m_SessionTimeOutMap[m_sTransactingClient]);
    else
        SetSessionTimeOut(0.0);
}

void CHelmApp::CTransaction::SetSessionTimeOut(double dfTimeOut)
{
    m_dfSessionTimeOut = dfTimeOut;
}

void CHelmApp::CTransaction::SetTransactingTask(CMOOSBehaviour *pNewTask)
{
    m_pTransactingTask = pNewTask;
}

void CHelmApp::CTransaction::SetTransactionIsOpen(bool bOpen)
{
    m_bTransactionIsOpen = bOpen;
}

CMOOSBehaviour * CHelmApp::CTransaction::GetTransactingTask()
{
    return m_pTransactingTask;
}

double CHelmApp::CTransaction::GetSessionTimeOut()
{
    return m_dfSessionTimeOut;
}

string CHelmApp::CTransaction::GetTransactingClient()
{
    return m_sTransactingClient;
}

double CHelmApp::CTransaction::GetTaskCompleteTime()
{
    return m_dfTPTaskCompleteTime;
}

bool CHelmApp::CTransaction::ParseInstruction(string &sInstruction, string &sClient, string &sTaskName)
{
    //need a Transaction object that stores all this stuff once
    //FORMAT is
    //JOB@CLIENT:TASK=TASKNAME|Task Parameter 1|Task Parameter 2
    string sBeginning = MOOSChomp(sInstruction,"|");
    
    sClient    = MOOSChomp(sBeginning,":");
    
    MOOSChomp(sBeginning,"=");
    
    sTaskName = sBeginning;

    return true;
}

void CHelmApp::CTransaction::SetPermissionsMap(PERMISSIONS_MAP PermissionsMap)
{
    m_PermissionsMap = PermissionsMap;
}

void CHelmApp::CTransaction::SetSessionTimeOutMap(SESSION_TIMEOUT_MAP SessionMap)
{
    m_SessionTimeOutMap = SessionMap;
}
