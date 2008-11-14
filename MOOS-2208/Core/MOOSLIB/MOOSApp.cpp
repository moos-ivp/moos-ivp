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

// MOOSApp.cpp: implementation of the CMOOSApp class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include "MOOSGlobalHelper.h"
#include "MOOSApp.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <iterator>

using namespace std;

// predicate for sorting on time
bool MOOSMsgTimeSorter(const CMOOSMsg  &M1, const CMOOSMsg &M2) 
{
    return (M1.GetTime() < M2.GetTime());
}

//////////////////////////////////////////////////
//  these are file-scope methods which allow
//  redirection of a call back into the CMOOSApp Class
//  they don't concern the average user...
///////////////////////////////////////////////////
bool MOOSAPP_OnConnect(void * pParam)
{
    if(pParam!=NULL)
    {
        CMOOSApp* pApp = (CMOOSApp*)pParam;

        //we may have private work to do
        pApp->OnConnectToServerPrivate();

        //client work
        return pApp->OnConnectToServer();

    }
    return false;
}

bool MOOSAPP_OnDisconnect(void * pParam)
{
    if(pParam!=NULL)
    {
        CMOOSApp* pApp = (CMOOSApp*)pParam;

        //private work
        pApp->OnDisconnectToServerPrivate();

        //client work
        return pApp->OnDisconnectFromServer();

    }
    return false;
}


bool MOOSAPP_OnMail(void *pParam)
{
    if(pParam!=NULL)
    {
        CMOOSApp* pApp = (CMOOSApp*)pParam;
        
        //client mail work
        return pApp->OnMailCallBack();
        
    }
    return false;
    
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMOOSApp::CMOOSApp()
{
    m_dfFreq=DEFAULT_MOOS_APP_FREQ;
    m_nCommsFreq=DEFAULT_MOOS_APP_COMMS_FREQ;
    m_nIterateCount = 0;
    m_nMailCount = 0;
    m_bServerSet = false;
    m_dfAppStartTime = -1;
    m_bDebug = false;
    m_bSimMode = false;
    m_bUseMOOSComms = true;
    m_dfLastRunTime = -1;
    m_bCommandMessageFiltering = false;
    m_dfLastStatusTime = -1;
    m_bSortMailByTime = true;
	m_bAppError = false;
    EnableIterateWithoutComms(false);
}

CMOOSApp::~CMOOSApp()
{

}

//this is an overloaded 3 parameter version which allows explicit setting of the registration name
bool CMOOSApp::Run(const char * sName,const char * sMissionFile,const char * sMOOSName)
{
    //fill in specialised MOOSName
    m_sMOOSName = sMOOSName;
    return Run(sName,sMissionFile);
}

//the main MOOSApp Run function
bool CMOOSApp::Run( const char * sName,
                    const char * sMissionFile)
{

    //save absolutely crucial info...
    m_sAppName      = sName;
    m_sMissionFile  = sMissionFile;
    m_MissionReader.SetAppName(m_sAppName);
    
    //by default we will 
	if(m_sMOOSName.empty())
        m_sMOOSName=m_sAppName;
    

    //can we see the mission file
    if(sMissionFile!=NULL)
    {
        if(!m_MissionReader.SetFile(m_sMissionFile.c_str()))
        {
            MOOSTrace("Warning Mission File \"%s\" not found...\n",m_sMissionFile.c_str());
        }
        else
        {
			//what is the global time warp
            double dfTimeWarp = 1.0;
            if(m_MissionReader.GetValue("MOOSTimeWarp", dfTimeWarp))
            {
                SetMOOSTimeWarp(dfTimeWarp);
            }
            
            
            //are we expected to use MOOS comms?
            m_MissionReader.GetConfigurationParam("UseMOOSComms",m_bUseMOOSComms);
            
            //are we being asked to sort mail by time..
            m_MissionReader.GetConfigurationParam("SortMailByTime",m_bSortMailByTime);
            
            //are we in debug mode
            m_MissionReader.GetConfigurationParam("DEBUG",m_bDebug);

            //are we in simulator mode?
            string sSim;
            if(m_MissionReader.GetValue("SIMULATOR",sSim))
            {
                m_bSimMode = MOOSStrCmp(sSim,"TRUE");
            }

            //are we in playback mode
            string sPlayBack;
            if(m_MissionReader.GetValue("PLAYBACK",sPlayBack))
            {
                SetMOOSPlayBack(MOOSStrCmp(sPlayBack,"TRUE"));
            }

            //OK now figure out our tick speeds  above what is set by default
            //in derived class constructors this can be set in the process config block
            //by the mission architect
            m_MissionReader.GetConfigurationParam("APPTICK",m_dfFreq);
            

            //do we want to enable command filtering (default is set in constructor)
            m_MissionReader.GetConfigurationParam("CatchCommandMessages",m_bCommandMessageFiltering);
        }
    }

    //what time did we start?
    m_dfAppStartTime = MOOSTime();
    
    //can we start the communications ?
    if(m_bUseMOOSComms)
    {
        if(!ConfigureComms())
        {
            return false;
        }

        ///////////////////////////////////////////////////////////////
        //OK we are going to wait for a conenction to be established
        // this is a little harsh but it saves derived classes having to
        // hold off connecting to the server until ready
        // but we will only hang around for 1 second...
        // so it is possible that notifies will fail...but very unlikely
        // note this is not a hack! just being helpful. Ths success of an
        // application is NOT dependent on this
        int t = 0;
        int dT = 50;
        while(!m_Comms.IsConnected())
        {
            MOOSPause(dT);
            t+=dT;
            if(t>5000)
                break;
        }
        //give iostream time to write comms start details up to screen..this is not really necessary
        //as the code is thread safe...it is aesthetic only
        MOOSPause(500);
    }



    /** let derivatives do stuff before execution*/
    if(!OnStartUp())
    {
        MOOSTrace("Derived OnStartUp() returned false... Quitting\n");
        return false;
    }


    MOOSTrace("%s is Running:\n",GetAppName().c_str());
    MOOSTrace("\t AppTick   @ %.1f Hz\n",m_dfFreq);
    MOOSTrace("\t CommsTick @ %d Hz\n",m_nCommsFreq);
    if(GetMOOSTimeWarp()!=1.0)
    	MOOSTrace("\t Time Warp @ %.1f \n",GetMOOSTimeWarp());


    /****************************  THE MAIN MOOS APP LOOP **********************************/

    while(1)
    {
        if(!m_Comms.HasMailCallBack())
        	DoRunWork();
        else
            MOOSPause(1000);
                
    }

    /***************************   END OF MOOS APP LOOP ***************************************/

    return true;
}

bool CMOOSApp::DoRunWork()
{
    //look for mail
    double dfT1 = MOOSLocalTime();
    //local vars
    MOOSMSG_LIST MailIn;
    if(m_bUseMOOSComms)
    {
        if( m_Comms.Fetch(MailIn))
        {
            /////////////////////////////
            //   process mail
            if(m_bSortMailByTime)
                MailIn.sort(MOOSMsgTimeSorter);
            
            
            //call our own private version
            OnNewMailPrivate(MailIn);
            
            //classes will have their own personal versions of this
            OnNewMail(MailIn);
            
            m_nMailCount++;
        }
        
        if(m_Comms.IsConnected() ||  CanIterateWithoutComms() )
        {
            //do private work
            IteratePrivate();
            
            //////////////////////////////////////
            //  do application specific processing
            Iterate();
            
            m_nIterateCount++;
        }
    }
    else
    {
        //do private work
        IteratePrivate();
        
        /////////////////////////////////////////
        //  do application specific processing
        Iterate();
        m_nIterateCount++;
    }
    
    //store for derived class use the last time iterate was called;
    m_dfLastRunTime = MOOSLocalTime();
    
    //sleep
    if(m_dfFreq>0)
    {
        int nElapsedTime_ms  = static_cast<int> (1000.0*(m_dfLastRunTime-dfT1));
		int nRequiredWait_ms = static_cast<int> (1000.0/m_dfFreq);

		if (nElapsedTime_ms < 0) nElapsedTime_ms = 0;
		
		int nSleep = (nRequiredWait_ms - nElapsedTime_ms);

        //a 10 ms sleep is a good as you are likely to get, if we are being told to sleep less than this we may as well
        //tick once more and let the OS schedule us appropriately
        if(nSleep>10 && !m_Comms.HasMailCallBack())
        {
            MOOSPause(nSleep);
        }
    }
    
    return true;
    
}



void CMOOSApp::SetServer(const char *sServerHost, long lPort)
{
    m_sServerHost = sServerHost;
    m_lServerPort = lPort;
    m_bServerSet = true;
}

bool CMOOSApp::CheckSetUp()
{
    if(m_sServerHost.empty())
    {
        MOOSTrace("MOOS Server host not specified\n");
        return false;
    }

    if(m_lServerPort==0)
    {
        MOOSTrace("MOOS Server port not specified\n");
        return false;
    }

    return true;
}


void CMOOSApp::SetAppError(bool bErr, const std::string & sErr)
{
    m_bAppError = bErr;
    m_sAppError =  m_bAppError ? sErr : "";
}


/////////////////// EXPERIMENTAL July 2008 ////////////////////
bool CMOOSApp::UseMailCallBack()
{
    /* by calling this function Iterate and OnNewMail will be
     called from the thread that is servicing the MOOS Comms client. It
     is provided to let really very specialised MOOSApps have very speedy
     response times. It is not recommended for general use*/
    m_Comms.SetOnMailCallBack(MOOSAPP_OnMail,this);
    return true;
}

////////////////////// DEFAULT HANDLERS //////////////////////
bool CMOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
    return true;
}

bool CMOOSApp::Iterate()
{
    if(m_nIterateCount==0)
    {
        MOOSTrace("Warning default Iterate handler invoked...doing nothing\n");
    }
    return true;
}

bool CMOOSApp::OnConnectToServer()
{
    MOOSTrace("- default OnConnectToServer called\n");
    return true;
}


bool CMOOSApp::OnDisconnectFromServer()
{
    MOOSTrace("- default OnDisconnectFromServer called\n");
    return true;
}

/** this is a call back from MOOSComms and its use is specialised (not for general consumption)*/
bool CMOOSApp::OnMailCallBack()
{
    return DoRunWork();
}

bool CMOOSApp::OnCommandMsg(CMOOSMsg  CmdMsg)
{
    MOOSTrace("- default OnCommandMsg called\n");
    return true;
}


bool CMOOSApp::ConfigureComms()
{



    if(!m_MissionReader.GetValue("SERVERHOST",m_sServerHost))
    {
        MOOSTrace("Warning Server host not read from mission file: assuming LOCALHOST\n");
        m_sServerHost = "LOCALHOST";
    }


    if(!m_MissionReader.GetValue("SERVERPORT",m_sServerPort))
    {
        MOOSTrace("Warning Server port not read from mission file: assuming 9000\n");
        m_sServerPort = "9000";
    }

    m_lServerPort = atoi(m_sServerPort.c_str());

    if(m_lServerPort==0)
    {
        m_lServerPort = 9000;
        MOOSTrace("Warning Server port not read from mission file: assuming 9000\n");
    }

    if(!CheckSetUp())
        return false;


    //OK now figure out our speeds etc above what is set by default
    //in derived class constructors
    m_MissionReader.GetConfigurationParam("COMMSTICK",m_nCommsFreq);
    m_nCommsFreq = m_nCommsFreq <0 ? 1 : m_nCommsFreq;

    //register a callback for On Connect
    m_Comms.SetOnConnectCallBack(MOOSAPP_OnConnect,this);
    
    //and one for the disconnect callback
    m_Comms.SetOnDisconnectCallBack(MOOSAPP_OnDisconnect,this);

    //start the comms client....
    if(m_sMOOSName.empty())
        m_sMOOSName = m_sAppName;
    
    m_Comms.Run(m_sServerHost.c_str(),m_lServerPort,m_sMOOSName.c_str(),m_nCommsFreq);

    return true;
}

/** over load this if you want to do something fancy at statup...*/
bool CMOOSApp::OnStartUp()
{
    return true;
}

int CMOOSApp::GetIterateCount()
{
    return m_nIterateCount;
}

double CMOOSApp::GetAppStartTime()
{
    return m_dfAppStartTime;
}

void CMOOSApp::SetAppFreq(double  dfFreq)
{
    if(m_dfFreq<=MOOS_MAX_APP_FREQ)
    {
        m_dfFreq = dfFreq;
    }
}

bool CMOOSApp::SetCommsFreq(unsigned int nFreq)
{
    if(nFreq<=MOOS_MAX_COMMS_FREQ)
    {
        m_nCommsFreq = nFreq;
        return m_Comms.SetCommsTick(m_nCommsFreq);
    }
    return false;
}

bool CMOOSApp::IsSimulateMode()
{
    return m_bSimMode;
}

bool CMOOSApp::AddMOOSVariable(string sName, string sSubscribeName, string sPublishName,double dfCommsTime)
{
    CMOOSVariable NewVar(sName,sSubscribeName,sPublishName,dfCommsTime);

    //does it already exist?
    if(m_MOOSVars.find(sName)!=m_MOOSVars.end())
        return false;

    m_MOOSVars[sName] = NewVar;

    return true;
}

//this function publishes all the fresh variables belong to the
//apllication. Useful in many sensor applications!
bool CMOOSApp::PublishFreshMOOSVariables()
{
    MOOSVARMAP::iterator p;

    for(p = m_MOOSVars.begin();p!=m_MOOSVars.end();p++)
    {
        CMOOSVariable & Var = p->second;
        if(Var.IsFresh())
        {
            if(Var.IsDouble())
            {
                m_Comms.Notify(Var.GetPublishName(),Var.GetDoubleVal(),Var.GetTime());
            }
            else
            {
                m_Comms.Notify(Var.GetPublishName(),Var.GetStringVal(),Var.GetTime());
            }

            Var.SetFresh(false);
        }
    }

    return true;
}

bool CMOOSApp::SetMOOSVar(const string &sVarName, double dfVal, double dfTime)
{
    MOOSVARMAP::iterator p = m_MOOSVars.find(sVarName);

    if(p==m_MOOSVars.end())
        return false;

    CMOOSVariable & rVar = p->second;

    return rVar.Set(dfVal,dfTime);
}

bool CMOOSApp::SetMOOSVar(const string &sVarName,const  string &sVal, double dfTime)
{
    MOOSVARMAP::iterator p = m_MOOSVars.find(sVarName);

    if(p==m_MOOSVars.end())
        return false;

    CMOOSVariable  & rVar = p->second;

    return rVar.Set(sVal,dfTime);


}

bool CMOOSApp::UpdateMOOSVariables(MOOSMSG_LIST &NewMail)
{
    //we only subscribe to things if we are in simulator mode
    MOOSVARMAP::iterator p;
    double dfTimeNow = MOOSTime();
    for(p = m_MOOSVars.begin();p!=m_MOOSVars.end();p++)
    {
        CMOOSVariable & rVar = p->second;
        CMOOSMsg Msg;
        if(m_Comms.PeekMail(NewMail,rVar.GetSubscribeName(),Msg))
        {
            if(!Msg.IsSkewed(dfTimeNow))
            {
                rVar.Set(Msg);
                rVar.SetFresh(true);
            }
        }
    }

    return true;
}

bool CMOOSApp::RegisterMOOSVariables()
{
    bool bSuccess = true;

    MOOSVARMAP::iterator p;

    for(p = m_MOOSVars.begin();p!=m_MOOSVars.end();p++)
    {
        CMOOSVariable & rVar = p->second;
        if(!rVar.GetSubscribeName().empty())
        {
            double dfCommsTime = rVar.GetCommsTime();
            if(dfCommsTime<0)
            {
                dfCommsTime = 0;
            }

            bSuccess &= m_Comms.Register(rVar.GetSubscribeName(),dfCommsTime);
        }
    }

    return bSuccess;
}

bool CMOOSApp::MOOSDebugWrite(const string &sTxt)
{
    if(m_Comms.IsConnected())
    {
        MOOSTrace(" %s says: %s\n",GetAppName().c_str(),sTxt.c_str());
        return m_Comms.Notify("MOOS_DEBUG",sTxt);
    }
    else
    {
        return false;
    }
}

bool CMOOSApp::CanIterateWithoutComms()
{
    return m_bIterateWithoutComms;
}

void CMOOSApp::EnableIterateWithoutComms(bool bEnable)
{
    m_bIterateWithoutComms = bEnable;
}



double CMOOSApp::GetTimeSinceIterate()
{
    return MOOSLocalTime()-m_dfLastRunTime;
}

double CMOOSApp::GetLastIterateTime()
{
    return m_dfLastRunTime;
}


std::string CMOOSApp::GetMissionFileName()
{
    return m_sMissionFile;
}

string CMOOSApp::GetAppName()
{
    return m_sAppName;
}

bool CMOOSApp::UseMOOSComms(bool bUse)
{
    m_bUseMOOSComms = bUse;

    return true;
}

CMOOSVariable * CMOOSApp::GetMOOSVar(string sName)
{
    MOOSVARMAP::iterator p =  m_MOOSVars.find(sName);
    if(p==m_MOOSVars.end())
    {
        return NULL;
    }
    else
    {
        return &(p->second);
    }
}


std::string CMOOSApp::GetCommandKey()
{
    std::string sCommandKey = GetAppName()+"_CMD";
    MOOSToUpper(sCommandKey);
    return sCommandKey;
}

bool CMOOSApp::LookForAndHandleAppCommand(MOOSMSG_LIST & NewMail)
{
    MOOSMSG_LIST::iterator q;
    bool bResult = true;
    for(q=NewMail.begin();q!=NewMail.end();q++)
    {
        if(MOOSStrCmp(q->GetKey(),GetCommandKey()))
        {
            //give a derived class a chance to respond
            bResult&= OnCommandMsg(*q);
        }
    }
    return bResult;
}


void CMOOSApp::EnableCommandMessageFiltering(bool bEnable)
{
    m_bCommandMessageFiltering = bEnable;
    if(bEnable)
    {
        //we had better register for the message
        m_Comms.Register(GetCommandKey(),0);
    }
    else
    {
        //we are no longer interested
        m_Comms.UnRegister(GetCommandKey());
    }
}


std::string CMOOSApp::MakeStatusString()
{
    std::set<std::string> Published = m_Comms.GetPublished();
    std::set<std::string> Registered = m_Comms.GetRegistered();

    std::stringstream ssStatus;
    ssStatus<<"AppErrorFlag="<<(m_bAppError?"true":"false")<<",";
    if(m_bAppError)
        ssStatus<<"AppErrorReason="<<m_sAppError<<",";
    
    ssStatus<<"Uptime="<<MOOSTime()-GetAppStartTime()<<",";

    ssStatus<<"MOOSName="<<GetAppName()<<",";

    ssStatus<<"Publishing=\"";
    std::copy(Published.begin(),Published.end(),std::ostream_iterator<string>(ssStatus,","));
	ssStatus<<"\"";

    ssStatus<<"Subscribing=\"";
    std::copy(Registered.begin(),Registered.end(),std::ostream_iterator<string>(ssStatus,","));
	ssStatus<<"\"";

    return ssStatus.str();
}

void CMOOSApp::OnDisconnectToServerPrivate()
{
    return;
}

//called just before calling a derived classes OnConnectToServer()
void CMOOSApp::OnConnectToServerPrivate()
{
    if(m_bCommandMessageFiltering)
    {
        m_Comms.Register(GetCommandKey(),0);
    }
}

/** here we do our private mail processing*/
void CMOOSApp::OnNewMailPrivate(MOOSMSG_LIST & Mail)
{
    //look to handle a command string
    if(m_bCommandMessageFiltering)
        LookForAndHandleAppCommand(Mail);
}

void CMOOSApp::IteratePrivate()
{
    if(fabs(m_dfLastStatusTime-MOOSTime())>STATUS_PERIOD)
    {
        std::string sStatus = GetAppName()+"_STATUS";
        MOOSToUpper(sStatus);
        m_Comms.Notify(sStatus,MakeStatusString());
        m_dfLastStatusTime = MOOSTime();
    }
}
