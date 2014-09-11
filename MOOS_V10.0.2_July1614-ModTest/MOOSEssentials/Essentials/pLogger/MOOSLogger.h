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

#if !defined(MOOSLOGGERH)
#define MOOSLOGGERH

#include <fstream>
#include <set>
#include <string>
#include "Zipper.h"

typedef std::vector<std::string> STRING_VECTOR; 

class CMOOSLogger : public CMOOSApp  
{
public:
    /* vanilla construction and destruction*/
    CMOOSLogger();
    virtual ~CMOOSLogger();


    /** virtual overide of base class CMOOSApp member. Here we do all the processing and IO*/
    bool Iterate();

    /** virtual overide of base class CMOOSApp member. Here we register for data we wish be
    informed about*/
    bool OnConnectToServer();

    /** called when new mail arrives */
    bool OnNewMail(MOOSMSG_LIST &NewMail);

    /** called just as the application is starting up*/
    bool OnStartUp();

    /** called when  command mesage is recieved  - use this channle for dynamic logging*/
    bool OnCommandMsg(CMOOSMsg Msg);

	/** a endless loop looking to compress data to a zip file (if required) */
	bool DoZipLogging();
	
	/** call to shut everything down and exit cleanly */
	bool ShutDown();


protected:

    /** called to set up machinery to log a variable geiven a command string Log = Var @ etc*/
    bool HandleLogRequest(std::string sParam,std::string &sNewVariable, bool bDynamic= false);
    bool HandleDynamicLogRequest(std::string sRequest);
    bool HandleCopyFileRequest(std::string sFileToCopy);
    bool HandleWildCardLogging();
    bool IsWildCardAccepted(const std::string & sVariableName) const;
    bool IsWildCardRejected(const std::string & sVariableName) const;

    bool CopyMissionFile();
    bool ConfigureLogging();
    bool DoAsyncLog(MOOSMSG_LIST & NewMail);
    bool OnLoggerRestart();
    bool AddSyncLineOfTimes(double dfTimeNow=-1);
    bool LabelSyncColumns();
    bool DoLogBanner(std::ostream & os,std::string & sFileName);
    bool IsSystemMessage(std::string & sKey);
    bool LogSystemMessages(MOOSMSG_LIST & NewMail);
    bool OpenAsyncFiles();
    bool OpenSystemFile();
    bool CloseFiles();
    bool OpenSyncFile();
    bool DoSyncLog(double dfTimeNow);
    std::string MakeLogName(std::string sStem);
    bool OpenFile(std::ofstream & of,const std::string & sName, bool bBinary = false);
    bool OnNewSession();
    bool CreateDirectory(const std::string & sDirectory);
    std::string MakeStatusString();

    std::ofstream m_AsyncLogFile;
    std::ofstream m_ExcludeLogFile;
    std::ofstream m_SyncLogFile;
    std::ofstream m_SystemLogFile;
    std::ofstream m_BinaryLogFile;

	
	std::string m_sLogRootName;
    std::string m_sAsyncFileName;
	std::string m_sExcludeFileName;
    std::string m_sSyncFileName;
    std::string m_sSystemFileName;
    std::string m_sBinaryFileName;

    std::string m_sMissionCopyName;
    std::string m_sHoofCopyName;

    std::string m_sPath;
    std::string m_sStemFileName;
    std::string m_sLogDirectoryName;
    
    STRING_VECTOR m_SynchronousLogVars;
    STRING_LIST m_UnusedDynamicVariables;
    bool    m_bSynchronousLog;
    bool    m_bAsynchronousLog;
    bool    m_bWildCardLogging;
	bool    m_bUseExcludedLog;
	bool    m_bMarkExternalCommunityMessages;
	bool    m_bMarkDataType;

	//variables to do with compressed logging...
	bool	m_bCompressAlog;
	CZipper m_AlogZipper;
	CZipper m_XlogZipper;
	
	
    //how many synline have been written?
    int     m_nSyncLines;

    ///true if we want fancy date appended to file name
    bool    m_bAppendFileTimeStamp;
	
	///true if we want to create log directories with UTC time stamps
	bool m_bUseUTCLogNames;

	//.true if we want to log AuxSrc varaibles
	bool m_bLogAuxSrc;

    //housekeeping  variables for performing tasks
    double m_dfLastSyncLogTime;
    double m_dfSyncLogPeriod;
    double m_dfLastMonitorTime;
	
	//what precision do we log doubles with?
	int m_nDoublePrecision;
    
    //name of a file where logger summary is written
    std::string     m_sSummaryFile;
	
	//current position of bianry file pointer
	std::streampos m_BinaryCursor;


    //housekeeping variables for checking that monotired messages
    //rally are being written to...
    typedef std::map< std::string, double > VARIABLE_TIMER_MAP;
    VARIABLE_TIMER_MAP m_MonitorMap;
	
	//house keeping functions and storage to keeptrack of which variable
	//goes to which log (especially when xlogging is on)
	enum LogType
	{
		XLOG,
		YLOG,
		SLOG,
		ALOG,
		UNKNOWN
	};
	
	CMOOSLogger::LogType GetDestinationLog(const std::string & sStr);
	std::map<std::string, LogType> m_LogDestinations;

private:
    //this is a collection of file position pointers which we will use
    //to fill in column names for dynamically registered variables
    //as and when they come in (housekeeping for the header block on
    //slog files)
    std::map< std::string, std::streampos > m_DynamicNameIndex;
    
    // collection of strings which specify names (can use wild card * and ? ) which
    // should be ommited from dynamic logging
    std::vector< std::string >  m_sWildCardOmitted;

    
    // collection of strings which specify names (can use wild card * and ? ) which
    // should be allowed in dynamic logging. If empty all strings are assumed to be wanted
    // unless they are in m_sDynamicMasked
    std::list< std::string >  m_sWildCardAccepted;
    
};

#endif 
