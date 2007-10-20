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
// Router.cpp: implementation of the CRouter class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include "Router.h"
#include "version.h"

#include <time.h>
#include <iostream>

using namespace std;

#define DEFAULT_CSV_LOG_INTERVAL (3600) // seconds => 1 hour

//By Arjuna 10/6/2007
#define DONTSORT (1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRouter::CRouter()
{
	int l;
	
	// init all queues to ""
	for(l=0;l<MAXQUEUES;l++){

		m_tFrameQ[l].sCommandFrame="";
		m_tFrameQ[l].sDeployFrame="";
		m_tFrameQ[l].sProsecuteFrame="";
		m_tFrameQ[l].sTrackFrame="";
		m_tFrameQ[l].sStatusFrame="";
		m_tFrameQ[l].sNoiseFrame="";
		m_tFrameQ[l].sHuxleyFrame="";
		m_tFrameQ[l].sOtherFrame="";
	}

	track_mark = 0;
	contact_mark = 0;
	noise_mark = 0;
	status_mark = 0;

	Track_prio = 4;
	Contact_prio = 2;
	Noise_prio = 1.4;
	Status_prio = 1;

	memory_time = 60;
	prev_msg_time = 0;

	ack_val = false;
}

CRouter::~CRouter()
{
 
}

bool CRouter::Iterate()
{

        double dfNow;

	    // nothing for now - eventually check for ad-hoc tx criteria and command / send something
        double curr_time = MOOSTime();

	    if ((curr_time - track_mark) <= 100.0 *memory_time)
	      wgt[3] = Track_prio*exp((curr_time - track_mark)/memory_time);
	    else
	      wgt[3] = exp(100.0);
	    
	    if ((curr_time - contact_mark) <= 100.0 *memory_time)
	      wgt[2] = Contact_prio*exp((curr_time - contact_mark)/memory_time);
	    else
	      wgt[2] = exp(100.0);
	    
	    if ((curr_time - noise_mark) <= 100.0 *memory_time)
	      wgt[1] = Noise_prio*exp((curr_time - noise_mark)/memory_time);
	    else
	      wgt[1] = exp(100.0);
	    
	    // if (ack_val)           // not necessary now - mg
	    //  {
	    //	wgt[0] = exp(200.0);
	    //  }
	    if ((curr_time - status_mark) <= 100.0 *memory_time)
	      wgt[0] = Status_prio*exp((curr_time - status_mark)/memory_time);
	    else
	      wgt[0] = exp(100.0);
	    
	    m_Comms.Notify("WGT_TRACK_MSG",wgt[3]);
	    m_Comms.Notify("WGT_CONTACT_MSG",wgt[2]);
	    m_Comms.Notify("WGT_NOISE_MSG",wgt[1]);
	    m_Comms.Notify("WGT_STATUS_MSG",wgt[0]);
	    
	return true;
}


bool CRouter::OnStartUp()
{
		string sInterval; 
		string sAddress;
        CMOOSApp::OnStartUp();
		
		MOOSTrace("=========================================================\n");
	    MOOSTrace("\n");
		MOOSTrace("                  %s, %d.%02d\n",VERSION_NAME,VERSION_MAJOR,VERSION_MINOR);
		MOOSTrace("\n");
		MOOSTrace("      MOOS uRouter - Simulates PLUSNet message strings \n");
		MOOSTrace("\n");
		MOOSTrace("\n");
		MOOSTrace("$Id: Router.cpp,v 1.5 2007-10-07 06:14:46 arjunab Exp $\n");
		MOOSTrace("=========================================================\n\n\n");
	
		// Input from Framer  
		//
        if (m_MissionReader.GetConfigurationParam("InputFrames",m_sFramerInputName)){
                MOOSTrace("pRouter: InputFrames = \"%s\"\n",m_sFramerInputName.c_str());
        } else {
                m_sFramerInputName="FRAMER_FRAME_OUTPUT";
                MOOSTrace("pRouter: Warning:\"InputFrames\" not set (using \"%s\")\n",m_sFramerInputName.c_str());
        }
        MOOSTrace("pRouter: Adding Frame Input: \"%s\"\n",m_sFramerInputName.c_str());
        AddMOOSVariable("InputFrames",m_sFramerInputName,"",0);


        // Output to Framer
		//
        if (m_MissionReader.GetConfigurationParam("OutputFrames",m_sFramerOutputName)){
                MOOSTrace("pRouter: OutputFrames = \"%s\"\n",m_sFramerOutputName.c_str());
        } else {
                m_sFramerOutputName="FRAMER_FRAME_INPUT";
                MOOSTrace("pRouter: Warning:\"OutputFrames\" not set (using \"%s\")\n",m_sFramerOutputName.c_str());
        }
        MOOSTrace("pRouter: Adding Frame Output: \"%s\"\n",m_sFramerOutputName.c_str());


        // Input from Modem
		//
        if (m_MissionReader.GetConfigurationParam("ModemData",m_sModemDataName)){
                MOOSTrace("pRouter: ModemData = \"%s\"\n",m_sModemDataName.c_str());
        } else {
                m_sModemDataName="MICROMODEM_DATA";
                MOOSTrace("Warning:\"ModemData\" not set (using \"%s\")\n",m_sModemDataName.c_str());
        }
        MOOSTrace("pRouter: Adding Modem Data: \"%s\"\n",m_sModemDataName.c_str());
        AddMOOSVariable("ModemData",m_sModemDataName,"",0);       // our frame input


        // Output to Modem  
	    //
        if (m_MissionReader.GetConfigurationParam("ModemCommand",m_sModemCommandName)){
                MOOSTrace("pRouter: ModemCommand = \"%s\"\n",m_sModemCommandName.c_str());
        } else {
                m_sModemCommandName="MICROMODEM_COMMAND";
                MOOSTrace("pRouter: Warning:\"ModemCommand\" not set (using \"%s\")\n",m_sModemCommandName.c_str());
        }
        MOOSTrace("pRouter: Adding Modem Command: \"%s\"\n",m_sModemCommandName.c_str());

        RegisterMOOSVariables();
	    MOOSTrace("\n\n");

        string sVal;
        if(m_MissionReader.GetValue("priority_time",sVal))
        {
            memory_time = atof(sVal.c_str());
        }
        return true;
}

bool CRouter::OnNewMail(MOOSMSG_LIST &NewMail)
{
	    MOOSMSG_LIST::iterator p;
		STRING_LIST::iterator s;
	    for(p = NewMail.begin();p!=NewMail.end();p++)
	    {
			CMOOSMsg & rMsg = *p;
			// skewed is a warning
			if (rMsg.IsSkewed(MOOSTime())){
				MOOSTrace("Warning: Mail on \"%s\" IS SKEWED (dropping mail) \n",
				rMsg.m_sKey.c_str());
				return false;
			}

			// now sort mail
			if (rMsg.GetKey()==m_sFramerInputName)
			{
				// enqueue frames
				MOOSTrace("pRouter: got a data frame.\n");
				OnFramerFrame(rMsg.GetString());
			}
			else if (rMsg.GetKey()==m_sModemDataName)
			{
				// parse modem messages
				MOOSTrace("pRouter: got modem data.\n");
				OnModemData(rMsg.GetString());

			} else {
				MOOSTrace("Unexpected Mail !!!\n");
			}
		}
		return UpdateMOOSVariables(NewMail);
}


bool CRouter::OnConnectToServer() {

	m_Comms.Register("ACK_VAL",0);   
	return true;
}


///////////////////////////////////////////////////////////////////////////
// here we initialise 
//
bool CRouter::Initialise()
{
        return true;
}

///////////////////////////////////////////////////////////////////////////
//  
// SendFrameToModem() - reply to data request with data
//
bool CRouter::SendFrameToModem()
{
	std::string sBestFrame;
	int iDest;
	int iAck;
	std::string sRequest = m_sLastModemData;
	std::string sMessageType;
	

#ifdef DONTSORT	
	MOOSTrace("SendFrame() => Not Sorting data by destination.\n");
	iDest = 0;
#else
	if (!MOOSValFromString(iDest,sRequest,"Dest")) {
		MOOSTrace("pRouter: error: no \"Dest\" in Modem Data Request Message!\n");
		return false;
	}
#endif

	if (!MOOSValFromString(iAck,sRequest,"Ack")) {
		MOOSTrace("pRouter: warning: no \"Ack\" in Modem Data Request Message => using Ack=0\n");
		iAck=0;
	}


	// Don't send anything unless 10 seconds since last one
	// HS 052407
	double curr_time = MOOSTime();
//	if (curr_time > prev_msg_time + 0.0)
//	{
		sBestFrame = GetBestFrame(iDest);
//	    prev_msg_time = curr_time;
//	} else
//	    sBestFrame = "";
        MOOSTrace("The STRING is %s\n",sBestFrame.c_str());

	if (sBestFrame.length()){
		if (!MOOSValFromString(sMessageType,sBestFrame,"MessageType")) {
			sMessageType = "UnknownData";
		}

		MOOSTrace("pRouter: Writing %s Frame for %d to %s.\n",
			  sMessageType.c_str(),iDest,m_sModemCommandName.c_str());
		      m_Comms.Notify(m_sModemCommandName,sBestFrame,MOOSTime());
	    } else {
		      MOOSTrace("pRouter: No Frame for %d.\n",iDest);

		      m_Comms.Notify(m_sModemCommandName,sBestFrame,MOOSTime());
	    }
        return true;
}

///////////////////////////////////////////////////////////////////////////
//  
// SendFrameToFramer() - forward data to framer
//
bool CRouter::SendFrameToFramer()
{

	m_Comms.Notify(m_sFramerOutputName,m_sLastModemData,MOOSTime());

	return true;
}

///////////////////////////////////////////////////////////////////////////
//  
// OnModemData() - parse modem data message
//
bool CRouter::OnModemData(std::string sDataIn)
{
	std::string sData = sDataIn ;
	std::string sDataType ;
	std::string sHexData ;
	int iSrc;
	int iDest;
	int iAck;

	time_t tTime;
	struct tm *tmNow;

	time(&tTime);
	tmNow=localtime(&tTime);

	if (!MOOSValFromString(sDataType,sData,"Data")) {
		MOOSTrace("pRouter: error: no \"Data=\" in Modem Data Message!\n");
		return false;
	}


	// DATA Data: a Data frame message
	if (sDataType=="DATA") {

		// Src
		if (!MOOSValFromString(iSrc,sData,"Src")){	
			MOOSTrace("pRouter: ERROR: OnModemData(): no \"Src\"\n");
			return false;
		}
		
		// Dest
		if (!MOOSValFromString(iDest,sData,"Dest")){
			MOOSTrace("pRouter: ERROR: OnModemData(): no \"Dest\"\n");
			return false;
		}
		
		// Ack
		if (!MOOSValFromString(iAck,sData,"Ack")){
			MOOSTrace("pRouter: ERROR: OnModemData(): no \"Ack\"\n");
			return false;
		}
		
		// Data
		if (!MOOSValFromString(sHexData,sData,"HexData")){
			MOOSTrace("pRouter: ERROR: OnModemData(): no \"HexData\"\n");
			return false;
		}
		MOOSTrace("pRouter(): Got Modem Data.   [Src=%d,Dest=%d,Ack=%d]\n",
			  iSrc,iDest,iAck);
		m_sLastModemData = sData ;
		SendFrameToFramer();
	} else if (sDataType=="DATAREQUEST"){
	  
	  // Ack
	  if (!MOOSValFromString(iAck,sData,"Ack")){
	    MOOSTrace("pRouter: WARNING: no \"Ack\" in DATREQUEST (using Ack=0)\n");
	    iAck=0;
	  }
	  
	  MOOSTrace("[%02d:%02d:%02d] pRouter(): Got Modem Data Request. (Ack=%d)\n",
		    tmNow->tm_hour,tmNow->tm_min,tmNow->tm_sec,iAck);
	  if (iAck)
	    ack_val=true;
	  else
	    ack_val=false;
	  m_sLastModemData = sData ;  // store DATAREQUEST message

	  SendFrameToModem();
	}
	
        return true;
}

///////////////////////////////////////////////////////////////////////////
//  
//  OnFramerFrame() - enqueue data frame
//
bool CRouter::OnFramerFrame(std::string sFrameIn)
{
	std::string sFrame = sFrameIn;
	std::string sMessageType;
	int iSrc ;
	int iDest ;
	int iType ;
	double dTimestamp=0.0;

	// Data
	if (!MOOSValFromString(sMessageType,sFrame,"MessageType")){
		MOOSTrace("pRouter: ERROR: OnFramerFrame(): no \"MessageType\" in Frame Message\n");
		return false;
	}
#ifdef DONTSORT
	MOOSTrace("On FramerFrame() => Not sorting data by destination\n");
	iDest = 0;
#else
	if (!MOOSValFromString(iDest,sFrame,"Dest")){
		MOOSTrace("pRouter: ERROR: OnFramerFrame(): no \"Dest\" in Frame Message\n");
		return false;
	}
#endif

	if (!MOOSValFromString(dTimestamp,sFrame,"Timestamp")){
		MOOSTrace("pRouter: WARNING: OnFramerFrame(): no \"Timestamp\" in Frame Message\n");
	}

	MOOSTrace("pRouter: Got a Message for Node %d MessageType is %s.[Timestamp=%012.1f]\n",
		  iDest,sMessageType.c_str(),dTimestamp);

	if (sMessageType=="HUXLEY"){
		m_tFrameQ[iDest].sHuxleyFrame = sFrame ;
		
	} else if (sMessageType=="SENSOR_RECORDER"){
		m_tFrameQ[iDest].sRecorderFrame = sFrame ;
				 
	} else if (sMessageType=="COMMAND"){
		m_tFrameQ[iDest].sCommandFrame = sFrame ;
				 
	}else if (sMessageType=="XRAY_STATUS"){
		m_tFrameQ[iDest].sXRayStatusFrame = sFrame ;
				 
	}else if (sMessageType=="XRAY_BEHAVIOR"){
		m_tFrameQ[iDest].sXRayBehaviorFrame = sFrame ;		
		 
	}else if (sMessageType=="SENSOR_STATUS"){
		m_tFrameQ[iDest].sStatusFrame = sFrame ;
				 
	}else if (sMessageType=="SENSOR_CONTACT"){
		m_tFrameQ[iDest].sContactFrame = sFrame ;	
			 
	}else if (sMessageType=="SENSOR_TRACK"){
		m_tFrameQ[iDest].sTrackFrame = sFrame ;		
		 
	}else if (sMessageType=="SENSOR_NOISE"){
		m_tFrameQ[iDest].sNoiseFrame = sFrame ;		
		 
	}else if (sMessageType=="SENSOR_PROSECUTE"){
		m_tFrameQ[iDest].sProsecuteFrame = sFrame ;	
			 
	}else if (sMessageType=="SENSOR_DEPLOY"){
		m_tFrameQ[iDest].sDeployFrame = sFrame ;	
			 
	}else{
		m_tFrameQ[iDest].sOtherFrame = sFrame ;
	}
        return true;
}

// priority function
//
std::string CRouter::GetBestFrame(int iDest){

	std::string sBest;
	std::string *sCandidate;
	std::string *nextCandidate;
	
	double curr_time = MOOSTime();
#ifdef DONTSORT
	MOOSTrace("GetBestFrame() => Not sorting data by destination\n");
	iDest = 0;
#endif

	if (ack_val){
MOOSTrace("The destination = %d\n",iDest);

		if(m_tFrameQ[iDest].sStatusFrame.length()) {
			MOOSTrace("DATAREQUEST: Dest=%d, Ack=1 => sending SENSOR_STATUS\n",iDest);
			sBest = m_tFrameQ[iDest].sStatusFrame ;
			// resets WGT_STATUS, when ACK=1, necessary?
			wgt[0]=exp(100.0);
			status_mark = curr_time ;
			return sBest;
		} else {
			MOOSTrace("DATAREQUEST: Dest=%d, Ack=1 => no SENSOR_STATUS to send !!!\n",iDest);			
		}
	} else {

	  MOOSTrace("DATAREQUEST: Ack=0, Sending best message available.\n");

	}
	    
	// Check which messages exist
	//    
	nextCandidate = &(m_tFrameQ[iDest].sTrackFrame); 
	exist[3] = (nextCandidate->length());
	nextCandidate = &(m_tFrameQ[iDest].sContactFrame); 
	exist[2] = (nextCandidate->length());
	nextCandidate = &(m_tFrameQ[iDest].sNoiseFrame); 
	exist[1] = (nextCandidate->length());
	nextCandidate = &(m_tFrameQ[iDest].sStatusFrame); 
	exist[0] = (nextCandidate->length());
	    
	double prio = 0;
	int highest = 0;
	    
	highest = DetHighestPrio(wgt, exist, 4);
	if (highest > 0) {
		prio = wgt[highest-1];
		MOOSTrace("Msg %d exists and has highest prio, wgt = %f \n",highest,prio);
	} else
		highest = 1;

	sCandidate = &(m_tFrameQ[iDest].sHuxleyFrame);  // highest priority
	if (sCandidate->length()) {
		sBest = *sCandidate;
		*sCandidate = "";
		return sBest;
	}
	sCandidate = &(m_tFrameQ[iDest].sCommandFrame); 
	if (sCandidate->length()) {
		sBest = *sCandidate;
		*sCandidate = "";
		return sBest;
	}
	sCandidate = &(m_tFrameQ[iDest].sRecorderFrame); 
	if (sCandidate->length()) {
		sBest = *sCandidate;
		*sCandidate = "";
		return sBest;
	}
	sCandidate = &(m_tFrameQ[iDest].sProsecuteFrame); 
	if (sCandidate->length()) {
		sBest = *sCandidate;
		*sCandidate = "";
		return sBest;
	}
	sCandidate = &(m_tFrameQ[iDest].sDeployFrame); 
	if (sCandidate->length()) {
		sBest = *sCandidate;
		*sCandidate = "";
		return sBest;
	}
	sCandidate = &(m_tFrameQ[iDest].sXRayBehaviorFrame); 
	if (sCandidate->length()) {
		sBest = *sCandidate;
		*sCandidate = "";
		return sBest;
	}
	    
	sCandidate = &(m_tFrameQ[iDest].sTrackFrame); 
	if (sCandidate->length() && highest == 4) 
		{
		sBest = *sCandidate;
		*sCandidate = "";
		track_mark = curr_time;
		return sBest;
	      }
	    
	    sCandidate = &(m_tFrameQ[iDest].sContactFrame); 
	    if (sCandidate->length() && highest == 3) 
	      {
		sBest = *sCandidate;
		*sCandidate = "";
		contact_mark = curr_time;
		return sBest;
	      }
	    
	    sCandidate = &(m_tFrameQ[iDest].sNoiseFrame); 
	    if (sCandidate->length() && highest == 2) {
	      sBest = *sCandidate;
	      *sCandidate = "";
	      noise_mark = curr_time;
	      return sBest;
	    }
	    
	    sCandidate = &(m_tFrameQ[iDest].sStatusFrame); 
	    if (sCandidate->length() && highest == 1) {
	      sBest = *sCandidate;
	      *sCandidate = "";
	      status_mark = curr_time;
	      // ack_val = false;  // not necessary, mg
	      return sBest;
	    }
	    
	    
	    sCandidate = &(m_tFrameQ[iDest].sXRayStatusFrame); 
	    if (sCandidate->length()) {
	      sBest = *sCandidate;
	      *sCandidate = "";
	      return sBest;
	    }
	    
	    sCandidate = &(m_tFrameQ[iDest].sOtherFrame); 
	    if (sCandidate->length()) {
	      sBest = *sCandidate;
	      *sCandidate = "";
	      return sBest;
	    }
	    
	    sBest = "";
	
	return sBest;
}

// find highest priority message
//
int CRouter::DetHighestPrio(double* wght , bool* exst, int n)
{
  int high = 0;
  double prio = 0;
  int i;
  for (i = 0 ; i < n ; i++)
    {
      if (wght[i] > prio && exst[i])
	{
	  high = i+1;
	  prio = wght[i];
	}
    }
  return(high);
}
