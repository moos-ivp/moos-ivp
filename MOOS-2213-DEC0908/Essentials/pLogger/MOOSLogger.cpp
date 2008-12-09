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
// MOOSLogger.cpp: implementation of the CMOOSLogger class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif


#include <MOOSLIB/MOOSApp.h>
#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <algorithm>
#include <MOOSGenLib/MOOSAssert.h>
#include <cmath>
#include <cstring>

#ifndef _WIN32
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif


using namespace std;
#include "MOOSLogger.h"

//maximum of logged columns...
#define MAX_SYNC_COLUMNS 255
#define MOOS_LOGGER_DEFAULT_PERIOD  2.0
#define COLUMN_WIDTH 18
#define DEFAULT_MONITOR_TIME 10.0
#define MIN_SYNC_LOG_PERIOD 0.1
#define DYNAMIC_NAME_SPACE 64
#define DEFAULT_WILDCARD_TIME 1.0 //how often to call into the DB to get a list of all variables if wild card loggin is turned on

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSLogger::CMOOSLogger()
{
    //be default we don't need to be too fast..
    SetAppFreq(5);

    //if no file name is given log files will begin with "MOOS"
    m_sStemFileName = "MOOS";

    //by default make an a-log
    m_bAsynchronousLog = true;

    //by default make an s-log
    m_bSynchronousLog = true;

    //and log every second
    m_dfSyncLogPeriod = 1.0;

    //start immediately
    m_dfLastSyncLogTime = 0;

    //we have no immediate need to check monitored variables
    m_dfLastMonitorTime = MOOSTime();

    //no s-log lines written yet
    m_nSyncLines = 0;

    //by default (if no mission file is specified) log to a local directory
    m_sPath = "./";

    //in a directory with the following stem
    m_sStemFileName = "MOOSLog";

    //and append a time ot the stem
    m_bAppendFileTimeStamp = true;
    
    //lets always sort mail by time...
    SortMailByTime(true);

}

CMOOSLogger::~CMOOSLogger()
{
    CloseFiles();
}



bool CMOOSLogger::CloseFiles()
{
    if(m_AsyncLogFile.is_open())
    {
        m_AsyncLogFile.close();
    }

    if(m_SyncLogFile.is_open())
    {
        m_SyncLogFile.close();
    }

    if(m_SystemLogFile.is_open())
    {
        m_SystemLogFile.close();
    }

    return true;

}


bool CMOOSLogger::OnConnectToServer()
{
    //ok so now lets register our interest in all these MOOS vars!
    RegisterMOOSVariables();

    //additional variables that are intersting to us..
    m_Comms.Register("LOGGER_RESTART",0.5);

    return true;
}

bool CMOOSLogger::OnNewMail(MOOSMSG_LIST &NewMail)
{
    //these three calls look through the incoming mail
    //and handle all appropriate logging
    DoAsyncLog(NewMail);

    UpdateMOOSVariables(NewMail);

    LogSystemMessages(NewMail);


    //here we look for more unusual things
    MOOSMSG_LIST::iterator q;

    for(q=NewMail.begin();q!=NewMail.end();q++)
    {

        if(q->IsSkewed(MOOSTime()))
            continue;

        //are we being asked to restart?
        if(MOOSStrCmp(q->GetKey(),"LOGGER_RESTART"))
        {
            OnLoggerRestart();
        }

    }

    return true;
}


bool CMOOSLogger::OnStartUp()
{
    //alway subscribe to these variables
    //they make up the sync log
    AddMOOSVariable("MOOS_DEBUG","MOOS_DEBUG","",0);
    AddMOOSVariable("MOOS_SYSTEM","MOOS_SYSTEM","",0);


    //are we required to perform synchronous logs?
    string sTmp;
    if(m_MissionReader.GetConfigurationParam("SYNCLOG",sTmp))
    {
        string sBool = MOOSChomp(sTmp,"@");
        if(MOOSStrCmp(sBool,"TRUE"))
        {
            m_bSynchronousLog = true;
        }

        //look for an additional parameter saying how often to log...
        if(!sTmp.empty())
        {
            //how often are we required to perform synchronous logs?
            m_dfSyncLogPeriod = atof(sTmp.c_str());

            //this limit is intentional - the thinking mans logs are alogs
            //slogs are more expensive to write. I choose about 10Hz
            if(m_dfSyncLogPeriod<MIN_SYNC_LOG_PERIOD)
            {
                m_dfSyncLogPeriod = MIN_SYNC_LOG_PERIOD;
            }
        }
    }


    //are we required to perform Asynchronous logs?
    m_MissionReader.GetConfigurationParam("ASYNCLOG",m_bAsynchronousLog);

    //what sort of file name are we using
    m_MissionReader.GetConfigurationParam("FILETIMESTAMP",m_bAppendFileTimeStamp);

    //do we have a path global name?
    if(!m_MissionReader.GetValue("GLOBALLOGPATH",m_sPath))
    {
        //read path name
        if(!m_MissionReader.GetConfigurationParam("PATH",m_sPath))
        {
            MOOSTrace("Warning:\n\tneither \"::GlobalLogPath\" or \"Path\" are specified\n");
            MOOSTrace("\tWill Log to %s\n",m_sPath.c_str());
        }
    }

    //remove trailing "/";

    if(*m_sPath.rbegin()=='/')
    {
        m_sPath.erase(m_sPath.size()-1,1);
    }

    // read in and set up all the names we are required to log..
    // note ConfigureLogging will return true even if registration
    // for variables doesn't complete (i.e DB not connected)
    if(!ConfigureLogging())
        return false;


    //fetch file name to log to
    m_MissionReader.GetConfigurationParam("FILE",m_sStemFileName);








    //////////////////////////////
    //  now open the log files  //
    //////////////////////////////
    if(!OnNewSession())
        return false;

    return true;
}


bool CMOOSLogger::ConfigureLogging()
{

    //figure out what we are required to log....
    //here we read in what we want to log from the mission file..
    STRING_LIST Params;
	bool bHasMissionFile = true;
    if(m_MissionReader.GetConfiguration(m_sAppName,Params))
    {
        //this will make columns in sync log in order they
        //were declared in *.moos file
        Params.reverse();

        STRING_LIST::iterator p;
        for(p=Params.begin();p!=Params.end();p++)
        {
            string sParam = *p;
            string sWhat = MOOSChomp(sParam,"=");

            if(MOOSStrCmp(sWhat,"LOG"))
            {
                std::string sNewVar;
                HandleLogRequest(sParam,sNewVar);
            }
        }
    }
    else
    {
		bHasMissionFile = false;
        MOOSTrace("Warning:\n\tNo Configuration block was read - unusual but not terminal\n");
    }


    //are we allowing dynamic logging of variables is via PLOGGER_CMD message?
    int nNumDynamicVariables = m_MissionReader.IsOpen() ? 0 : 10;

    //this won't touch nNumDynamicVariables if mission file isn't open.
    m_MissionReader.GetConfigurationParam("DynamicSyncLogColumns",nNumDynamicVariables);

    if(nNumDynamicVariables>0)
    {
        MOOSTrace("Comment:\n\tReserving space for %d dynamic variables in slog\n",nNumDynamicVariables);
        for(int i = 0; i<nNumDynamicVariables;i++)
        {
            m_UnusedDynamicVariables.push_back(MOOSFormat("DYNAMIC_%d",i));
        }
    }

    //and generally turn on command message filtering at the CMOOSApp level
    EnableCommandMessageFiltering(true);

    //do we want wildcard logging - ie have the logger log every change...
    m_bWildCardLogging = false;
    if(bHasMissionFile)
	{
		m_MissionReader.GetConfigurationParam("WildcardLogging",m_bWildCardLogging);
	}
	else
	{
		m_bWildCardLogging = true;
	}

    if(m_bWildCardLogging )
        m_bAsynchronousLog = true;

    //ok so now lets register our interest in all these MOOS vars!
    if(!RegisterMOOSVariables())
        MOOSDebugWrite("Variable subscription is still pending - not terminal, but unusual");

    return true;

}

bool CMOOSLogger::HandleLogRequest(std::string sParam,std::string & sVar, bool bDynamic)
{
    sVar = MOOSChomp(sParam,"@");

    if(GetMOOSVar(sVar)!=NULL)
        return MOOSFail("Ignoring request to log %s - already requested\n",sVar.c_str());

    //do we want to monitor it?
    size_t iMonitor = sParam.find("MONITOR");
    if(iMonitor!=string::npos)
    {
        m_MonitorMap[sVar] = DEFAULT_MONITOR_TIME;
    }

    //now figure out where/if it should go in the synchronous log
    if(sParam.find("NOSYNC")==string::npos)
    {
        if(!bDynamic || m_UnusedDynamicVariables.size()>0)
        {
            //give it a default location...
            //and add to our list of syncronous vars
            m_SynchronousLogVars.push_back(sVar);
        }
    }


    string sFreq = MOOSChomp(sParam,",");

    double dfPeriod = MOOS_LOGGER_DEFAULT_PERIOD;
    if(!sFreq.empty())
    {
        dfPeriod = atof(sFreq.c_str());
    }

    //OK lets make a (internal) MOOS variable to hold this data
    AddMOOSVariable(sVar,sVar,"",dfPeriod);

    return true;
}

bool CMOOSLogger::Iterate()
{
    double dfTimeNow = MOOSTime();

    //look to do a synchronous log....
    if(m_bSynchronousLog)
    {
        if(dfTimeNow-m_dfLastSyncLogTime>m_dfSyncLogPeriod)
        {
            m_dfLastSyncLogTime = dfTimeNow;

            DoSyncLog(dfTimeNow);

            //finally everything is now stale..
            MOOSVARMAP::iterator q;

            for(q = m_MOOSVars.begin();q!=m_MOOSVars.end();q++)
            {
                q->second.SetFresh(false);
            }
        }
    }


    //check monitored variables
    if(dfTimeNow-m_dfLastMonitorTime>DEFAULT_MONITOR_TIME)
    {
        m_dfLastMonitorTime = dfTimeNow;
        VARIABLE_TIMER_MAP::iterator p;

        int nMissing  = 0;
        for(p = m_MonitorMap.begin();p!=m_MonitorMap.end();p++)
        {
            CMOOSVariable* pV = GetMOOSVar(p->first);
            if(pV)
            {
                double dfTolerance = p->second;
                if(pV->GetAge(dfTimeNow)>dfTolerance)
                {
                    MOOSTrace("Monitored Variable \"%s\" is not appearing\n",pV->GetName().c_str());
                    nMissing++;
                }
            }
        }
        if(nMissing>0)
        {
            MOOSDebugWrite(MOOSFormat("%d monitored variable%s not being logged\n",nMissing,nMissing==1?" is":"s are"));
        }

        //piggy back on this timer to publish current log directory
        m_Comms.Notify("LOGGER_DIRECTORY",m_sLogDirectoryName.c_str());
    }

    //are we requested to do wild card logging?
    if(m_bWildCardLogging)
        HandleWildCardLogging();


    //finally flush all files to be safe
    m_SyncLogFile.flush();
    m_AsyncLogFile.flush();
    m_SystemLogFile.flush();



    return true;
}

bool CMOOSLogger::HandleWildCardLogging()
{
    static double  dfLastWildCardTime = -1.0;

    if(MOOSTime()-dfLastWildCardTime>DEFAULT_WILDCARD_TIME)
    {
        MOOSMSG_LIST InMail;
        if(m_Comms.ServerRequest("VAR_SUMMARY", InMail, 2.0, false))
        {
            MOOSAssert(InMail.size()==1);
            std::string ss(InMail.begin()->GetString());
            bool bHit = false;
            while(!ss.empty())
            {
                std::string sVar = MOOSChomp(ss);
                if(GetMOOSVar(sVar)==NULL)
                {
                    if(AddMOOSVariable(sVar,sVar,"",0.0))
                    {
                        bHit = true;
                        MOOSTrace("   Added wildcard logging of %s\n",sVar.c_str());
                    }
                }
            }

            if(bHit)
                RegisterMOOSVariables();
        }

        dfLastWildCardTime = MOOSTime();
    }

    return true;
}


std::string CMOOSLogger::MakeLogName(string sStem)
{
    struct tm *Now;
    time_t aclock;
    time( &aclock );
    Now = localtime( &aclock );

    std::string  sTmp;

    if(m_bAppendFileTimeStamp)
    {
        // Print local time as a string

        //ODYSSEYLOG_14_5_1993_____9_30.log
        sTmp = MOOSFormat( "%s_%d_%d_%d_____%.2d_%.2d_%.2d",
            sStem.c_str(),
            Now->tm_mday,
            Now->tm_mon+1,
            Now->tm_year+1900,
            Now->tm_hour,
            Now->tm_min,
            Now->tm_sec);
    }
    else
    {
        sTmp = MOOSFormat("%s",sStem.c_str());
    }

    return sTmp;

}


bool CMOOSLogger::DoSyncLog(double dfTimeNow)
{

    //begin with time...
    m_SyncLogFile<<setw(COLUMN_WIDTH)<<setprecision(7)<<dfTimeNow-GetAppStartTime()<<' ';

    //now for all our variables...
    int nLogVarsSize = m_SynchronousLogVars.size();
    for(int nVar = 0; nVar<nLogVarsSize;nVar++)
    {
        string sVar = m_SynchronousLogVars[nVar];

        //oops empty string!
        if(sVar.empty())
            continue;

        //we want left justification
        m_SyncLogFile.setf(ios::left);

        //ok we have a name at column nVar...(numerical order retained in vector)
        MOOSVARMAP::iterator q = m_MOOSVars.find(sVar);

        if(q!=m_MOOSVars.end())
        {
            //OK so now we have the variable ..log it simply
            CMOOSVariable & rVar = q->second;

            m_SyncLogFile<<setw(COLUMN_WIDTH);

            //has this variable changed since last time?
            if(rVar.IsFresh())
            {
                //we can only write doubles
                if(rVar.IsDouble())
                {
                    m_SyncLogFile<<rVar.GetAsString(COLUMN_WIDTH).c_str()<<' ';
                }
                else
                {
                    //signify string variables or other types by NaN
                    //sync log is only for numbers
                    m_SyncLogFile<<"NaN"<<' ';
                }

                //we have used this variable so it is no longer fresh
                rVar.SetFresh(false);
            }
            else
            {
                //NO!
                m_SyncLogFile<<"NaN"<<' ';
            }


        }

    }


    //here we want to add columns for unclaimed dynamic variables;
    for(unsigned int i = 0; i<m_UnusedDynamicVariables.size();i++)
    {
            m_SyncLogFile<<setw(COLUMN_WIDTH);
            m_SyncLogFile<<"NaN"<<' ';
    }

    //put a new line in...
    m_SyncLogFile<<endl;

    //every few lines put a comment in
    if((m_nSyncLines++)%30==0)
        LabelSyncColumns();

    return true;
}


bool CMOOSLogger::OpenFile(std::ofstream & of,const std::string & sName)
{
    of.open(sName.c_str());

    if(!of.is_open())
    {
        string sErr = MOOSFormat("ERROR: Failed to open File: %s",sName.c_str());
        MOOSDebugWrite(sErr);
        return false;
    }

    return true;
}

bool CMOOSLogger::OpenSyncFile()
{
    if(!OpenFile(m_SyncLogFile,m_sSyncFileName))
        return MOOSFail("Failed to Open slog file");


    //be pretty
    DoBanner(m_SyncLogFile,m_sSyncFileName);


    //put a column of names and where they can be found
    m_SyncLogFile<<"%%   (1) TIME "<<endl;

    //now for all our variables say what the columns mean..
    int nCount = 2;
    int nLogVarsSize = m_SynchronousLogVars.size();
    for(int nVar = 0; nVar<nLogVarsSize;nVar++)
    {
        string sVar = m_SynchronousLogVars[nVar];

        //oops empty string!
        if(sVar.empty())
            continue;

        m_SyncLogFile<<"%%   ("<<nCount++<<") "<<sVar.c_str()<<endl;

    }


    //here we want to describe columns for unclaimed dynamic variables;
    STRING_LIST::iterator q;
    for( q = m_UnusedDynamicVariables.begin();q!=m_UnusedDynamicVariables.end();q++)
    {
        m_SyncLogFile<<"%%   ("<<nCount++<<") ";

        //remember where the next string will be..
        m_DynamicNameIndex[*q] =  m_SyncLogFile.tellp();

        //write the as yet unclaimed name - note we are also writing a tonne of spare space so we can
        //later acccess the file here and write a new MOOS variabl name..
        m_SyncLogFile<<setw(DYNAMIC_NAME_SPACE)<<left<<q->c_str()<<endl;
    }

    m_SyncLogFile<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;

    LabelSyncColumns();


    return true;
}


bool CMOOSLogger::OpenSystemFile()
{

    if(!OpenFile(m_SystemLogFile,m_sSystemFileName))
        return MOOSFail("Failed to Open system log file");


    DoBanner(m_SystemLogFile,m_sSystemFileName);

    return true;
}



bool CMOOSLogger::OpenAsyncFile()
{

    if(!OpenFile(m_AsyncLogFile,m_sAsyncFileName))
        return MOOSFail("Failed to Open alog file");

    DoBanner(m_AsyncLogFile,m_sAsyncFileName);

    return true;
}

bool CMOOSLogger::LogSystemMessages(MOOSMSG_LIST &NewMail)
{
    MOOSMSG_LIST::iterator p;

    m_SystemLogFile.setf(ios::left);

    double dfTimeNow = MOOSTime();

    for(p = NewMail.begin();p!=NewMail.end();p++)
    {
        CMOOSMsg & rMsg = *p;
        if(IsSystemMessage(rMsg.m_sKey) && !rMsg.IsSkewed(dfTimeNow))
        {

            m_SystemLogFile<<setw(10)<<setprecision(7)<<rMsg.m_dfTime-GetAppStartTime()<<' ';

            m_SystemLogFile<<setw(20)<<rMsg.m_sKey.c_str()<<' ';

            m_SystemLogFile<<setw(20)<<rMsg.m_sSrc.c_str()<<' ';

            if(rMsg.m_cDataType==MOOS_DOUBLE)
            {
                m_SystemLogFile<<setw(20)<<rMsg.m_dfVal<<' ';
            }
            else
            {
                MOOSRemoveChars(rMsg.m_sVal,"\n");
                m_SystemLogFile<<setw(20)<<rMsg.m_sVal.c_str()<<' ';
            }
            m_SystemLogFile<<endl;
        }
    }

    return true;
}

bool CMOOSLogger::IsSystemMessage(string &sKey)
{
    if (MOOSStrCmp(sKey,"MOOS_DEBUG")) return true;
    if (MOOSStrCmp(sKey,"MOOS_SYSTEM")) return true;

    return false;
}

bool CMOOSLogger::DoBanner(ostream &os, string &sFileName)
{
    os<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    os<<"%% LOG FILE:       "<<sFileName.c_str()<<endl;
    os<<"%% FILE OPENED ON  "<<MOOSGetDate().c_str();
    os<<"%% LOGSTART        "<<setw(20)<<setprecision(12)<<GetAppStartTime()<<endl;
    os<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";

    return true;
}

bool CMOOSLogger::LabelSyncColumns()
{
    //now put a header on each column...
    m_SyncLogFile.setf(ios::left);

    m_SyncLogFile<<setw(COLUMN_WIDTH)<<"%% TIME"<<' ';

    int nLogVarsSize = m_SynchronousLogVars.size();
    for(int nVar = 0; nVar<nLogVarsSize;nVar++)
    {
        string sVar = m_SynchronousLogVars[nVar];

        //oops empty string!
        if(sVar.empty())
            continue;

        m_SyncLogFile.setf(ios::left);

        m_SyncLogFile<<setw(COLUMN_WIDTH)<<sVar.c_str()<<' ';
    }

    //here we want to add columns for unclaimed dynamic variables;
    STRING_LIST::iterator q;
    for( q = m_UnusedDynamicVariables.begin();q!=m_UnusedDynamicVariables.end();q++)
    {
        m_SyncLogFile.setf(ios::left);
        m_SyncLogFile<<setw(COLUMN_WIDTH)<<q->c_str()<<' ';
    }


    m_SyncLogFile<<endl;

    //and add a line of times for good measure..
    AddSyncLineOfTimes(MOOSTime()-GetAppStartTime());

    return true;
}

bool CMOOSLogger::AddSyncLineOfTimes(double dfTimeNow)
{
    //now put a header on each column...
    m_SyncLogFile.setf(ios::left);

    m_SyncLogFile<<setw(COLUMN_WIDTH)<<"%% TIME"<<' ';

    string sNow = MOOSFormat("[%7.2f]",dfTimeNow);

    int nLogVarsSize = m_SynchronousLogVars.size()+ m_UnusedDynamicVariables.size();
    for(int nVar = 0; nVar<nLogVarsSize;nVar++)
    {
        m_SyncLogFile<<setw(COLUMN_WIDTH)<<sNow.c_str()<<' ';
    }


    m_SyncLogFile<<endl;

    return true;

}

bool CMOOSLogger::CreateDirectory(const std::string & sDirectory)
{

#if _WIN32
    int bOK  = ::CreateDirectory(sDirectory.c_str(),NULL);

    if(!bOK)
    {
        DWORD TheError = GetLastError();

        if(TheError!=ERROR_ALREADY_EXISTS)
        {

            LPVOID lpMsgBuf;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                TheError,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL
                );
            // Process any inserts in lpMsgBuf.
            // ...
            // Display the string.
            MOOSTrace("Error %ld  making directory :  \"%s\"\n",TheError,(LPCTSTR)lpMsgBuf);

            // Free the buffer.
            LocalFree( lpMsgBuf );

            return false;
        }

    }
#else
    if(mkdir(sDirectory.c_str(),0755)==-1)
    {
        switch(errno)
        {
        case EEXIST:
            break;
        default:
            MOOSTrace("Error %ld  making directory :  \"%s\"\n",errno,strerror(errno));
            return false;
        }
    }

#endif


    return true;
}

bool CMOOSLogger::OnNewSession()
{
    //Make a directory to hold the new files

    std::string sRoot = MakeLogName(m_sStemFileName);

    std::string sLogDirectory = m_sPath+"/"+sRoot;

    if(!CMOOSLogger::CreateDirectory(sLogDirectory))
    {
        MOOSTrace("Warning:\n\tFailed to create directory %s\n",sLogDirectory.c_str());

        sLogDirectory = "./"+sRoot;
        MOOSTrace("\tfalling back to creating %s...",sLogDirectory.c_str());
        if(!CMOOSLogger::CreateDirectory(sLogDirectory))
        {
            return MOOSFail("Failed to create a logging directory either in specified path or locally\n");
        }
        else
        {
            MOOSTrace("OK\n");
        }
    }

    //looks safe remember it...
    m_sLogDirectoryName = sLogDirectory;

    //and publish it
    m_Comms.Notify("LOGGER_DIRECTORY",m_sLogDirectoryName.c_str());

    m_sAsyncFileName = m_sLogDirectoryName+"/"+sRoot+".alog";
    m_sSyncFileName = m_sLogDirectoryName+"/"+sRoot+".slog";
    m_sSystemFileName = m_sLogDirectoryName+"/"+sRoot+".ylog";
    m_sMissionCopyName = m_sLogDirectoryName+"/"+sRoot+"._moos";
    m_sHoofCopyName = m_sLogDirectoryName+"/"+sRoot+"._hoof";

    if(!OpenAsyncFile())
        return MOOSFail("Error:\n\tUnable to open Asynchronous log file\n");

    if(! OpenSyncFile())
        return MOOSFail("Error:\n\tUnable to open Synchronous log file\n");

    if(!OpenSystemFile())
        return MOOSFail("Error:\n\tUnable to open System log file\n");

    if(!CopyMissionFile())
        MOOSTrace("Warning:\n\tunable to create a back up of the mission file\n");

    return true;
}


std::string GetDirectoryName(const std::string & sStr)
{
    std::string sT = sStr;
    std::string sD;

    while(!sT.empty())
        sD = MOOSChomp(sT,"/");

    return sD;
}

bool CMOOSLogger::OnLoggerRestart()
{
    string sTxt = MOOSFormat("Closing : %s",GetDirectoryName(m_sLogDirectoryName).c_str());
    MOOSDebugWrite(sTxt);

    CloseFiles();

    //start up fresh
    if(!OnNewSession())
        return MOOSFail("Failed to start a new logging session");

    MOOSDebugWrite(MOOSFormat("Now Logging to : %s",GetDirectoryName(m_sLogDirectoryName).c_str()));

    return true;
}

bool CMOOSLogger::DoAsyncLog(MOOSMSG_LIST &NewMail)
{
    //log asynchronously...
    if(m_AsyncLogFile.is_open()&& m_bAsynchronousLog)
    {
        MOOSMSG_LIST::iterator q;
        for(q = NewMail.begin();q!=NewMail.end();q++)
        {
            CMOOSMsg & rMsg = *q;

            //now see if we are logging this kind of message..
            //if so we will have a variable named after it...
            //which is used for the synchronous case..
            if(m_MOOSVars.find(rMsg.m_sKey)!=m_MOOSVars.end())
            {
				m_AsyncLogFile.setf(ios::left);
                m_AsyncLogFile.setf(ios::fixed);

                m_AsyncLogFile<<setw(15)<<setprecision(3)<<rMsg.GetTime()-GetAppStartTime()<<' ';

                m_AsyncLogFile<<setw(20)<<rMsg.GetKey().c_str()<<' ';

                m_AsyncLogFile<<setw(10)<<rMsg.GetSource().c_str()<<' ';

                m_AsyncLogFile<<rMsg.GetAsString().c_str()<<' ';

                m_AsyncLogFile<<endl;
            }
        }
    }
    return true;
}

bool CMOOSLogger::CopyMissionFile()
{
    //open the original
    ifstream MissionFile;
    MissionFile.open(m_sMissionFile.c_str());
    if(!MissionFile)
        return false;


    //open a copy file
    ofstream MissionCopy;

    if(!OpenFile(MissionCopy,m_sMissionCopyName))
        return MOOSFail("Failed to open a copy of mission file");

    //write a banner
    DoBanner(MissionCopy,m_sMissionCopyName);

    //do the copy..
    while(!MissionFile.eof())
    {
        char Tmp[10000];
        MissionFile.getline(Tmp,sizeof(Tmp));
        string sLine = string(Tmp);

        MissionCopy<<sLine.c_str()<<endl;
    }
    MissionCopy.close();
    MissionFile.close();



    //now look for hoof files!
    CProcessConfigReader HelmReader;
    HelmReader.SetFile(m_sMissionFile);
    HelmReader.SetAppName("pHelm");

    string sHoof;
    if(HelmReader.GetConfigurationParam("TaskFile",sHoof))
    {

        //open a copy file
        ofstream HoofCopy;

        if(!OpenFile(HoofCopy,m_sHoofCopyName))
            return MOOSFail("Failed to copy of mission file");

        //open the original
        ifstream HoofFile;
        HoofFile.open(sHoof.c_str());

        if(HoofFile.is_open() && HoofCopy.is_open())
        {

            //do the copy..
            while(!HoofFile.eof())
            {
                char Tmp[10000];
                HoofFile.getline(Tmp,sizeof(Tmp));
                string sLine = string(Tmp);

                HoofCopy<<sLine.c_str()<<endl;
            }

            HoofCopy.close();
            HoofFile.close();
        }
    }



    return true;

}

bool CMOOSLogger::OnCommandMsg(CMOOSMsg Msg)
{
    if(Msg.IsSkewed(MOOSTime()))
        return true;

    if(!Msg.IsString())
        return MOOSFail("pLogger only accepts string command messages\n");

    std::string sCmd = Msg.GetString();

    //OK lets look for dynamic log messages:
    std::string sTask,sParam;
    m_MissionReader.GetTokenValPair(sCmd,sTask,sParam);

    if(MOOSStrCmp(sTask,"LOG_REQUEST"))
    {
        HandleDynamicLogRequest(sParam);
    }
    else if(MOOSStrCmp(sTask,"COPY_FILE_REQUEST"))
    {
        HandleCopyFileRequest(sParam);
    }
    else
    {
        return MOOSFail("Dynamic command %s is not supported\n",sTask.c_str());
    }

    return true;
}



bool CMOOSLogger::HandleCopyFileRequest(std::string sFileToCopy)
{
    std::string sPath,sFile,sExtension;

    MOOSFileParts(sFileToCopy,sPath,sFile,sExtension);

    //alter extension to show its been harvested by the logger
    //similar to the way *.moos ->*._moos
    if(sExtension.empty())
    {
        sExtension = "_bak";
    }
    else
    {
        sExtension = "_"+sExtension;
    }

    std::string sBackUpName =  m_sLogDirectoryName+"/"+sFile+"."+sExtension;

    std::ifstream In(sFileToCopy.c_str());

    if(!In.is_open())
    {
        MOOSDebugWrite("failed to open file for reading");
        return false;
    }

    std::ofstream Out(sBackUpName.c_str());
    if(!Out.is_open())
    {
        MOOSDebugWrite("failed to open file for writing");
        return false;
    }

    //you've gotta lurve C++ ...
    Out<<In.rdbuf();

    Out.close();
    In.close();

    MOOSTrace("copied file to %s\n",sBackUpName.c_str());


    return true;
}

bool CMOOSLogger::HandleDynamicLogRequest(std::string sRequest)
{
    std::string sNewVar;
    if(HandleLogRequest(sRequest,sNewVar,true))
    {
        //now the above will have added the variable to async logs but maybe not to
        //sync logs - there maynot be space - but that the price you pay for not talking to each other
        //users can set DynamicSyncLogColumns = N to reserve space
        if(!m_UnusedDynamicVariables.empty())
        {
            //looks like we have space...so reserved slog columns is still filling up
            //but we now have one less column as yet unclaimed
            std::string sDynamic = m_UnusedDynamicVariables.front();

            //remember where the next string will be..
            if(m_DynamicNameIndex.find(sDynamic)==m_DynamicNameIndex.end())
            {
                MOOSAssert("this is a logical error - call PMN");
            }


            //better remember where we are...
            std::streampos pNow =  m_SyncLogFile.tellp();

            //where do we write the new name to?
            std::streampos p = m_DynamicNameIndex[sDynamic];

            //go there...
            m_SyncLogFile.seekp(p);

            //write the as yet unclaimed name - note we are also writing a tonne of spare space so we can
            //later acccess the file here and write a new MOOS variable name..
            if(sNewVar.size()>DYNAMIC_NAME_SPACE-1)
            {
                //we only reserved a limited amount of space...
                MOOSTrace("warning:\n\t name  \"%s\" will be truncated to %d characters in the slog\n",
                    sNewVar.c_str(),
                    DYNAMIC_NAME_SPACE-1);
            }

            m_SyncLogFile<<setw(DYNAMIC_NAME_SPACE-1)<<left<<sNewVar;;

            //and return from whence you came....
            m_SyncLogFile.seekp(pNow);

            //pop the name of a now used, dynamic variable
            m_UnusedDynamicVariables.pop_front();
        }

        //indicate column semantics have changed
        LabelSyncColumns();

        //register to receive notifications on this
        RegisterMOOSVariables();

        MOOSTrace("Processed dynamic log request on \"%s\". \nSpace for %d more such requests before slog becomes full\n",sNewVar.c_str(),m_UnusedDynamicVariables.size());
    }
    else
    {
        MOOSDebugWrite("failed to process dynamic log request  - see pLogger output\n");
        return false;
    }

    return true;


}
