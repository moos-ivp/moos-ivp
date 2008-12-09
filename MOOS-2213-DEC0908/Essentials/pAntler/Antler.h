/*
 *  Antler.h
 *  MOOS
 *
 *  Created by pnewman on 18/04/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#ifndef ANTLERH
#define ANTLERH

#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include <string>
#include <iostream>

#ifdef _WIN32
	#include "XPCProcess.h"
	#include "XPCProcessAttrib.h"
	#include "XPCException.h"
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <signal.h>
#endif 

#define DEFAULT_NIX_TERMINAL "xterm"
#define DEFAULTTIMEBETWEENSPAWN 1000


class CAntler 
    {
    public:
        
        //something to hold alient process information for the Antler class
        struct MOOSProc
        {
#ifdef _WIN32
            XPCProcess * pWin32Proc;
            XPCProcessAttrib * pWin32Attrib;
#else
            pid_t m_ChildPID;
#endif        
            std::string m_sApp;
            std::string m_sMOOSName;
            std::string m_sMissionFile;
            bool m_bInhibitMOOSParams;
            bool m_bNewConsole;
            STRING_LIST m_ExtraCommandLineParameters;
            STRING_LIST m_ConsoleLaunchParameters;
            
        };
        
    public:
        
        //constructor
        CAntler();
        //this is the only public function. Call it to have Antler do its thing.
        bool Run(const std::string & sMissionFile,std::set<std::string> Filter = std::set<std::string>() );
        
        //run in a headless fashion - instructions will be recieved via MOOSComms
        bool Run(const std::string & sHost,  int lPort, const std::string & sAntlerName);
        
    protected:
        
        //top level spawn - all comes from here
        bool Spawn(const std::string & sMissionFile, bool bHeadless = false);
        
        
        //create, configure and launch a process
        MOOSProc* CreateMOOSProcess(std:: string sProcName);
        
        // called to figure out what xterm parameters should be used with launch (ie where should the xterm be and how should it look)
        bool MakeConsoleLaunchParams(std::string sParam,STRING_LIST & LaunchList,std::string sProcName,std::string sMOOSName);
        
        //caled to figure out what if any additional  parameters should be passed to the process being launched	
        bool MakeExtraExecutableParameters(std::string sParam,STRING_LIST & ExtraCommandLineParameters,std::string sProcName,std::string sMOOSName);
        
        //Functions responsible for actually start new processes according to OS
#ifndef _WIN32
        bool DoNixOSLaunch(MOOSProc * pNewProc);
#else
        bool DoWin32Launch(MOOSProc *pNewProc);
#endif
        bool ConfigureMOOSComms();
        bool SendMissionFile();
        
        //tell a Monarch what is goinon remotely
        bool PublishProcessQuit(const std::string & sProc);
        bool PublishProcessLaunch(const std::string & sProc);
        
        typedef std::list<MOOSProc*> MOOSPROC_LIST;
        MOOSPROC_LIST    m_ProcList;
        std::string m_sDefaultExecutablePath;
        CProcessConfigReader m_MissionReader;
        
        //if this set is non empty then only procs listed here will be run..
        std::set<std::string> m_Filter;
        
        int m_nCurrentLaunch;
        
        //this is used to communicate with the BD and ultimately other instantiations of
        //pAntler on different machines...
        CMOOSThread m_RemoteControlThread;
        CMOOSCommClient * m_pMOOSComms;
        /**method to allow Listen thread to be launched with a MOOSThread.*/
        static bool _RemoteControlCB(void* pParam)
        {
            CAntler* pMe = (CAntler*) pParam;
            return pMe->DoRemoteControl();
        }
		/** internal MOOS client callbacks*/
        static bool _MOOSConnectCB(void *pParam)
        {
            CAntler* pMe = (CAntler*) pParam;
            return pMe->OnMOOSConnect();
        }
        static bool _MOOSDisconnectCB(void *pParam)
        {
            CAntler* pMe = (CAntler*) pParam;
            return pMe->OnMOOSDisconnect();
        }
        /** main comms handling thread*/
        bool DoRemoteControl();
        /** hellos MOOSDB!*/
        bool OnMOOSConnect();
        /** goodby MOOSDB*/
        bool OnMOOSDisconnect();
        
        CMOOSLock m_JobLock;
        std::string m_sMissionFile;
        bool m_bHeadless;
        bool m_bQuitCurrentJob;
        bool m_bRunning;
        bool m_bNewJob;
        std::string m_sMonarchAntler;
        bool m_bKillOnDBDisconnect;
        std::string m_sReceivedMissionFile;
        std::string m_sAntlerName;
        std::string m_sDBHost;
        int m_nDBPort;
        

        
    };
#endif

