/**
///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of 
//   Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and 
//   the University of Oxford 2003-2013 
//   
//   email: pnewman@robots.ox.ac.uk. 
//              
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/



// MOOSApp.cpp: implementation of the CMOOSApp class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif



#include "MOOS/libMOOS/Utils/MOOSPlaybackStatus.h"
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/MOOSVersion.h"
#include "MOOS/libMOOS/GitVersion.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <iterator>


#ifdef ASYNCHRONOUS_CLIENT
#include "MOOS/libMOOS/Thirdparty/PocoBits/Event.h"
#endif



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

bool MOOSAPP_OnMessage(CMOOSMsg & M, void *pParam)
{

    if(pParam!=NULL)
    {
        CMOOSApp* pApp = (CMOOSApp*)pParam;

        //client mail work
        return pApp->OnMessage(M);

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
    m_dfMaxAppTick = DEFAULT_MOOS_APP_FREQ; //we can respond to mail very quickly but by default we are cautious
    m_IterationMode = REGULAR_ITERATE_AND_MAIL;
    m_nIterateCount = 0;
    m_nMailCount = 0;
    m_bServerSet = false;
    m_dfAppStartTime = -1;
    m_bDebug = false;
    m_bSimMode = false;
    m_bQuiet = false;
    m_bUseMOOSComms = true;
    m_dfLastRunTime = -1;
    m_bCommandMessageFiltering = false;
    m_dfLastStatusTime = -1;
    m_bSortMailByTime = true;
	m_bAppError = false;
    m_bQuitOnIterateFail = false;
	m_bQuitRequested = false;
    
    SetMOOSTimeWarp(1.0);
    
#ifdef ASYNCHRONOUS_CLIENT
    m_pMailEvent = new MOOS::Poco::Event;
    UseMailCallBack();
#endif

    EnableIterateWithoutComms(false);
}

CMOOSApp::~CMOOSApp()
{
#ifdef ASYNCHRONOUS_CLIENT
    delete m_pMailEvent;
#endif
}



void CMOOSApp::OnPrintExampleAndExit()
{
	std::cout<<" tragically, there is no example configuration for this app\n";
	exit(0);
}

void CMOOSApp::OnPrintInterfaceAndExit()
{
	std::cout<<" sadly, there is no description of the interface for this app\n";
	exit(0);
}


void CMOOSApp::PrintDefaultCommandLineSwitches()
{
	std::cout<<MOOS::ConsoleColours::Yellow()<<"\n";
	std::cout<<"--------------------------------------------------\n";
	std::cout<<GetAppName()<<"'s standard MOOSApp switches are:\n";
	std::cout<<"--------------------------------------------------\n";
	std::cout<<MOOS::ConsoleColours::reset()<<"\n";
	std::cout<<"\nvariables:\n";
	std::cout<<"  --moos_app_name=<string>    : name of application\n";
	std::cout<<"  --moos_name=<string>        : name with which to register with MOOSDB\n";
	std::cout<<"  --moos_file=<string>        : name of configuration file\n";
	std::cout<<"  --moos_host=<string>        : address of machine hosting MOOSDB\n";
	std::cout<<"  --moos_port=<number>        : port on which DB is listening \n";
	std::cout<<"  --moos_app_tick=<number>    : frequency of application (if relevant) \n";
	std::cout<<"  --moos_max_app_tick=<number>: max frequency of application (if relevant) \n";
	std::cout<<"  --moos_comms_tick=<number>  : frequency of comms (if relevant) \n";
    std::cout<<"  --moos_tw_delay_factor=<num>: comms delay as % of time warp (if relevant) \n";



	std::cout<<"  --moos_iterate_Mode=<0,1,2> : set app iterate mode \n";
	std::cout<<"  --moos_time_warp=<number>   : set time warp \n";
    std::cout<<"  --moos_suicide_channel=<str>: suicide monitoring channel (IP address) \n";
    std::cout<<"  --moos_suicide_port=<int>   : suicide monitoring port  \n";
    std::cout<<"  --moos_suicide_phrase=<str> : suicide pass phrase  \n";

	std::cout<<"\nflags:\n";
	std::cout<<"  --moos_iterate_no_comms     : enable iterate without comms \n";
	std::cout<<"  --moos_filter_command       : enable command message filtering \n";
	std::cout<<"  --moos_no_sort_mail         : don't sort mail by time \n";
	std::cout<<"  --moos_no_comms             : don't start communications \n";
	std::cout<<"  --moos_quiet                : don't print banner information \n";
	std::cout<<"  --moos_quit_on_iterate_fail : quit if iterate fails \n";
	std::cout<<"  --moos_no_colour            : disable colour printing \n";
    std::cout<<"  --moos_suicide_disable      : disable suicide monitoring \n";
    std::cout<<"  --moos_suicide_print        : print suicide conditions \n";



	std::cout<<"\nhelp:\n";
	std::cout<<"  --moos_print_example        : print an example configuration block \n";
	std::cout<<"  --moos_print_interface      : describe the interface (subscriptions/pubs) \n";
	std::cout<<"  --moos_print_version        : print the version of moos in play \n";
	std::cout<<"  --moos_help                 : print help on moos switches\n";
    std::cout<<"  --moos_configuration_audit  : print configuration terms searched for\n";
	std::cout<<"  --help                      : print help on moos switches and custom help\n";


}

void CMOOSApp::OnPrintVersionAndExit()
{
	std::cout<<"--------------------------------------------------\n";
	std::cout<<"MOOS version "<<MOOS_VERSION_NUMBER<<"\n";
	std::cout<<"Built on "<<__DATE__<<" at "<<__TIME__<<"\n";
	std::cout<<MOOS_GIT_VERSION<<"\n";
	std::cout<<"--------------------------------------------------\n";
	exit(0);
}


void CMOOSApp::OnPrintHelpAndExit()
{
	std::cout<<" distressingly, there is no custom help for this app\n";
	exit(0);
}


//this is an overloaded 3 parameter version which allows explicit setting of the registration name
bool CMOOSApp::Run(const std::string & sName,const std::string & sMissionFile,const std::string & sMOOSName)
{
    //fill in specialised MOOSName
    m_sMOOSName = sMOOSName;
    return Run(sName,sMissionFile);
}

//this is an overloaded 3 parameter version which allows explicit setting of the registration name
bool CMOOSApp::Run(const std::string &  sName,int argc,  char * argv[])
{
	SetCommandLineParameters(argc,argv);
	return Run(sName);
}

bool  CMOOSApp::Run(const std::string &  sName,const std::string & sMissionFile, int argc,  char * argv[])
{
	SetCommandLineParameters(argc,argv);
	return Run(sName,sMissionFile);
}

//the main MOOSApp Run function
bool CMOOSApp::Run( const std::string & sName,
                    const std::string & sMissionFile)
{

	//straight away do we want colour in this application?
	if(m_CommandLineParser.GetFlag("--moos_no_colour"))
	{
		//std::cerr<<"turning off colour\n";
		MOOS::ConsoleColours::Enable(false);
	}


	//save absolutely crucial info...
	m_sAppName      = sName; //default
	m_CommandLineParser.GetOption("--moos_app_name",m_sAppName);//overload

	//but things might be overloaded
	m_sMissionFile  = sMissionFile; //default
	m_CommandLineParser.GetVariable("--moos_file",m_sMissionFile); //overload

	m_MissionReader.SetAppName(m_sAppName);

	//by default we will register with our application name
	if(m_sMOOSName.empty()) //default
		m_sMOOSName=m_sAppName;

	m_CommandLineParser.GetVariable("--moos_name",m_sMOOSName); //overload


	if(m_CommandLineParser.GetFlag("--moos_help"))
	{
		PrintDefaultCommandLineSwitches();
		exit(0);
	}

	if(m_CommandLineParser.GetFlag("--help"))
	{
		PrintDefaultCommandLineSwitches();
		std::cout<<"\ncustom help:\n";
		OnPrintHelpAndExit();
		exit(0);
	}

	if(m_CommandLineParser.GetFlag("--moos_print_example"))
		OnPrintExampleAndExit();

	if(m_CommandLineParser.GetFlag("--moos_print_interface"))
		OnPrintInterfaceAndExit();

	if(m_CommandLineParser.GetFlag("--moos_print_version"))
		OnPrintVersionAndExit();




	//look at mission file etc
	if(!Configure())
	{
		std::cerr<<"configure returned false. Quitting\n";
		return false;
	}

	//here we give users a chance to alter configurations
	//or do more work in configuring
	if(m_CommandLineParser.IsAvailable())
		OnProcessCommandLine();
    
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
    if(!OnStartUpPrepare())
    {
        MOOSTrace("Derived OnStartUpPrepare() returned false... Quitting\n");
        return false;
    }

    if(!OnStartUp())
    {
        MOOSTrace("Derived OnStartUp() returned false... Quitting\n");
        return false;
    }


    if(!OnStartUpComplete())
    {
        MOOSTrace("Derived OnStartUpComplete() returned false... Quitting\n");
        return false;
    }

    if(m_CommandLineParser.GetFlag("--moos_configuration_audit"))
    {
        PrintSearchedConfigurationFileParameters();
        return true;
    }


    DoBanner();




    /****************************  THE MAIN MOOS APP LOOP **********************************/

    while(!m_bQuitRequested)
    {
		bool bOK = DoRunWork();

		if(m_bQuitOnIterateFail && !bOK)
			return MOOSFail("MOOSApp Exiting as requested");


    }

    /***************************   END OF MOOS APP LOOP ***************************************/

    m_Comms.Close();

    return true;
}


bool CMOOSApp::GetFlagFromCommandLineOrConfigurationFile(std::string sOption,bool bPrependMinusMinusForCommandLine)
{
    bool bC,bF,bFlag;
    bF = m_MissionReader.GetConfigurationParam(sOption,bFlag);

    if(bPrependMinusMinusForCommandLine)
        sOption = "--"+sOption;

    bC = m_CommandLineParser.GetFlag(sOption);
    if(bC)
    {
        return true;
    }
    if(bF)
    {
        //std::cerr<<"found in config file:"<<sOption<<"\n";
        return bFlag;
    }
    return false;
}



bool CMOOSApp::Configure()
{

    //are we being asked to be quiet?
    SetQuiet(GetFlagFromCommandLineOrConfigurationFile("moos_quiet") || m_bQuiet);

	//can we see the mission file?
	if(!m_MissionReader.SetFile(m_sMissionFile.c_str()))
	{
	    if(!m_bQuiet)
	    {
            std::cerr<<MOOS::ConsoleColours::yellow();
            std::cerr<<"note mission file "<<m_sMissionFile<<" was not found\n";
            std::cerr<<MOOS::ConsoleColours::reset();
	    }
	}

	//are we being asked to iterate without comms
    EnableIterateWithoutComms(GetFlagFromCommandLineOrConfigurationFile("moos_iterate_no_comms"));

    //are we having our suicide address being set?
    std::string sSuicideAddress;
    if(GetParameterFromCommandLineOrConfigurationFile("moos_suicide_channel",sSuicideAddress))
    {
        m_SuicidalSleeper.SetChannel(sSuicideAddress);
    }

    //are we having our suicide port being set?
    int nSuicidePort=-1;
    if(GetParameterFromCommandLineOrConfigurationFile("moos_suicide_port",nSuicidePort))
    {
        m_SuicidalSleeper.SetPort(nSuicidePort);
    }

    //are we having our suicide phrase being set?
    std::string sSuicidePhrase;
    if(GetParameterFromCommandLineOrConfigurationFile("moos_suicide_phrase",sSuicidePhrase))
    {
        m_SuicidalSleeper.SetPassPhrase(sSuicidePhrase);
    }

    //are we being told to print out how to get us to commit suicide?
    if(GetFlagFromCommandLineOrConfigurationFile("moos_suicide_print"))
    {
        std::cerr<<"suicide terms and conditions are:\n";
        std::cerr<<" channel  "<<m_SuicidalSleeper.GetChannel()<<"\n";
        std::cerr<<" port     "<<m_SuicidalSleeper.GetPort()<<"\n";
        std::cerr<<" phrase   \""<<m_SuicidalSleeper.GetPassPhrase()<<"\"\n";
    }

    //are we being told to disable suicide?
    if(!GetFlagFromCommandLineOrConfigurationFile("moos_suicide_disable"))
    {

        //no - then allow it....
        m_SuicidalSleeper.SetName(GetAppName());
        m_SuicidalSleeper.Run();
    }


	//what is the global time warp
	double dfTimeWarp = 1.0;

    if(GetParameterFromCommandLineOrConfigurationFile("moos_time_warp",dfTimeWarp) ||
            m_MissionReader.GetValue("MOOSTimeWarp", dfTimeWarp))
	{
		SetMOOSTimeWarp(dfTimeWarp);
	}

    double dfTimeWarpCommsFactor = 0.0;
    if(GetParameterFromCommandLineOrConfigurationFile("moos_tw_delay_factor",dfTimeWarpCommsFactor))
    {
        if(!m_Comms.SetCommsControlTimeWarpScaleFactor(dfTimeWarpCommsFactor))
        {
            std::cerr<<MOOS::ConsoleColours::Red();
            std::cerr<<"ERROR: failed to set moos_tw_delay_factor\n";
            std::cerr<<MOOS::ConsoleColours::reset();
        }
    }


	//are we expected to use MOOS comms?
    if(GetFlagFromCommandLineOrConfigurationFile("moos_no_comms"))
    {
        m_bUseMOOSComms = false;
    }
    //alternative (older version)
    m_MissionReader.GetValue("UseMOOSComms", m_bUseMOOSComms);


	//are we being asked to sort mail by time..
    m_bSortMailByTime = true;
    if(GetFlagFromCommandLineOrConfigurationFile("moos_no_sort_mail"))
    {
        m_bSortMailByTime = false;
    }
    //alternative
    m_MissionReader.GetConfigurationParam("SortMailByTime",m_bSortMailByTime);


	//are we being asked to quit if iterate fails?
    if(GetFlagFromCommandLineOrConfigurationFile("moos_quit_on_iterate_fail"))
    {
        m_bQuitOnIterateFail = true;
    }



	//OK now figure out our tick speeds  above what is set by default
	//in derived class constructors this can be set in the process config block
	//by the mission architect
    GetParameterFromCommandLineOrConfigurationFile("moos_app_tick",m_dfFreq);
	m_MissionReader.GetConfigurationParam("APPTICK",m_dfFreq);

    GetParameterFromCommandLineOrConfigurationFile("moos_max_app_tick",m_dfMaxAppTick);
	m_MissionReader.GetConfigurationParam("MAXAPPTICK",m_dfMaxAppTick);

	unsigned int nMode = 0;

    GetParameterFromCommandLineOrConfigurationFile("moos_iterate_mode",nMode);
	m_MissionReader.GetConfigurationParam("ITERATEMODE",nMode);


	switch(nMode)
	{
		case 0: SetIterateMode(REGULAR_ITERATE_AND_MAIL); break;
		case 1: SetIterateMode(COMMS_DRIVEN_ITERATE_AND_MAIL); break;
		case 2: SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL); break;
		default:SetIterateMode(REGULAR_ITERATE_AND_MAIL); break;

	}



	//do we want to enable command filtering (default is set in constructor)
    if(GetFlagFromCommandLineOrConfigurationFile("moos_filter_command"))
    {
        m_bCommandMessageFiltering = true;
    }
    //alternative
    m_MissionReader.GetConfigurationParam("CatchCommandMessages",m_bCommandMessageFiltering);


	return IsConfigOK();

}

void CMOOSApp::PrintSearchedConfigurationFileParameters()
{
    std::list<std::string> L = m_MissionReader.GetSearchedParameters(GetAppName());
    std::cout<<MOOS::ConsoleColours::Green();
    std::cout<<"\nThis application has searched the configuration file for the following parameters:\n\n   ";
    std::cout<<MOOS::ConsoleColours::green();
    std::copy(L.begin(),L.end(),std::ostream_iterator<std::string>(std::cerr,"\n   ") );
    std::cout<<MOOS::ConsoleColours::reset();

}

bool CMOOSApp::OnProcessCommandLine()
{
	//use things like
	//m_CommandLineParser.GetVariable("--moos_name",std::string);

	return true;
}

bool CMOOSApp::IsConfigOK()
{
	//put checks in here....if needed (or overload)
	return true;
}

bool CMOOSApp::SetQuiet(bool bQ)
{
	m_bQuiet = bQ;
	m_Comms.SetQuiet(m_bQuiet);
	return true;
}

void CMOOSApp::SetMOOSName(const std::string &sMOOSName)
{
	m_sMOOSName=sMOOSName;
}


void CMOOSApp::DoBanner()
{
	if(m_bQuiet)
		return;
	MOOSTrace("%s is Running:\n",GetAppName().c_str());
	MOOSTrace(" |-Baseline AppTick   @ %.1f Hz\n",m_dfFreq);
	if(m_Comms.IsAsynchronous())
	{
		MOOSTrace(" |--Comms is Full Duplex and Asynchronous\n");
		switch(m_IterationMode)
		{
		case REGULAR_ITERATE_AND_MAIL:
			std::cout<<" -Iterate Mode 0 :\n   |-Regular iterate and message delivery at "<<m_dfFreq<<" Hz\n";
			break;
		case COMMS_DRIVEN_ITERATE_AND_MAIL:
			std::cout<<" |--Iterate Mode 1 :\n   |-Dynamic iterate speed driven by message delivery ";
			if(m_dfMaxAppTick==0.0)
				std::cout<<"at an unlimited rate\n";
			else
				std::cout<<"at up to "<<m_dfMaxAppTick<<"Hz\n";
			break;
		case REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL:
			std::cout<<" |--Iterate Mode 2 :\n   -Regular iterate at "<<m_dfFreq<<" Hz. \n   |-Dynamic message delivery ";
			if(m_dfMaxAppTick==0.0)
				std::cout<<"at an unlimited rate\n";
			else
				std::cout<<"at up to "<<m_dfMaxAppTick<<"Hz\n";

			break;
		}
	}
	else
	{
		MOOSTrace(" |\t Comms is Synchronous\n");
		MOOSTrace(" |\t Baseline CommsTick @ %d Hz\n",m_nCommsFreq);
	}

	if(GetMOOSTimeWarp()!=1.0)
		MOOSTrace("\t|-Time Warp @ %.1f \n",GetMOOSTimeWarp());
	if(m_Comms.GetCommsControlTimeWarpScaleFactor()>0.0  && GetMOOSTimeWarp()>1.0)
	    MOOSTrace("\t|-Time Warp delay @ %.1f ms \n",m_Comms.GetCommsControlTimeWarpScaleFactor()*GetMOOSTimeWarp());


	std::cout<<"\n\n";
    std::cout<<std::flush;


}

/*called by a third party to request a MOOS App to quit - only useful for 
 example if a MOOSApp is run in a secondary thread */
bool CMOOSApp::RequestQuit()
{
	m_bQuitRequested = true;
	return true;
}

bool CMOOSApp::DoRunWork()
{

	bool bIterateRequired = true;

	SleepAsRequired(bIterateRequired);

	//std::cerr<<"bIterate:"<<bIterateRequired<<"\n";

    //store for derived class use the last time iterate was called;
    m_dfLastRunTime = MOOSLocalTime();

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
            
            if(bIterateRequired)
            {
				//////////////////////////////////////
				//  do application specific processing

                /** called just after Iterate has finished - another place to overload*/
            	bool bOK = OnIteratePrepare();
            	if(m_bQuitOnIterateFail && !bOK)
            		return false;

				bOK = Iterate();
				if(m_bQuitOnIterateFail && !bOK)
					return false;

				bOK = OnIterateComplete();
            	if(m_bQuitOnIterateFail && !bOK)
            		return false;


            }
            
            m_nIterateCount++;
        }
    }
    else
    {
        //do private work
        IteratePrivate();

        if(bIterateRequired)
        {
			/////////////////////////////////////////
			//  do application specific processing
			bool bOK = Iterate();

			if(m_bQuitOnIterateFail && !bOK)
				return false;
        }
        
        m_nIterateCount++;
    }
    

    


    return true;
    
}

bool CMOOSApp::SetIterateMode(IterateMode Mode)
{
	if(!m_Comms.IsAsynchronous() && Mode!=REGULAR_ITERATE_AND_MAIL)
	{
		std::cerr<<"can only set iterate mode to REGULAR_ITERATE_AND_MAIL"
				" for old MOOS Clients\n"<<Mode;
		m_IterationMode = REGULAR_ITERATE_AND_MAIL;
		return false;
	}

	m_IterationMode =Mode;
	return true;


}

void CMOOSApp::SleepAsRequired(bool &  bIterateShouldRun)
{

	bIterateShouldRun = true;

	//do we need to sleep at all?
	if(m_dfFreq<=0.0)
	{
		//no we are being told to go flat out
		return;
	}

	//first thing we do is look to see how long we need to sleep in
	//vanilla case
	int nAppPeriod_ms = static_cast<int> (1000.0/m_dfFreq);

	//how long since we ran?
	int nElapsedTime_ms = static_cast<int> (1000.0*( MOOSLocalTime() - m_dfLastRunTime));

	if (nElapsedTime_ms < 0){
		nElapsedTime_ms = 0;
	}

	//so how long do we need to pause for
	int nSleep = (nAppPeriod_ms - nElapsedTime_ms);


	if(nSleep<1){
		//std::cerr<<"no sleep for "<<nSleep<<"\n";
		return;//nothing to do...
	}

	if(!m_Comms.IsAsynchronous())
	{
		//we just always sleep;
		MOOSPause(nSleep);
		return;
	}

#ifdef ASYNCHRONOUS_CLIENT


	//OK, we are a modern client and we have three distinct behaviours
	switch(m_IterationMode)
	{
	case  REGULAR_ITERATE_AND_MAIL:
		//we always to sleep - this behaves like old MOOS did - AppTick governs it all
		//std::cerr<<"sleeping for "<<nSleep<<"\n";
		MOOSPause(nSleep);
		break;
	case  REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL:
		//On NewMail is called as often as is needed but iterate is only called
		//at AppTick rates.
		if(m_Comms.GetNumberOfUnreadMessages() || m_pMailEvent->tryWait(nSleep))
		{
			if(MOOSLocalTime()-m_dfLastRunTime<1.0/m_dfFreq)
			{
				//we have mail but we are in a mode where we don't have
				//to call Iterate
				bIterateShouldRun= false;
			}
		}
		break;
	case COMMS_DRIVEN_ITERATE_AND_MAIL:
		//both OnNewMail and Iterate will be called as fast as mail comes in up
		//to a limiting speed.
		if(m_Comms.GetNumberOfUnreadMessages()>0 || m_pMailEvent->tryWait(nSleep))
		{

			//we got woken by the arrival of mail.....
			//do we need to sleep some more to ensure we don't iterate
			//too fast?
			double dfTimeSinceRun = MOOSLocalTime()-m_dfLastRunTime;
			double dfMinPeriod = m_dfMaxAppTick> 0.0 ? 1.0/m_dfMaxAppTick : 0.0;
			if(dfTimeSinceRun<dfMinPeriod)
			{
				//yes we do..
				nSleep=static_cast<int> (1000*(dfMinPeriod-dfTimeSinceRun));
				if(nSleep>0)
				{
					MOOSPause(nSleep);
				}
			}
		}
		break;
	default:
		break;
	}

#endif

}


bool CMOOSApp::AddMessageRouteToActiveQueue(const std::string & sQueueName,
                    const std::string & sMsgName)
{
    if(!m_Comms.HasActiveQueue(sQueueName))
    {
        return false;
    }
    return m_Comms.AddMessageRouteToActiveQueue(sQueueName,sMsgName);

}

bool CMOOSApp::AddMessageRouteToActiveQueue(const std::string & sQueueName,
		const std::string & sMsgName,
		bool (*pfn)(CMOOSMsg &M, void * pYourParam),
		void * pYourParam )
{
	//first do we have that queue made already?
	if(!m_Comms.HasActiveQueue(sQueueName))
	{
		//no we had better make one...
		m_Comms.AddActiveQueue(sQueueName,pfn,pYourParam);
	}
	//finally make sure this message routes to this message.
	return m_Comms.AddMessageRouteToActiveQueue(sQueueName,sMsgName);
}

bool CMOOSApp::AddActiveMessageQueueCallback(const std::string & sQueueName,
    		const std::string & sMsgName,
    		bool (*pfn)(CMOOSMsg &M, void * pYourParam),
    		void * pYourParam )
{
	return AddMessageRouteToActiveQueue(sQueueName,sMsgName,pfn,pYourParam);
}



bool CMOOSApp::AddMessageRouteToOnMessage(const std::string & sMsgName)
{
	return AddMessageRouteToActiveQueue(sMsgName+"_CB",sMsgName,MOOSAPP_OnMessage,this);
}
//deprecated version
bool CMOOSApp::AddMessageCallback(const std::string & sMsgName)
{
	return AddMessageRouteToOnMessage(sMsgName);
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

void CMOOSApp::SetCommandLineParameters(int argc,  char * argv[])
{
	m_CommandLineParser.Open(argc,argv);
}



bool CMOOSApp::UseMailCallBack()
{
    /* This attaches a callback to thw comms object's mail handler*/
    m_Comms.SetOnMailCallBack(MOOSAPP_OnMail,this);
    return true;
}

////////////////////// DEFAULT HANDLERS //////////////////////
bool CMOOSApp::OnNewMail(MOOSMSG_LIST &)
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

bool CMOOSApp::OnMessage(CMOOSMsg &M)
{
	MOOSTrace("- default handler for %s invoked. Did you mean to write your own?",M.GetKey().c_str());
	return true;
}


bool CMOOSApp::OnDisconnectFromServer()
{
//    MOOSTrace("- default OnDisconnectFromServer called\n");
    return true;
}


/* this block of functions simply notifies the DB that a string variable has changed*/
bool CMOOSApp::Notify(const std::string &sVar, const std::string & sVal, double dfTime)
{
	return m_Comms.Notify(sVar,sVal,dfTime);
}

bool CMOOSApp::Notify(const std::string &sVar, const std::string & sVal, const std::string & sSrcAux, double dfTime)
{
	return m_Comms.Notify(sVar,sVal,sSrcAux,dfTime);
}

bool CMOOSApp::Notify(const std::string &sVar, const char * sVal,double dfTime)
{
	return m_Comms.Notify(sVar,sVal,dfTime);
}

bool CMOOSApp::Notify(const std::string &sVar, const char * sVal,const std::string & sSrcAux, double dfTime)
{
	return m_Comms.Notify(sVar,sVal,sSrcAux,dfTime);
}


/** notify the MOOS community that something has changed (double)*/
bool CMOOSApp::Notify(const std::string & sVar,double dfVal, double dfTime)
{
	return m_Comms.Notify(sVar,dfVal,dfTime);
}
bool CMOOSApp::Notify(const std::string & sVar,double dfVal, const std::string & sSrcAux,double dfTime)
{
	return m_Comms.Notify(sVar,dfVal,sSrcAux,dfTime);
}


/** notify the MOOS community that something has changed binary data*/
bool CMOOSApp::Notify(const std::string & sVar,void *  pData, unsigned int nDataSize, double dfTime)
{
	return m_Comms.Notify(sVar,pData,nDataSize,dfTime);
}
bool CMOOSApp::Notify(const std::string & sVar,void *  pData, unsigned int nDataSize, const std::string & sSrcAux,double dfTime)
{
	return m_Comms.Notify(sVar,pData,nDataSize,sSrcAux,dfTime);
}

bool CMOOSApp::Notify(const std::string & sVar,const std::vector<unsigned char> & vData, double dfTime)
{
	return m_Comms.Notify(sVar,vData,dfTime);
}

bool CMOOSApp::Notify(const std::string & sVar,const std::vector<unsigned char> & vData,const std::string & sSrcAux, double dfTime)
{
	return m_Comms.Notify(sVar,vData,sSrcAux,dfTime);
}



/** Register for notification in changes of named variable*/
bool CMOOSApp::Register(const std::string & sVar,double dfInterval)
{
	return m_Comms.Register(sVar,dfInterval);
}

/** Register with wildcards based on application and variable patterns (* and ? allowed)*/
bool CMOOSApp::Register(const std::string & sVarPattern,const std::string & sAppPattern, double dfInterval)
{
	return m_Comms.Register(sVarPattern,sAppPattern,dfInterval);
}



/** UnRegister for notification in changes of named variable*/
bool CMOOSApp::UnRegister(const std::string & sVar)
{
	return m_Comms.UnRegister(sVar);
}




/** this is a call back from MOOSComms and its use is specialised (not for general consumption)*/
bool CMOOSApp::OnMailCallBack()
{
#ifdef ASYNCHRONOUS_CLIENT
    m_pMailEvent->set();
    return true;
#else
    std::cerr<<"OnMailCallback is deprecated for Asynchronous Clients\n";
    return false;
#endif
}

bool CMOOSApp::OnCommandMsg(CMOOSMsg /*CmdMsg*/)
{
    MOOSTrace("- default OnCommandMsg called\n");
    return true;
}


bool CMOOSApp::ConfigureComms()
{
	m_sServerHost = "LOCALHOST";
	if(!m_CommandLineParser.GetVariable("--moos_host",m_sServerHost))
	{
		if(!m_MissionReader.GetValue("SERVERHOST",m_sServerHost))
		{
			//MOOSTrace("Warning Server host not read from mission file or command line: assuming %s\n",m_sServerHost.c_str());
		}
	}

	m_lServerPort = 9000;
	if(!m_CommandLineParser.GetVariable("--moos_port",m_lServerPort))
	{
		if(!m_MissionReader.GetValue("SERVERPORT",m_lServerPort))
		{
			//MOOSTrace("Warning Server port not read from mission file or command line: assuming %d\n",m_lServerPort);
		}
	}

	//this is to support an old mistake
	//m_sServerPort shoudl never have been a string!
	std::stringstream ss;
	ss<<m_lServerPort;
	m_sServerPort = ss.str();

    if(!CheckSetUp())
        return false;


    //OK now figure out our speeds etc above what is set by default
    //in derived class constructors
    m_MissionReader.GetConfigurationParam("COMMSTICK",m_nCommsFreq);
    m_CommandLineParser.GetOption("--moos_comms_tick",m_nCommsFreq);
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

/** return the application frequency*/
double CMOOSApp::GetAppFreq()
{
	return m_dfFreq;
}

/** get the comms frequency*/
unsigned int CMOOSApp::GetCommsFreq()
{
	return m_nCommsFreq;
}


void CMOOSApp::SetAppFreq(double  dfFreq,double dfMaxFreq)
{
    if(m_dfFreq<=MOOS_MAX_APP_FREQ)
    {
        m_dfFreq = dfFreq;
    }
    else
    {
    	std::cout<<"Setting baseline apptick to allowable maximum of "<<MOOS_MAX_APP_FREQ<<std::endl;
    }

    if(dfMaxFreq>=0.0)
    {
    	m_dfMaxAppTick = dfMaxFreq;
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

void CMOOSApp::WaitForEmptyOutbox()
{
	while(m_Comms.GetNumberOfUnsentMessages())
	{
		MOOSPause(100);
	}
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

    for(p = m_MOOSVars.begin();p!=m_MOOSVars.end();++p)
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

bool CMOOSApp::SetMOOSVar(const CMOOSVariable& Var)
{
    MOOSVARMAP::iterator p = m_MOOSVars.find(Var.GetName ());
	
    if(p==m_MOOSVars.end())
        return false;
	
    p->second = Var;
    p->second.SetFresh (true);
    return true;
}

bool CMOOSApp::UpdateMOOSVariables(MOOSMSG_LIST &NewMail)
{
    //we only subscribe to things if we are in simulator mode
    MOOSVARMAP::iterator p;
    double dfTimeNow = MOOSTime();
    for(p = m_MOOSVars.begin();p!=m_MOOSVars.end();++p)
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

    for(p = m_MOOSVars.begin();p!=m_MOOSVars.end();++p)
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
    for(q=NewMail.begin();q!=NewMail.end();++q)
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

double CMOOSApp::GetCPULoad()
{
	double dfLoad=0;
	m_ProcessMonitor.GetPercentageCPULoad(dfLoad);
	return dfLoad;
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

    double dfCPULoad;
    if(m_ProcessMonitor.GetPercentageCPULoad(dfCPULoad))
    {
    	ssStatus<<"cpuload="<<std::setprecision(4)<<dfCPULoad<<",";
    }

    size_t memory_now, memory_max;
    if(m_ProcessMonitor.GetMemoryUsage(memory_now,memory_max))
    {
    	ssStatus<<"memory_kb="<<std::setprecision(4)<<memory_now/(1024.0)<<",";
    	ssStatus<<"memory_max_kb="<<std::setprecision(4)<<memory_max/(1024.0)<<",";
    }


    ssStatus<<"MOOSName="<<GetAppName()<<",";

    ssStatus<<"Publishing=\"";
    std::copy(Published.begin(),Published.end(),std::ostream_iterator<string>(ssStatus,","));
	ssStatus<<"\",";

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
        std::string sStatus = MOOSToUpper(GetAppName())+"_STATUS";
        MOOSToUpper(sStatus);
        m_Comms.Notify(sStatus,MakeStatusString());
        m_dfLastStatusTime = MOOSTime();
    }
}
