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
#include <iostream>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool MOOSAPP_OnConnect(void * pParam)
{
    if(pParam!=NULL)
    {
		CMOOSApp* pApp = (CMOOSApp*)pParam;
		
		return pApp->OnConnectToServer();
		
    }
    return false;
}

bool MOOSAPP_OnDisconnect(void * pParam)
{
    if(pParam!=NULL)
    {
		CMOOSApp* pApp = (CMOOSApp*)pParam;
		
		return pApp->OnDisconnectFromServer();
		
    }
    return false;
}


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
}

CMOOSApp::~CMOOSApp()
{
	
}

bool CMOOSApp::Run( char * sName,
				   char * sMissionFile)
{
	
    //save absolutely crucial info...	
    m_sAppName      = sName;
    m_sMissionFile	= sMissionFile;
    m_MissionReader.SetAppName(m_sAppName);
	
    //waht time did we start?
    m_dfAppStartTime = MOOSTime();
	
    //can we see the mission file
    if(sMissionFile!=NULL)
    {
        if(!m_MissionReader.SetFile(m_sMissionFile.c_str()))
        {
            MOOSTrace("Warning Mission File \"%s\" not found...\n",m_sMissionFile.c_str());
        }
    }
	
	//are we expected to use MOOS comms?
    m_MissionReader.GetConfigurationParam("UseMOOSComms",m_bUseMOOSComms);
	
    //are we in debug mode
    string sDebug;
    if(m_MissionReader.GetConfigurationParam(m_sAppName,"DEBUG",sDebug))
    {
        m_bDebug = MOOSStrCmp(sDebug,"TRUE");
    }
    
	
    //are we in simulator mode
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
    if(!m_MissionReader.GetConfigurationParam(m_sAppName,"APPTICK",m_dfFreq))
    {
        //MOOSTrace("Assuming default Application frequency of %f Hz\n",m_dfFreq);
    }
	
	
	
    //can we start the communications ?
    if(m_bUseMOOSComms)
	{
		if(!ConfigureComms())
		{
			return false;
		}
    
		///////////////////////////////////////////////////////////////
		//OK we are going to wait for a conenction to be established
		// this is a littel harsh but it saves derived classes having to
		// hold off connecting to the server until ready
		// but we will only hang around for 1 second...
		// so it is possible that notifies will fail...but very unlikely
		// note this is not a hack! just being helpful. Ths success of an
		// application is NOT dependent on this
		int t = 0; int dT = 50;
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
        MOOSTrace("derived OnStartUp() returned false..quiting\n");
        return false;
    }
	
	
    MOOSTrace("%s AppTick   @ %.1f Hz\n",GetAppName().c_str(),m_dfFreq);
    MOOSTrace("%s CommsTick @ %d Hz\n",GetAppName().c_str(),m_nCommsFreq);
    MOOSTrace("%s is Running\n",GetAppName().c_str());
	
	
    /****************************  THE MAIN MOOS APP LOOP **********************************/
	
    //local vars
    MOOSMSG_LIST MailIn;
	
    while(1)
    {
        //look for mail
		double dfT1 = MOOSTime();
		
		if(m_bUseMOOSComms)
		{
			if(m_Comms.Fetch(MailIn))
			{
				//process mail
				OnNewMail(MailIn);
				m_nMailCount++;
			}
			
			if(m_Comms.IsConnected())
			{
				//do application specific processing
				Iterate();
				m_nIterateCount++;
			}
		}
		else
		{
			//do application specific processing
			Iterate();
			m_nIterateCount++;
		}

        //store for derived class use the last time iterate was called;
        m_dfLastRunTime = HPMOOSTime();

        //sleep
        if(m_dfFreq>0)
        {			
			int nSleep = (int)(1000.0/m_dfFreq-1000*(m_dfLastRunTime-dfT1));
			
            if(nSleep>10)
            {
                MOOSPause(nSleep);
            }
        }
    }
	
    /***************************   END OF MOOS APP LOOP ***************************************/
	
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
    return true;
}


bool CMOOSApp::OnDisconnectFromServer()
{
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
    string sCommsTick;
    if(m_MissionReader.GetConfigurationParam(m_sAppName,"COMMSTICK",sCommsTick))
    {
		unsigned int nTmp = atoi(sCommsTick.c_str());
		if(nTmp!=0)
		{
			m_nCommsFreq = nTmp;
		}
    }
	
	
    
	
    //register a callback for On Connect
    m_Comms.SetOnConnectCallBack(MOOSAPP_OnConnect,this);
	
    //start the comms client....
    m_Comms.Run(m_sServerHost.c_str(),m_lServerPort,m_sAppName.c_str(),m_nCommsFreq);
	
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

void CMOOSApp::SetCommsFreq(unsigned int nFreq)
{
    if(nFreq<=MOOS_MAX_COMMS_FREQ)
    {
		m_nCommsFreq = nFreq;
    }
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
            else
            {
                //MOOSTrace("UpdateMOOSVariables() SKEWED: My Time = %f,MsgTime = %f\n",dfTimeNow,Msg.m_dfTime);
                //Msg.Trace();
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

double CMOOSApp::GetTimeSinceIterate()
{
    return MOOSTime()-m_dfLastRunTime;
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
