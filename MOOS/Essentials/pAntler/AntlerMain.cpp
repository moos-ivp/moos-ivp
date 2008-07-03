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



#ifdef _WIN32
#pragma warning(disable : 4786)
#endif



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
#endif 

using namespace std;

#define DEFAULT_NIX_TERMINAL "xterm"
#define DEFAULTTIMEBETWEENSPAWN 1000


class CAntler 
{
protected:

    //something to hold alient process information for the Antler class
    struct MOOSProc
    {
#ifdef _WIN32
        XPCProcess * pProc;
        XPCProcessAttrib * pAttrib;
#else
        pid_t m_ChildPID;
#endif        
        string sApp;
    
    };
    
public:
    //this is the only public function. Call it to have Antler do its thing.
    bool Spawn(const std::string & sMissionFile);
    
protected:
    //create, configure and launch a process
    MOOSProc* CreateMOOSProcess(std:: string sProcName);
    
    // called to figure out what xterm parameters should be used with launch (ie where should teh xterm be and how should it look)
    bool MakeXtermLaunchParams(std::string sParam,STRING_LIST & LaunchList,std::string sProcName,std::string sMOOSName);
    
    //caled to figure out what if any additional  parameters should be passed to the process being launched	
    bool MakeExtraExecutableParameters(std::string sParam,STRING_LIST & ExtraCommandLineParameters,std::string sProcName,std::string sMOOSName);
    
    
    typedef std::list<MOOSProc*> MOOSPROC_LIST;
    MOOSPROC_LIST    m_ProcList;
    std::string m_sDefaultExecutablePath;
    CProcessConfigReader m_MissionReader;
    int m_nCurrentLaunch;

};


int main(int argc ,char *argv[])
{
    
    MOOSTrace("*************************************\n");
    MOOSTrace("*  This is Antler, head of MOOS...  *\n");
    MOOSTrace("*  P. Newman 2008                   *\n");
    MOOSTrace("*************************************\n");
    
    
    if(argc!=2)
    {
        MOOSTrace("Mission File not specified\n");
        MOOSPause(1000);
        return -1;
    }
    
    std::string sMissionFile = argv[1];
        
    CAntler Antler;
    
    return Antler.Spawn(sMissionFile) ? 0 :-1;

}

bool CAntler::Spawn(const std::string &  sMissionFile)
{
  m_nCurrentLaunch = 0;
  
  
    //set up the mission file reader
    if(!m_MissionReader.SetFile(sMissionFile))
        return MOOSFail("error reading mission file\n");
    
    
    m_MissionReader.SetAppName("ANTLER"); //NB no point in running under another name...(I guess Anter1 could luanch Antler2 though...)

    STRING_LIST      sParams;

    if(!m_MissionReader.GetConfiguration(  m_MissionReader.GetAppName(),sParams))
        return MOOSFail("error reading antler config block from mission file\n");

    //fetch all the lines in teg Antler configuration block
    STRING_LIST::iterator p;
    sParams.reverse();

    int nTimeMSBetweenSpawn=DEFAULTTIMEBETWEENSPAWN;
    m_MissionReader.GetConfigurationParam("MSBetweenLaunches",nTimeMSBetweenSpawn);
    
    //here we'll figure out a what paths to use when looking for  executables
    m_sDefaultExecutablePath = "SYSTEMPATH";
    m_MissionReader.GetConfigurationParam("ExecutablePath",m_sDefaultExecutablePath);

    if(!MOOSStrCmp("SYSTEMPATH",m_sDefaultExecutablePath))
    {
        MOOSTrace(" \"ExecutablePath\" is %s  (silent default is system path)\n",m_sDefaultExecutablePath.c_str());
        if(*m_sDefaultExecutablePath.rbegin()!='/')
        {
            //look to add extra / if needed
            m_sDefaultExecutablePath+='/';
        }

    }
    else
    {
        MOOSTrace("Unless directed otherwise using system path to locate binaries \n");
        m_sDefaultExecutablePath="";
    }

	//no cycle through each line in teh configuration block. If it begins with run then it means launch
    for(p = sParams.begin();p!=sParams.end();p++)
    {
        string sLine = *p;

        string sWhat = MOOSChomp(sLine,"=");

        if(MOOSStrCmp(sWhat,"RUN"))
        {
            //OK we are being asked to run a process
            
            //try to create a process
            MOOSProc* pNew  = CreateMOOSProcess(sLine);
            
            if(pNew!=NULL)
            {
                MOOSTrace("MOOSProcess: \"%s\" launched successfully\n",pNew->sApp.c_str());
                m_ProcList.push_front(pNew);
		m_nCurrentLaunch++;
            }
            
	    
            MOOSPause(nTimeMSBetweenSpawn);
            
        }
    }

    //now wait on all our processes to close....


    while(m_ProcList.size()!=0)
    {
        MOOSPROC_LIST::iterator q;
        
        for(q = m_ProcList.begin();q!=m_ProcList.end();q++)
        {
            MOOSProc * pMOOSProc = *q;
            
#ifdef _WIN32
            
            pMOOSProc->pProc->vWaitForTerminate(500);
            if(    pMOOSProc->pProc->dwGetExitCode()!=STILL_ACTIVE)
            {
                MOOSTrace("%s has quit\n",pMOOSProc->sApp.c_str());
                
                delete pMOOSProc->pAttrib;
                delete pMOOSProc->pProc;
                delete pMOOSProc;
                
                ProcList.erase(q);
                break;
            }
#else
            int nStatus = 0;
            if(waitpid(pMOOSProc->m_ChildPID,&nStatus,WNOHANG)>0)
            {
                MOOSTrace("%s has quit\n",pMOOSProc->sApp.c_str());
                
                m_ProcList.erase(q);
                break;
            }
            MOOSPause(500);
    #endif

        }

    }

    MOOSTrace("All MOOS Processes complete...\n");

    return 0;
    
}


#ifdef _WIN32
MOOSProc* CreateMOOSProcess(string sProcName,
                string sParam,
                string sMissionFile,
				string sDefaultExePath)
{
    
    MOOSProc *            pNewProc= NULL;
    XPCProcessAttrib*    pAttrib    = NULL;
    XPCProcess *          pProc    = NULL;

    try
    {
		//FIX ME FIX ME
        
        //do we want a new console?
        string sConsole = MOOSChomp(sParam,"=");
        
        string sOption = MOOSChomp(sParam,"~");

        // make the command line ProcessBinaryName MissionFile ProcessMOOSName
        string sCmd = sProcName+" " +sMissionFile+" "+sParam ;

        //make a new process attributes class
        XPCProcessAttrib* pAttrib = new XPCProcessAttrib(NULL,(char *)sCmd.c_str());

        if(MOOSStrCmp("NEWCONSOLE",sConsole) & MOOSStrCmp(sOption,"TRUE") )
        {
            pAttrib->vSetCreationFlag(CREATE_NEW_CONSOLE | CREATE_SUSPENDED);        
        }
        else
        {
            pAttrib->vSetCreationFlag(CREATE_SUSPENDED);
        }

    
        //we shall use our own start up image as a starting point
        STARTUPINFO StartUp;
        GetStartupInfo (&StartUp);

        //set up the title
        StartUp.lpTitle = (char*)sProcName.c_str();

        if(sProcName.find("MOOSDB")!=string::npos)
        {
            StartUp.dwFillAttribute = 
            
            FOREGROUND_INTENSITY| 
            FOREGROUND_BLUE |
            FOREGROUND_RED |
            FOREGROUND_GREEN|
            BACKGROUND_BLUE | 
            BACKGROUND_INTENSITY ;

            StartUp.dwFlags|=STARTF_USEFILLATTRIBUTE;
        }
        else if(sProcName.find("iRemote")!=string::npos)
        {
            StartUp.dwFillAttribute = 
            
            FOREGROUND_INTENSITY| 
            FOREGROUND_BLUE |
            FOREGROUND_RED |
            FOREGROUND_GREEN|
            BACKGROUND_RED | 
            BACKGROUND_INTENSITY ;

            StartUp.dwFlags|=STARTF_USEFILLATTRIBUTE;

        }
            
        pAttrib->vSetStartupInfo(&StartUp);


        XPCProcess * pProc = new XPCProcess(*pAttrib);

        pNewProc = new MOOSProc;

        pNewProc->pAttrib = pAttrib;
        pNewProc->pProc = pProc;
        pNewProc->sApp = sProcName;

        pProc->vResume();
        
    }
    catch (XPCException & e)
    {
        if(pAttrib!=NULL)
        {
            delete pAttrib;
        }
        if(pProc!=NULL)
        {
            delete pProc;
        }
        if(pNewProc!=NULL)
        {
            delete pNewProc;
            pNewProc = NULL;
        }
        MOOSTrace("*** %s Launch Failed:***\n\a\a",sProcName.c_str());
        MOOSTrace("%s\n",e.sGetException());
    }

    return pNewProc;

}
#else


bool CAntler::MakeExtraExecutableParameters(std::string sParam,STRING_LIST & ExtraCommandLineParameters,std::string sProcName,std::string sMOOSName)
{
    
    ExtraCommandLineParameters.clear();
    
    std::string sExtraParamsName;
    if(!MOOSValFromString(sExtraParamsName, sParam, "ExtraProcessParams", true))
        return true;//nothing to do
    
    std::string sExtraParams;
    
    //OK look for this configuration string
    if(!m_MissionReader.GetConfigurationParam(sExtraParamsName,sExtraParams))
        return MOOSFail("cannot find extra parameters named \"%s\"\n",sExtraParamsName.c_str());
    
    while(!sExtraParams.empty())
        ExtraCommandLineParameters.push_back(MOOSChomp(sExtraParams,","));
    
    return true;
    
}

bool CAntler::MakeXtermLaunchParams(std::string sParam,STRING_LIST & LaunchList,std::string sProcName,std::string sMOOSName)
{
    //sParam is a string in the Run=ProcName @ sParam ~ MOOSName
    std::string sLaunchConfigurationName;
    std::string sLaunchConfiguration;
    
    if(!MOOSValFromString(sLaunchConfigurationName, sParam, "LaunchString"))
    {
        
        string sBgColor = "#DDDDFF";
        string sFgColor = "#000000";
        
        if(MOOSStrCmp(sProcName,"iRemote"))
        {
            sBgColor = "#CC0000";
            sFgColor = "#FFFFFF";
        }
        if(MOOSStrCmp(sProcName,"MOOSDB"))
        {
            sBgColor = "#003300";
            sFgColor = "#FFFFFF";
        }
        
        sLaunchConfiguration = "-geometry,"+MOOSFormat("80x12+2+%d",(m_nCurrentLaunch++)*50)+
        ",+sb,"+
        ",-fg,"+sFgColor+
        ",-bg,"+sBgColor+
        ",-T,"+MOOSFormat("%s %s",sProcName.c_str(),sMOOSName.empty()?"":MOOSFormat("as MOOSName \"%s\"",sMOOSName.c_str()).c_str());
        
    }
    else
    {
        
        //OK look for this configuration string
        if(!m_MissionReader.GetConfigurationParam(sLaunchConfigurationName,sLaunchConfiguration))
            return false;
    }
    
    //OK now simply chomp our way through a space delimited list...
    while(!sLaunchConfiguration.empty())
    {
        MOOSTrimWhiteSpace(sLaunchConfiguration);
        std::string sP = MOOSChomp(sLaunchConfiguration,",");
        MOOSTrimWhiteSpace(sP);
        if(!sP.empty())
        	LaunchList.push_back(sP);
    }
    
    return !LaunchList.empty();
       
    
}



CAntler::MOOSProc* CAntler::CreateMOOSProcess(string sConfiguration)
{
    

    //what tis its name? (if no @ symbol we just get the name and no cmdline)
    string sProcName = MOOSChomp(sConfiguration,"@");
    
    //further parameters are to left left of @
    string sParam = sConfiguration;
    
    if(sProcName.empty())
    {
        MOOSTrace("no process specified - RUN=???\n");
        return NULL;
    }
        
    //std::string sFullProcName = m_sDefaultExecutablePath+sProcName;
    
    //if things go well this will eventually point to a new Process
    MOOSProc * pNewProc    = NULL;
    
	//look for tilde demarking end of param=val block
    string sOption = MOOSChomp(sParam,"~");
    
    //do we want a new console?
    bool bNewConsole = false;
	MOOSValFromString(bNewConsole, sOption, "NEWCONSOLE",true);
    
    //do we want to inhibit the passing of MOOS parameters (mission file and MOOSName)
    bool bInhibitMOOSParams = false;
    MOOSValFromString(bInhibitMOOSParams, sOption, "InhibitMOOSParams", true);
    
    //by default process are assumed to be on the system path
    //users can specify an alternative path for all process by setting "ExectutablePath=<Path>" in the mission file
    //configuration block.
	//user has the option of specifying paths individually process by process.
    //alternativelt they can specify that a particular process shouod be located by  system wide and not
    //in the default executable path
    std::string sSpecifiedPath;
    std::string sFullProcName=sProcName;
    if(MOOSValFromString(sSpecifiedPath, sOption, "PATH",true))
    {
        if(MOOSStrCmp(sSpecifiedPath,"SYSTEM"))
        {
           	//do nothing - the system should know where to look 
        }
        else
        {
            //ok we are being told to look in a special place
            sFullProcName=sSpecifiedPath+"/"+sProcName;
        }
    }
    else
    {
        //we just use the Anter-wide Exepath
        sFullProcName = m_sDefaultExecutablePath+sProcName;
    }
    
    //name of process as registered...is the first param after "~"
    string sMOOSName = MOOSChomp(sParam);
    
    //here we figure out what MOOS name is implied if none is given (thanks to N.P and F.A)
    if(sMOOSName.empty())
	{
        std::string sTmp = sProcName;
        if(sTmp.rfind('/') != string::npos) 
        {
            sTmp = sTmp.substr(sTmp.rfind('/')+1);
        }
        if(sTmp.empty()) 
        { 
            // ended with a / ?
            MOOSTrace("Error in configuration  -MOOS Name cannot end in \" / \" : %s\n",sProcName.c_str());
            return NULL;
        }
        sMOOSName = sTmp;
	}
    
    
    //it is pssible to specifiy complicated parameters to the process being launched. (For example
    //xterm being passed a whole load of configurations and then the name of the MOOS process it should
    //itself launch. This next call fills in a string list of such parameters
    STRING_LIST sLaunchParams;
    if(bNewConsole)
    	MakeXtermLaunchParams(sOption, sLaunchParams,sProcName,sMOOSName);
    
    
    //here we figure what extra command line parameters should be given to the MOOS Process
    //being launched.
    STRING_LIST ExtraCommandLineParameters;
    MakeExtraExecutableParameters(sOption,ExtraCommandLineParameters,sProcName,sMOOSName);
    
    
    
    //make a new process info holder stucture...
    pNewProc = new MOOSProc;
    pNewProc->sApp = sFullProcName;
                
    //make a child process
    if((pNewProc->m_ChildPID = fork())<0)
    {
        //hell!
        MOOSTrace("fork failed, not good\n");
        return NULL;
    }
    else if(pNewProc->m_ChildPID ==0)
    {
        //I'm the child now..
        
        
        
        STRING_LIST::iterator p = sLaunchParams.begin();
        char ** pExecVParams = new char  *  [sLaunchParams.size()+ExtraCommandLineParameters.size()+ 6];
        int i = 0;
        
        
        //do we need to configure an xterm?
        if(bNewConsole)
        {
            pExecVParams[i++] =DEFAULT_NIX_TERMINAL; 
            if(!sLaunchParams.empty())
            {
                while(p!=sLaunchParams.end())
                {
                    pExecVParams[i++] = (char*)(p++->c_str());
                }
            }
           
            pExecVParams[i++] = "-e";
            
        }
        
        //here we fill in the process name we really care about
        pExecVParams[i++] = (char *)sFullProcName.c_str() ;
        if(!bInhibitMOOSParams)
        {
            //we do teh usual thing f supplying Mission file and MOOSName
            pExecVParams[i++] = (char *)m_MissionReader.GetFileName().c_str(); 
            pExecVParams[i++] = (char *)sMOOSName.c_str(); 
        }
        
        
        //here we pass extra parameters to the MOOS process if required
        for(p = ExtraCommandLineParameters.begin();p!=ExtraCommandLineParameters.end();p++)
        {
            pExecVParams[i++] = (char *)(p->c_str());
        }
        
        //terminate list
        pExecVParams[i++] = NULL;
        
        for(int j = 0;j<i;j++)
            MOOSTrace("argv[%d]:\"%s\"\n",j,pExecVParams[j]);
        
        MOOSTrace("here we go\n");
        //and finally replace ourselves with a new xterm process image
        if(execvp(pExecVParams[0],(char * const *)pExecVParams)==-1)
        {
            MOOSTrace("Failed exec - not good. Called exec as follows:\n");
            exit(EXIT_FAILURE);
            
        }
        
    }

    //Parent execution stream continues here...
    //with nothing to do

    return pNewProc;

}
#endif

