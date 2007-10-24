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
// Framer.h: interface for the CFramer class.
//
//////////////////////////////////////////////////////////////////////

#define VERSION_NAME "pFramer"
#define VERSION_MAJOR (0)
#define VERSION_MINOR (77)

#define MAXFRAMESIZE  (256)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

typedef std::vector<std::string> STRING_VECTOR;
typedef std::vector<int> INT_VECTOR;

class CFramer : public CMOOSApp  
{
public:
	CFramer();

	virtual ~CFramer();
	typedef struct {
		int iSrc;
		int iDest;
		int bAck;
		int bValid;
		int iType;
		int iFrameNum;
		unsigned char caData[MAXFRAMESIZE];
		int iSize;
	} sFrame;

protected:
	bool Initialise();
	bool Iterate();
        bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();	

	// deal with the frames
	bool OnFrame(const std::string & sFrame);
	bool SendFrame();

	// message parsers
	bool OnMessage(const std::string & sMessage);
	bool OnMessageHuxley(const std::string & sMessage);
	bool OnMessageRecorder(const std::string & sMessage);
	bool OnMessageCommand(const std::string & sMessage);
	bool OnMessageXRayStatus(const std::string & sMessage);
	bool OnMessageXRayBehavior(const std::string & sMessage);
	bool OnMessagePlusnetReport(const std::string & sMessage);
	bool OnMessagePlusnetCommand(const std::string & sMessage);

	// message makers
	bool SendMessage();
	bool SendMessageHuxley();
	bool SendMessageCommand();
	bool SendMessageRecorder();
	bool SendMessageXRayStatus();
	bool SendMessageXRayBehavior();
	bool SendMessagePlusnetReport();
	bool SendMessagePlusnetCommand();
	bool SendMessageUnknown(); // pass unknown types along 

	int HexToData(char *hex, unsigned char *buf);
	int DataToHex(unsigned char *buf, int nbytes, char *hex);
	
        // config
	std::string m_sInputMsgName;
	std::string m_sOutputMsgName;
	std::string m_sInputFrameName;
	std::string m_sOutputFrameName;
	bool m_bDoAckHuxley;
	bool m_bDoAckCommands;
	bool m_bDoAckStatus;

	// state
        double m_dfLastMailTime;
        bool m_bRecentlyWarned;
	sFrame m_sTheFrame;
	double m_dLastMessageTimestamp;
	std::string m_sLastMessageType;
	
};
