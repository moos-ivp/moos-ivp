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
//   This file is part of a  MOOS Instrument. 
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
// MicroModemInstrument.h: interface for the CMicroModemInstrument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MicroModemINSTRUMENT_H__1233F956_765B_48B3_BDB5_0255B563F4B3__INCLUDED_)
#define AFX_MicroModemINSTRUMENT_H__1233F956_765B_48B3_BDB5_0255B563F4B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "umodem.h"

typedef std::vector<std::string> STRING_VECTOR;
typedef std::vector<int> INT_VECTOR;
typedef std::list<std::string> STRING_LIST;

class CMicroModemInstrument : public CMOOSInstrument  
{
public:

	// methods
	CMicroModemInstrument();
	virtual ~CMicroModemInstrument();
	bool OnConnectToServer();
	bool OnStartUp();	
	bool InitialiseSensor();
	bool Iterate();
	bool ConfigIterate();
	bool GetModemMessage();
	bool ParseModemMessage();
        bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnModemData();
	bool OnPingReply();
	bool OnCommand(const std::string &sVal);
	bool CheckTimeouts();
	bool PublishLastQoS();
	bool OnModemConfig(char *caName,int nValue);
	bool SetModemClock();

	// context
	sDataFrame m_sRxFrame;
	sDataFrame m_sTxFrame;
	bool m_bHaveTxFrame;
	std::string sLastModemMessage;
	int  m_nDefaultDestinationAddress;
	int  m_nThisModemAddress;
	int  m_iLastModemMsgType;
	int  m_iOldModemStatus;
	double dtLastGood[UMODEM_MAXNODES];
	
	// configurable MOOS variable names, generic prefix "MICROMODEM" can be changed.
	std::string m_sVarNamePrefix;   // the prefix to pre-pend to all the variable names
	std::string m_sDataVarName;     
	std::string m_sCommandVarName;
	std::string m_sRawVarName;
	std::string m_sServiceVarName;
	std::string m_sToModemVarName;
	std::string m_sFromModemVarName;
	std::string m_sStatusVarName;
	std::string m_sStatusCodeVarName;
	int  vehicleID;

	// configure the modem
	STRING_VECTOR m_svSettingNames;  // settings to write to modem
	INT_VECTOR m_ivSettingValues;    // settings to write to modem
	int m_iNumSettings;
	int m_iCurrentSetting;
	STRING_VECTOR m_svConfigNames;   // config replies from modem
	INT_VECTOR m_ivConfigValues;     // config replies from modem
	int m_iNumConfigs;
	bool m_bIsConfigured;
	
protected:

private:
	bool DoSubscribe();
	bool DoFormVarNames();
	bool SendModemString(char *caStr);
};

#endif // !defined(AFX_MicroModemINSTRUMENT_H__1233F956_765B_48B3_BDB5_0255B563F4B3__INCLUDED_)
