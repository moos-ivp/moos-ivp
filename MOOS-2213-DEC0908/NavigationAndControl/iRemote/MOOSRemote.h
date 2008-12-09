///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a  MOOS Basic (Common) Application.
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


// MOOSRemote.h: interface for the CMOOSRemote class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(iRemoteh)
#define iRemoteh


class CMOOSRemote : public CMOOSApp
{
public:
	bool MakeHeading();
	bool DoNavSummary(CMOOSMsg & Msg);
	class CCustomKey
	{
	public:
		CCustomKey(){bIsNumeric = false;};
		char m_cChar;
		std::string m_sKey;
		std::string m_sVal;
		bool bIsNumeric;
		bool bAskToConfirm;
	};
	typedef std::map< char ,CCustomKey> CUSTOMKEY_MAP;

	class CJournal
	{
	public:
		CJournal(){};
		CJournal(const std::string & sName, int nSize,char cKey)
		{
			m_nMaxSize=nSize;
			m_cKey = cKey;
			m_sName = sName;
		};
		STRING_LIST m_Entries;
		char m_cKey;
		unsigned int m_nMaxSize;
		std::string m_sName;
		void Add(const std::string & sEntry)
		{
			m_Entries.push_back(sEntry);
			while(m_Entries.size()>m_nMaxSize)
			{
				m_Entries.pop_front();
			}
		}
	};

	typedef std::map< std::string ,CJournal> CUSTOMJOURNAL_MAP;


    class CNavVar
    {
    public:
        CNavVar(){};
        CNavVar(std::string sName,std::string sPrint ,double dfS)
        {
            m_sMessageName = sName;
            m_sPrintName = sPrint;
            m_dfScaleBy = dfS;
            m_dfVal = 0;
            m_dfTime = -1;
        }
    public:
        std::string m_sMessageName;
        std::string m_sPrintName;
        double m_dfScaleBy;
        double m_dfVal;
        double m_dfTime;
    };
    typedef std::map< std::string ,CNavVar> NAVVAR_MAP;



	bool WDLoop();
    bool MailLoop();
	CMOOSRemote();
	virtual ~CMOOSRemote();
    bool Run( const char * sName,const char * sMissionFile);

    double m_dfCurrentElevator;
    double m_dfCurrentRudder;
    double m_dfCurrentThrust;
    double m_dfTimeLastSent;
    bool   m_bManualOveride;

    bool   m_bRunMailLoop;



protected:
	bool PrintCustomSummary();
	bool MakeCustomSummary();
	bool MakeCustomKeys();
	bool MakeCustomJournals();


	bool HandleSAS();
	bool ReStartAll();
	bool MakeWayPoint();
	bool HandleThirdPartyTask();
	bool HandleScheduler();
	bool PrintNavSummary();
	bool EnableMailLoop(bool bEnable);
	bool HandleTopDownCal();
	bool DoCustomKey(char cCmd);
	bool DoCustomJournal(char cCmd);
	bool HitInputWD();
	bool DoWiggle();
	bool SetManualOveride(bool bOveride);
	virtual bool OnConnectToServer();
	bool OnStartUp();
    bool    StopThreads();
    bool    StartThreads();
    bool FetchDB();
    bool    m_bQuit;
    double m_dfTimeLastAck;

    STRING_LIST m_CustomSummaryList;

    /** Win32 handle to IO thread */
    #ifdef _WIN32
	    HANDLE m_hWDThread;
    #endif
	/** ID of IO thread */
	unsigned long		m_nWDThreadID;

    /** Win32 handle to text print thread */
    #ifdef _WIN32
	    HANDLE m_hMailThread;
    #endif
	/** ID of Mail reading thread */
	unsigned long		m_nMailThreadID;

    CMOOSLock m_Lock;
	bool SendDesired();
	void PrintHelp( );

	CUSTOMKEY_MAP m_CustomKeys;

	// a map of custom journals
	CUSTOMJOURNAL_MAP m_CustomJournals;
    //a map of nav vars used to keep a record of where
    //vehicle is...
    NAVVAR_MAP m_NavVars;

};

#endif
