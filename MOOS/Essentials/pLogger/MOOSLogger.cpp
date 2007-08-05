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
#include <math.h>

#ifndef _WIN32
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif


using namespace std;
#include "MOOSLogger.h"

//maximum of logged columns...
#define MAX_SYNC_COLUMNS 100
#define COLUMN_WIDTH 18
#define DEFAULT_MONITOR_TIME 10.0
#define MIN_SYNC_LOG_PERIOD 0.1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSLogger::CMOOSLogger()
{
    SetAppFreq(5);
    
    
    m_sStemFileName = "MOOS";
    
    m_bAsynchronousLog = true;
    m_bSynchronousLog = true;
    m_dfLastSyncLogTime = 0;
    m_dfSyncLogPeriod = 1.0;

    m_dfLastMonitorTime = MOOSTime();
    
    
    m_bAppendFileTimeStamp = false;
    m_nSyncLines = 0;
    
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
    m_bSynchronousLog = false;
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
    m_bAsynchronousLog = false;
    if(m_MissionReader.GetConfigurationParam("ASYNCLOG",sTmp))
    {
        if(MOOSStrCmp(sTmp,"TRUE"))
        {
            m_bAsynchronousLog = true;
        }
    }
    
    //what sort of file name are we using
    if(m_MissionReader.GetConfigurationParam("FILETIMESTAMP",sTmp))
    {
        if(MOOSStrCmp(sTmp,"TRUE"))
        {
            m_bAppendFileTimeStamp = true;
        }
    }
    
    //do we have a path global name?
    if(!m_MissionReader.GetValue("GLOBALLOGPATH",m_sPath))
    {
        //read path name
        if(!m_MissionReader.GetConfigurationParam("PATH",m_sPath))
        {
            return MOOSFail("Error neither \"::GlobalLogPath\" or \"Path\" are specified");
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
                string sVar = MOOSChomp(sParam,"@");
          
                //do we want to monitor it?
                int iMonitor = sParam.find("MONITOR");
                if(iMonitor!=string::npos)
                {
                    m_MonitorMap[sVar] = DEFAULT_MONITOR_TIME;
                }
                
                //now figure out where/if it should go in the synchronous log
                if(sParam.find("NOSYNC")==string::npos)
                {
                    //give it a default location...
                    //and add to our list of syncronous vars
                    m_SynchronousLogVars.push_back(sVar);
                }


                string sFreq = MOOSChomp(sParam,",");
                
                double dfPeriod = MOOS_LOGGER_DEFAULT_PERIOD;
                if(!sFreq.empty())
                {
                    dfPeriod = atof(sFreq.c_str());
                }
                
                //OK lets make a (internal) MOOS variable to hold this data
                AddMOOSVariable(sVar,sVar,"",dfPeriod);
                
                
          
            }            
        }
    }
    //ok so now lets register our interest in all these MOOS vars!
    if(!RegisterMOOSVariables())
		MOOSDebugWrite("variable subscription is still pending - not terminal, but unusual");

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
    
    
    
    
    
    m_SyncLogFile.flush();
    m_AsyncLogFile.flush();
    m_SystemLogFile.flush();
    
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
    
    bool bWrittenTime=false;
    
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
        
        if(!bWrittenTime)
        {
            //begin with time...
            m_SyncLogFile<<setw(COLUMN_WIDTH)<<setprecision(7)<<dfTimeNow-GetAppStartTime()<<' ';
            bWrittenTime = true;
        }
        
        
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
        string sErr = MOOSFormat("ERROR: Failed to open Log File: %s",sName.c_str());
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
    m_SyncLogFile<<endl;
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
    
    int nLogVarsSize = m_SynchronousLogVars.size();
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
        return MOOSFail("Failed to create directory %s\n",sLogDirectory.c_str());
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
    
    return (
        OpenAsyncFile() &&
        OpenSyncFile()&&
        OpenSystemFile()&&
        CopyMissionFile()
        );
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
    //log asynchrounously...
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
    //open a copy file
    ofstream MissionCopy;
    
    if(!OpenFile(MissionCopy,m_sMissionCopyName))
        return MOOSFail("Failed to copy of mission file");    
    
    //open the original
    ifstream MissionFile;
    MissionFile.open(m_sMissionFile.c_str());
    
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
