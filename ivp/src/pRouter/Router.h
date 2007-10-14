//////////////////////////////////////////////////////////////////////////
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
//////////////////////////    END_GPL    ////////////////////////////////
//
// Router.h: interface for the CRouter class.
//
/////////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

//#define MAXQUEUES (16)
//changed due to segmentation fault
//127 is the max WHOI umodems support FSK
#define MAXQUEUES (127)

typedef struct 
{
	std::string sHuxleyFrame;
	std::string sCommandFrame;
	std::string sRecorderFrame;
	std::string sProsecuteFrame;
	std::string sDeployFrame;
	std::string sXRayBehaviorFrame;
	std::string sTrackFrame;
	std::string sContactFrame;
	std::string sNoiseFrame;


	std::string sStatusFrame;
	std::string sXRayStatusFrame;
	std::string sOtherFrame;
} tFrameQueue;
	


class CRouter : public CMOOSApp  
{
public:
	CRouter();
	virtual ~CRouter();

protected:

	//basic MOOS Stuff
	bool Initialise();
	bool Iterate();
        bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();
        
        // output functions
        bool SendFrameToModem();
	bool SendFrameToFramer();

	// input functions
        bool OnModemData(std::string sData);
	bool OnFramerFrame(std::string sFrame);

	// priority function
	std::string GetBestFrame(int iDest);
	int DetHighestPrio(double* , bool*, int);

        // interface: variable names
	std::string m_sModemDataName;        // subscribe to this
	std::string m_sModemCommandName;     // write to this
	std::string m_sFramerOutputName;     // subscribe to this
	std::string m_sFramerInputName;      // write to this

	// state
	std::string m_sLastModemData;        // modem messages land here
	std::string m_sLastFramerFrame;      // framer messages land here

	// the frame queues
	tFrameQueue m_tFrameQ[MAXQUEUES];

	// Message Priorityweights
	double Track_prio;
	double Contact_prio;
	double Noise_prio;
	double Status_prio;

	double track_mark;
	double contact_mark;
	double noise_mark;
	double status_mark;

	double memory_time;
	double prev_msg_time ;
	bool ack_val;

	    double wgt[4];
	    bool exist[4];
	    
};
