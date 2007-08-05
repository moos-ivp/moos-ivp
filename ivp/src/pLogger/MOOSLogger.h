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
// MOOSLogger.h: interface for the CMOOSLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSLOGGER_H__EF526A96_BC2B_47DB_AE59_C7D1BC56FA8F__INCLUDED_)
#define AFX_MOOSLOGGER_H__EF526A96_BC2B_47DB_AE59_C7D1BC56FA8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MOOS_LOGGER_DEFAULT_PERIOD  2.0
#include <fstream>
#include <set>
#include <string>

typedef std::vector<std::string> STRING_VECTOR; 

class CMOOSLogger : public CMOOSApp  
{
public:
	CMOOSLogger();
	virtual ~CMOOSLogger();


    /** virtual overide of base class CMOOSApp member. Here we do all the processing and IO*/
	bool Iterate();

    /** virtual overide of base class CMOOSApp member. Here we register for data we wish be
    informed about*/
	bool OnConnectToServer();

    bool OnNewMail(MOOSMSG_LIST &NewMail);

    bool OnStartUp();

protected:
	bool CopyMissionFile();
	bool ConfigureLogging();
	bool DoAsyncLog(MOOSMSG_LIST & NewMail);
	bool OnLoggerRestart();
	bool AddSyncLineOfTimes(double dfTimeNow=-1);
	bool LabelSyncColumns();
	bool DoBanner(std::ostream & os,std::string & sFileName);
	bool IsSystemMessage(std::string & sKey);
	bool LogSystemMessages(MOOSMSG_LIST & NewMail);
	bool OpenAsyncFile();
	bool OpenSystemFile();
    bool CloseFiles();
	bool OpenSyncFile();
	bool DoSyncLog(double dfTimeNow);
    std::string MakeLogName(std::string sStem);
    bool OpenFile(std::ofstream & of,const std::string & sName);
    bool OnNewSession();
    bool CreateDirectory(const std::string & sDirectory);

	std::ofstream m_AsyncLogFile;
    std::ofstream m_SyncLogFile;
    std::ofstream m_SystemLogFile;

    std::string m_sAsyncFileName;
    std::string m_sSyncFileName;
    std::string m_sSystemFileName;
    std::string m_sMissionCopyName;
    std::string m_sHoofCopyName;

    std::string m_sPath;
    std::string m_sStemFileName;
    std::string m_sLogDirectoryName;
    
    STRING_VECTOR m_SynchronousLogVars;
    bool    m_bSynchronousLog;
    bool    m_bAsynchronousLog;
    int     m_nSyncLines;

    ///true if we want fancy date appended to file name
    bool    m_bAppendFileTimeStamp;

    double m_dfLastSyncLogTime;
    double m_dfSyncLogPeriod;
    double m_dfLastMonitorTime;


    typedef std::map< std::string, double > VARIABLE_TIMER_MAP;
    VARIABLE_TIMER_MAP m_MonitorMap;


};

#endif // !defined(AFX_MOOSLOGGER_H__EF526A96_BC2B_47DB_AE59_C7D1BC56FA8F__INCLUDED_)
