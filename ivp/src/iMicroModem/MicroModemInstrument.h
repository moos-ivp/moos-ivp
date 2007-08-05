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

#include "ccl.h"

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
	bool GetModemMessage();
	bool ParseModemMessage();
        bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool DoDecodeFrame(sDataFrame *f);
	CCL_contactxy_t * MakeContactXYFromQ();
	CCL_statexy_t * MakeStateXYFromDB();
	CCL_navxy_t * MakeNavXYFromDB();
	bool OnModemData();
	bool OnDataRequest();
	bool OnPingReply();
	bool OnTrackerFix(const std::string &sVal);
	bool OnCommand(const std::string &sVal);
        bool CheckTimeouts();
        bool PublishLastQoS();

	// context
	sDataFrame m_sRxFrame;
	std::list<CCL_contact_det_t> m_ContactsToTx;
	std::list<sDataFrame> m_ForwardsToTx;
	std::string sLastModemMessage;
	std::string m_sDefaultTxType;
	CCL_statexy_t m_sStateXY;
	CCL_navxy_t m_sNavXY;
	CCL_contactxy_t m_sContactXY;
	int  m_nDefaultDestinationAddress;
	int  m_nThisModemAddress;
	int  iLastModemMsgType;
	int  iOldModemStatus;
	bool  m_bDefaultUseNAVXY;
        double dtLastGood[UMODEM_MAXNODES];
protected:

};

#endif // !defined(AFX_MicroModemINSTRUMENT_H__1233F956_765B_48B3_BDB5_0255B563F4B3__INCLUDED_)
