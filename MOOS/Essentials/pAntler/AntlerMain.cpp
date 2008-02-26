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
#ifdef _WIN32
#include "XPCProcess.h"
#include "XPCProcessAttrib.h"
#include "XPCException.h"
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif 

#include <string>
#include <iostream>

using namespace std;


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

//forward declarations...
MOOSProc* CreateMOOSProcess(string sProcName,string sCmdLine,string sMissionFile);

typedef list <MOOSProc* > MOOSPROC_LIST;









int main(int argc ,char *argv[])

{

    MOOSTrace("*************************************\n");
    MOOSTrace("*  This is Antler, head of MOOS...  *\n");
    MOOSTrace("*        P. Newman 2001             *\n");
    MOOSTrace("*************************************\n");
    
    if(argc!=2)
    {
        MOOSTrace("Mission File not specified\n");
        MOOSPause(1000);
        return -1;
    }

    char * sMissionFile = argv[1];


    CProcessConfigReader MissionReader;
    MissionReader.SetAppName("ANTLER"); //NB no point in running under another name...(I guess Anter1 could luanch Antler2 though...)


    if(!MissionReader.SetFile(sMissionFile))
    {
        MOOSTrace("error reading mission file\n");
        return -1;
    }

    

    MOOSPROC_LIST    ProcList;
    STRING_LIST        sParams;



    if(!MissionReader.GetConfiguration(    MissionReader.GetAppName(),sParams))
    {
        MOOSTrace("error reading antler config block from mission file\n");
    }

    STRING_LIST::iterator p;
    sParams.reverse();



    int nTimeMSBetweenSpawn=1000;

    double dfTmp;
    if(!MissionReader.GetConfigurationParam("MSBetweenLaunches",dfTmp))
    {
        MOOSTrace("Time between launches is default %d MS (use \"MSBetweenLaunches\" variable to change) \n",nTimeMSBetweenSpawn);        
    }
    else
    {
        nTimeMSBetweenSpawn = (int)dfTmp;
        MOOSTrace("launching every %d MS \n",nTimeMSBetweenSpawn);        
    }


    //here we'll figure out a what paths to use when looking for  executables
    std::string sExePath = "SYSTEMPATH";
    MissionReader.GetConfigurationParam("ExecutablePath",sExePath);

    if(!MOOSStrCmp("SYSTEMPATH",sExePath))
    {
        MOOSTrace(" \"ExecutablePath\" is %s  (use \"ExecutablePath=SYSTEMPATH\" to use system path)\n",sExePath.c_str());
        if(*sExePath.rbegin()!='/')
        {
            sExePath+='/';
        }

    }
    else
    {
        MOOSTrace(" Using system path to locate MOOS Binaries \n");
        sExePath="";
    }


    for(p = sParams.begin();p!=sParams.end();p++)
    {
        string sLine = *p;

        string sWhat = MOOSChomp(sLine,"=");

        if(MOOSStrCmp(sWhat,"RUN"))
        {
            //OK we are being asked to run a process

            //what tis its name? (if no @ symbol we just get the name and no cmdline)
            string sProcName = MOOSChomp(sLine,"@");
            std::string sFullProcName = sExePath+sProcName;

            //cmd line is to left of @
            string sParam = sLine;
            
            if(!sProcName.empty())
            {
                //try to create a process
                MOOSProc* pNew  = CreateMOOSProcess(sFullProcName,
                                sParam,
                                sMissionFile);

                if(pNew!=NULL)
                {
                    MOOSTrace("MOOSProcess: \"%s\" launched successfully\n",sProcName.c_str());
                    ProcList.push_front(pNew);
                }

                MOOSPause(nTimeMSBetweenSpawn);
            }
        }
    }

    //now wait on all our processes to close....


    while(ProcList.size()!=0)
    {
        MOOSPROC_LIST::iterator q;
        
    for(q = ProcList.begin();q!=ProcList.end();q++)
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

                ProcList.erase(q);
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
                string sMissionFile)
{
    
    MOOSProc *            pNewProc= NULL;
    XPCProcessAttrib*    pAttrib    = NULL;
    XPCProcess *          pProc    = NULL;

    try
    {

        
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



MOOSProc* CreateMOOSProcess(string sProcName,
                string sParam,
                string sMissionFile)
{
    static int iCurrentLaunch = 0;
    iCurrentLaunch++;
    MOOSProc * pNewProc    = NULL;

    string sPath;

    //do we want a new console?
    string sConsole = MOOSChomp(sParam,"=");
    
    string sOption = MOOSChomp(sParam,"~");

    //name of process as registered...
    string sMOOSName = sParam;

    if(sMOOSName.empty())
    {
      std::string sTmp = sProcName;
      MOOSRemoveChars(sTmp,"./");
      sMOOSName = sTmp;
    }

      
    bool bNewConsole = false;
    if(MOOSStrCmp("NEWCONSOLE",sConsole) & MOOSStrCmp(sOption,"TRUE") )
    {
        bNewConsole = true;
    }


    //make a new process info holder stucture...
    pNewProc = new MOOSProc;
    
    //added August 2003 PMN (was originally commented out why....?
//    sProcName = "./"+sProcName;

    pNewProc->sApp = sProcName;

                
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
        //exec over me...
/*
    MOOSTrace("About to launch %s under MOOS Name \"%s\" on %s, New console = %s\n",
          sProcName.c_str(),
          sMOOSName.c_str(),
          sMissionFile.c_str(),
          bNewConsole?"YES":"NO");
*/
        if(bNewConsole)
        {
            string sBgColor = "#DDDDFF";
            string sFgColor = "#000000";
            
            if(sProcName.find("iRemote")!=string::npos)
            {
                sBgColor = "#CC0000";
                sFgColor = "#FFFFFF";
            }
            else if(sProcName.find("MOOSDB")!=string::npos)
            {
                sBgColor = "#003300";
                sFgColor = "#FFFFFF";
            }
            if(execlp("xterm","xterm",
              "-geometry",MOOSFormat("80x12+2+%d",(iCurrentLaunch)*50).c_str(),
              "+sb",
              "-fg",sFgColor.c_str(),
              "-bg",sBgColor.c_str(),
              "-T",MOOSFormat("%s %s",sProcName.c_str(),sMOOSName.empty()?"":MOOSFormat("as MOOSName \"%s\"",sMOOSName.c_str()).c_str()).c_str(),
              "-e",
                sProcName.c_str(),
                sMissionFile.c_str(),
                sMOOSName.c_str(),
                NULL )==-1)
            {
                MOOSTrace("failed exec -> not good\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if(execlp(  sProcName.c_str(),
                        sProcName.c_str(),
                        sMissionFile.c_str(),
                        sMOOSName.c_str(),
                        NULL )==-1)
            {
                MOOSTrace("failed exec -> not good\n");
                exit(EXIT_FAILURE);
            }
            
        }

    }

    //Parent execution stream continues here...
    //with nothing to do

    return pNewProc;

}
#endif

