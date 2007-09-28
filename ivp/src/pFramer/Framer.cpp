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
// Framer.cpp: implementation of the CFramer class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include "Framer.h"
#include "CCLMessageTypes.h"
#include "WHOI_FieldInterfaceViaMOOSUnpackFunctor.h"
#include "WHOI_FieldInterfaceViaMOOSPackFunctor.h"


// 3rd party encode/decode
#include "XRayPackets.h"

#include <time.h>
#include <iostream>

using namespace std;

#define DEFAULT_CSV_LOG_INTERVAL (3600) // seconds => 1 hour

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFramer::CFramer()
{
	// not in .moos file yet
	m_bDoAckHuxley = true;
	m_bDoAckCommands = true;
	m_bDoAckStatus = false;
}

CFramer::~CFramer()
{

}

//
// Iterate() - just check timeouts, for now
//
bool CFramer::Iterate()
{

        double dfNow;
        long liElapsed;

        dfNow=MOOSTime();

        liElapsed =(long)(dfNow - m_dfLastMailTime);
        
        switch (liElapsed) {
        case 10:
        case 30:
        case 60:
                if (!m_bRecentlyWarned) {
                        MOOSTrace("pFramer: WARNING: No Mail Received for %d seconds\n",liElapsed);
                        m_bRecentlyWarned = true;
                }
                break;

        case 120:
        case 180:
        case 240:
        case 300:
        case 600:
                if (!m_bRecentlyWarned) {
                        MOOSTrace("pFramer: WARNING: No Mail Received for %d minutes\n",liElapsed/60);
                        m_bRecentlyWarned = true;
                }
                break;
        default:
                m_bRecentlyWarned = false ;
        }
	return true;
}

//
// OnStartup() - register pFramer variable names, as read from .moos file
//
bool CFramer::OnStartUp()
{

        CMOOSApp::OnStartUp();

	MOOSTrace("=========================================================\n");
	MOOSTrace("\n");
	MOOSTrace("       MOOS pFramer - Encodes messages to binary data\n");
	MOOSTrace("\n");
	MOOSTrace("                   %s Version %d.%02d\n",VERSION_NAME,VERSION_MAJOR,VERSION_MINOR);
	MOOSTrace("\n");
	MOOSTrace("\n");
	MOOSTrace("$Id: Framer.cpp,v 1.3 2007-09-06 15:39:57 mgrund Exp $\n");
	MOOSTrace("=========================================================\n\n\n");
	

        // Input Messages  
	//
        if (m_MissionReader.GetConfigurationParam("InputMessages",m_sInputMsgName)){
                MOOSTrace("InputMessages = \"%s\"\n",m_sInputMsgName.c_str());
        } else {
                m_sInputMsgName="FRAMER_MESSAGE_INPUT";
                MOOSTrace("Warning:\"InputMessages\" not set (using \"%s\")\n",m_sInputMsgName.c_str());
        }
        MOOSTrace("pFramer: Adding Message Input: \"%s\"\n",m_sInputMsgName.c_str());
        AddMOOSVariable("MessageInput",m_sInputMsgName,"",0);       // our message input


        // Output Messages  
	//
        if (m_MissionReader.GetConfigurationParam("OutputMessages",m_sOutputMsgName)){
                MOOSTrace("InputMessages = \"%s\"\n",m_sOutputMsgName.c_str());
        } else {
                m_sOutputMsgName="FRAMER_MESSAGE_OUTPUT";
                MOOSTrace("Warning:\"OutputMessages\" not set (using \"%s\")\n",m_sOutputMsgName.c_str());
        }
        MOOSTrace("pFramer: Adding Message Output: \"%s\"\n",m_sOutputMsgName.c_str());
        // AddMOOSVariable("MessageOutput",m_sOutputMsgName,"",0);       // our message output


        // Input Frames  
	//
        if (m_MissionReader.GetConfigurationParam("InputFrames",m_sInputFrameName)){
                MOOSTrace("InputFrames = \"%s\"\n",m_sInputFrameName.c_str());
        } else {
                m_sInputFrameName="FRAMER_FRAME_INPUT";
                MOOSTrace("Warning:\"InputFrames\" not set (using \"%s\")\n",m_sInputFrameName.c_str());
        }
        MOOSTrace("pFramer: Adding Frame Input: \"%s\"\n",m_sInputFrameName.c_str());
        AddMOOSVariable("FrameInput",m_sInputFrameName,"",0);       // our frame input


        // Output Frames  
	//
        if (m_MissionReader.GetConfigurationParam("OutputFrames",m_sOutputFrameName)){
                MOOSTrace("InputFrames = \"%s\"\n",m_sOutputFrameName.c_str());
        } else {
                m_sInputFrameName="FRAMER_FRAME_OUTPUT";
                MOOSTrace("Warning:\"OutputFrames\" not set (using \"%s\")\n",m_sOutputFrameName.c_str());
        }
        MOOSTrace("pFramer: Adding Frame Output: \"%s\"\n",m_sOutputFrameName.c_str());
        // AddMOOSVariable("FrameOutput",m_sOutputFrameName,"",0);       // our frame output
        RegisterMOOSVariables();
	MOOSTrace("\n\n");
	
        m_dfLastMailTime = MOOSTime();

        return true;
}

//
//  OnNewMail() - parse the input, send output (this app is data driven)
//
bool CFramer::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	STRING_LIST::iterator s;
	for(p = NewMail.begin();p!=NewMail.end();p++)
	{
		CMOOSMsg & rMsg = *p;
  
                m_dfLastMailTime = MOOSTime();

		// skewed is a warning
		if (rMsg.IsSkewed(MOOSTime())){
                        MOOSTrace("Warning: Mail on \"%s\" IS SKEWED (not dropping mail...everything old is new again) \n",
				  rMsg.m_sKey.c_str());
                }

		// now sort mail
		if (rMsg.GetKey()==m_sInputMsgName){
			// translate messages to frames (bounce off member struct)
			if (OnMessage(rMsg.GetString()))
				SendFrame();
		}else if (rMsg.GetKey()==m_sInputFrameName){
			// translate frames to messages (bounce off member struct)
			if (OnFrame(rMsg.GetString()))
				SendMessage();
		} else {
			MOOSTrace("Unexpected Mail !!!\n");
		}
	}
	return UpdateMOOSVariables(NewMail);
}


bool CFramer::OnConnectToServer()
{
        // Add variables and register them

	RegisterMOOSVariables();
        MOOSTrace("pFramer: Registered MOOS Variables.\n");
        return true;
}


////////////////////////////////////////////////
// 
//  MOOSApp Initialization
//
bool CFramer::Initialise()
{    
        return true;
}

////////////////////////////////////////////////
//
//  Parse user messages 
//
bool CFramer::OnMessage(const std::string & sMessageIn)
{    
	std::string sMessageType;
	std::string sMessage = sMessageIn;
	double dTimestamp;

        MOOSRemoveChars(sMessage," \t");
	
	if(!MOOSValFromString(sMessageType,sMessage,"MessageType")){
		MOOSTrace("pFramer: ERROR: No MessageType in: \"%s\" \n",sMessage.c_str());
		return false;
	}

	if(!MOOSValFromString(dTimestamp,sMessage,"Timestamp")){
		MOOSTrace("pFramer: WARNING: No Timestamp in: \"%s\" \n",sMessage.c_str());
		m_dLastMessageTimestamp = 0.0 ; 
	} else {
	  m_dLastMessageTimestamp = dTimestamp;
	}

	m_sLastMessageType = sMessageType ;

	MOOSTrace("pFramer: Got Message Type: %s \n",sMessageType.c_str());
	
	if (sMessageType=="HUXLEY") 
		return OnMessageHuxley(sMessage);
	
	if (sMessageType=="SENSOR_RECORDER") 
		return OnMessageRecorder(sMessage);
	
	if (sMessageType=="COMMAND")
		return OnMessageCommand(sMessage);

	if (sMessageType=="XRAY_BEHAVIOR")
		return OnMessageXRayBehavior(sMessage);

	if (sMessageType=="XRAY_STATUS")
		return OnMessageXRayStatus(sMessage);

	if (sMessageType=="SENSOR_STATUS")
		return OnMessagePlusnetReport(sMessage);

	if (sMessageType=="SENSOR_CONTACT")
		return OnMessagePlusnetReport(sMessage);

	if (sMessageType=="SENSOR_TRACK")
		return OnMessagePlusnetReport(sMessage);

	if (sMessageType=="SENSOR_NOISE")
		return OnMessagePlusnetReport(sMessage);

	if (sMessageType=="SENSOR_PROSECUTE")
		return OnMessagePlusnetCommand(sMessage);

	if (sMessageType=="SENSOR_DEPLOY")
		return OnMessagePlusnetCommand(sMessage);
	
        return false;
}

// HUXLEY message
bool CFramer::OnMessageHuxley(const std::string & sMessageIn)
{    
	std::string sVal = sMessageIn;
	std::string sHexData;
	
	// Src
	if (!MOOSValFromString(m_sTheFrame.iSrc,sVal,"SourcePlatformId")){	
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"SourcePlatformId\"\n");
		return false;
	}
	
	// Dest
	if (!MOOSValFromString(m_sTheFrame.iDest,sVal,"DestinationPlatformId")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Dest\"\n");
		return false;
	}
	
	// Data
	if (!MOOSValFromString(sHexData,sVal,"HexData")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"HexData\"\n");
		return false;
	}
	
	m_sTheFrame.iSize=HexToData((char *)sHexData.c_str(),m_sTheFrame.caData);	
	m_sTheFrame.iType = MDAT_EDGESERVE;

	// Ack
	m_sTheFrame.bAck = (m_bDoAckHuxley)?(1):(0);

	// Size
	m_sTheFrame.iSize = sHexData.length()/2;

        return true;
}

// SENSOR_RECORDER message
bool CFramer::OnMessageRecorder(const std::string & sMessageIn)
{    
	std::string sVal = sMessageIn;
	int nRecorderOn;
	
	// Src
	if (!MOOSValFromString(m_sTheFrame.iSrc,sVal,"SourcePlatformId")){	
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"SourcePlatformId\"\n");
		return false;
	}
	
	// Dest
	if (!MOOSValFromString(m_sTheFrame.iDest,sVal,"DestinationPlatformId")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Dest\"\n");
		return false;
	}

	// RecorderOn
	if (!MOOSValFromString(nRecorderOn,sVal,"RecorderOn")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"RecorderOn\"\n");
		return false;
	}


	// form the command frame
	m_sTheFrame.iType = MDAT_COMMAND;      // redundant
	m_sTheFrame.caData[0] = MDAT_COMMAND;  // redundant
	m_sTheFrame.caData[1] = 0 ;            // spare

	if (nRecorderOn)
		m_sTheFrame.caData[2] = VC_RECORDER_ON ;  // LSB of command
	else
		m_sTheFrame.caData[2] = VC_RECORDER_OFF;  // LSB of command

	m_sTheFrame.caData[3] = 0;                        // MSB of command

	// Ack
	m_sTheFrame.bAck = (m_bDoAckCommands)?(1):(0);

	// Size
	m_sTheFrame.iSize = 4 ;  // not always - other commands can have args

        return true;
}

//
//  Generic Command Message: by number for now
//
bool CFramer::OnMessageCommand(const std::string & sMessageIn)
{    
	std::string sVal = sMessageIn;
	int nCommand;
	
	// Src
	if (!MOOSValFromString(m_sTheFrame.iSrc,sVal,"SourcePlatformId")){	
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"SourcePlatformId\"\n");
		return false;
	}
	
	// Dest
	if (!MOOSValFromString(m_sTheFrame.iDest,sVal,"DestinationPlatformId")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Dest\"\n");
		return false;
	}

	// CommandNumber
	if (!MOOSValFromString(nCommand,sVal,"CommandNumber")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"CommandNumber\"\n");
		return false;
	}


	// form the command frame
	m_sTheFrame.iType = MDAT_COMMAND;         // redundant
	m_sTheFrame.caData[0] = MDAT_COMMAND;     // redundant
	m_sTheFrame.caData[1] = 0 ;               // spare
	m_sTheFrame.caData[2] = nCommand ;        // LSB of command
	m_sTheFrame.caData[3] = 0;                // MSB of command

	// Ack
	m_sTheFrame.bAck = (m_bDoAckCommands)?(1):(0);

	// Size
	m_sTheFrame.iSize = 4 ;  // not always - other commands can have args

        return true;
}

//
//  Parse XRay Behavior Message
//
bool CFramer::OnMessageXRayBehavior(const std::string & sMessageIn)
{    
	std::string sVal = sMessageIn;
	xray_behavior sXRay;
	mdat_xray_behavior *psXRayPacked;
	int nTmp;
	double dTmp;
	std::string sPassword;
	char caPassword[16];	

	// Src
	if (!MOOSValFromString(m_sTheFrame.iSrc,sVal,"SourcePlatformId")){	
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"SourcePlatformId\"\n");
		return false;
	}
	
	// Dest
	if (!MOOSValFromString(m_sTheFrame.iDest,sVal,"DestinationPlatformId")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Dest\"\n");
		return false;
	}


	// Ack
	m_sTheFrame.bAck = (m_bDoAckCommands)?(1):(0);

	// Type
	m_sTheFrame.iType = MDAT_XRAY_BEHAVIOR;

	// Size
	m_sTheFrame.iSize = 32 ;

	// Fill out the struct
	// MsgSim(): MessageType=XRAY_BEHAVIOR,SourcePlatformId=1,DestinationPlatformId=0,BehaviorType=0,
	// Depth1=32.4,Depth2=109.0,Depth3=221.5,Latitude1=48.295904,Longitude1=-122.601943,Latitude2= -10.101930,
	// Longitude2=65.797127,Heading=0.296,HeadingRate=-2.5,Speed=1.5,TimeStamp=8488464,Flag1=0,Flag2=1,Flag3=0,
	// Flag4=1,Flag5=0,Flag6=1,Flag7=0,Flag8=1,Password=XR

	// BehaviorType
	if (!MOOSValFromString(nTmp,sVal,"BehaviorType")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"BehaviorType\"\n");
		return false;
	}
	sXRay.behaviorType=(XRAY_BEHAVIOR_TYPE)nTmp;

	// TimeStamp
	if (!MOOSValFromString(nTmp,sVal,"TimeStamp")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"TimeStamp\"\n");
		return false;
	}
	sXRay.time=(long)nTmp;

	// Depth1
	if (!MOOSValFromString(sXRay.depth1,sVal,"Depth1")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Depth1\"\n");
		return false;
	}

	// Depth2
	if (!MOOSValFromString(sXRay.depth2,sVal,"Depth2")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Depth2\"\n");
		return false;
	}
	
	// Depth3
	if (!MOOSValFromString(sXRay.depth3,sVal,"Depth3")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Depth3\"\n");
		return false;
	}

	// Latitude1
	if (!MOOSValFromString(sXRay.latitude1,sVal,"Latitude1")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Latitude1\"\n");
		return false;
	}

	// Longitude1
	if (!MOOSValFromString(sXRay.longitude1,sVal,"Longitude1")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Longitude1\"\n");
		return false;
	}

	// Latitude2
	if (!MOOSValFromString(sXRay.latitude2,sVal,"Latitude2")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Latitude2\"\n");
		return false;
	}

	// Longitude2
	if (!MOOSValFromString(sXRay.longitude2,sVal,"Longitude2")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Longitude2\"\n");
		return false;
	}

	// Heading
	if (!MOOSValFromString(sXRay.heading,sVal,"Heading")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Heading\"\n");
		return false;
	}

	// HeadingRate
	if (!MOOSValFromString(sXRay.headingRate,sVal,"HeadingRate")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"HeadingRate\"\n");
		return false;
	}

	// Speed
	if (!MOOSValFromString(sXRay.speed,sVal,"Speed")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Speed\"\n");
		return false;
	}

	// Flag1
	if (!MOOSValFromString(nTmp,sVal,"Flag1")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag1\"\n");
		return false;
	}
	sXRay.flag1=nTmp;

	// Flag2
	if (!MOOSValFromString(nTmp,sVal,"Flag2")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag2\"\n");
		return false;
	}
	sXRay.flag2=nTmp;
	// Flag3
	if (!MOOSValFromString(nTmp,sVal,"Flag3")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag3\"\n");
		return false;
	}
	sXRay.flag3=nTmp;
	// Flag4
	if (!MOOSValFromString(nTmp,sVal,"Flag4")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag4\"\n");
		return false;
	}
	sXRay.flag4=nTmp;
	// Flag5
	if (!MOOSValFromString(nTmp,sVal,"Flag5")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag5\"\n");
		return false;
	}
	sXRay.flag5=nTmp;
	// Flag6
	if (!MOOSValFromString(nTmp,sVal,"Flag6")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag6\"\n");
		return false;
	}
	sXRay.flag6=nTmp;
	// Flag7
	if (!MOOSValFromString(nTmp,sVal,"Flag7")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag7\"\n");
		return false;
	}
	sXRay.flag7=nTmp;
	// Flag8
	if (!MOOSValFromString(nTmp,sVal,"Flag8")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag8\"\n");
		return false;
	}
	sXRay.flag8=nTmp;

	// Password
	if (!MOOSValFromString(sPassword,sVal,"Password")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Password\"\n");
		return false;
	}
	strcpy(caPassword,sPassword.c_str());
        sXRay.password[0]=caPassword[0];
        sXRay.password[1]=caPassword[1];

	// pack it
	//
	psXRayPacked =(mdat_xray_behavior *)m_sTheFrame.caData ;
	encodeXRayBehavior(&sXRay,psXRayPacked);  // in XRayPackets.cpp

        return true;
}

//
//  Parse XRay Status Message
//
bool CFramer::OnMessageXRayStatus(const std::string & sMessageIn)
{    
	std::string sVal = sMessageIn;
	int nCommand;
	xray_status sXRay;
	mdat_xray_status *psXRayPacked;
	int nTmp;
	double dTmp;
	

	// Src
	if (!MOOSValFromString(m_sTheFrame.iSrc,sVal,"SourcePlatformId")){	
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"SourcePlatformId\"\n");
		return false;
	}
	
	// Dest
	if (!MOOSValFromString(m_sTheFrame.iDest,sVal,"DestinationPlatformId")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Dest\"\n");
		return false;
	}

	// Ack
	m_sTheFrame.bAck = (m_bDoAckCommands)?(1):(0);

	// Type
	m_sTheFrame.iType = MDAT_XRAY_STATUS;

	// Size
	m_sTheFrame.iSize = 32 ;

	// Fill out the XRay struct
        // MsgSim(): MessageType=XRAY_STATUS,SourcePlatformId=1,DestinationPlatformId=0,TimeStamp=8474216,
        // Energy=72.8,Depth=174.9,DepthRate=0.46,Latitude=36.893750,Longitude=-122.253619,Heading=0.296,HeadingRate=-2.5,
        // Roll=7.7,Pitch=-1.3,BEVolume=11467.5,BehaviorResponse=1,Flag1=0,Flag2=1,Flag3=0,Flag4=1,Flag5=0,Flag6=1,Flag7=0,
        // Flag8=1,Contacts=0

	// TimeStamp
	if (!MOOSValFromString(nTmp,sVal,"TimeStamp")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"TimeStamp\"\n");
		return false;
	}
	sXRay.time=nTmp;
	
	// Energy
	if (!MOOSValFromString(sXRay.energy,sVal,"Energy")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Energy\"\n");
		return false;
	}
	// Depth
	if (!MOOSValFromString(sXRay.depth,sVal,"Depth")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Depth\"\n");
		return false;
	}
	// DepthRate
	if (!MOOSValFromString(sXRay.depthRate,sVal,"DepthRate")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"DepthRate\"\n");
		return false;
	}
	// Latitude
	if (!MOOSValFromString(sXRay.latitude,sVal,"Latitude")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Latitude\"\n");
		return false;
	}
	// Longitude
	if (!MOOSValFromString(sXRay.longitude,sVal,"Longitude")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Longitude\"\n");
		return false;
	}
	// Altitude
	if (!MOOSValFromString(sXRay.pitch,sVal,"Altitude")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Altitude\"\n");
		return false;
	}
	// Heading
	if (!MOOSValFromString(sXRay.heading,sVal,"Heading")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Heading\"\n");
		return false;
	}
	// HeadingRate
	if (!MOOSValFromString(sXRay.headingRate,sVal,"HeadingRate")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"HeadingRate\"\n");
		return false;
	}
	// Roll
	if (!MOOSValFromString(sXRay.roll,sVal,"Roll")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Roll\"\n");
		return false;
	}
 	// Pitch
	if (!MOOSValFromString(sXRay.pitch,sVal,"Pitch")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Pitch\"\n");
		return false;
	}

	// BEVolume
	if (!MOOSValFromString(sXRay.beVolume,sVal,"BEVolume")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"BEVolume\"\n");
		return false;
	}
	// BehaviorResponse
	if (!MOOSValFromString(nTmp,sVal,"BehaviorResponse")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"BehaviorResponse\"\n");
		return false;
	}
	sXRay.behaviorResponse=(XRAY_RESPONSE_TYPE)nTmp;
	
	// Fault
	if (!MOOSValFromString(nTmp,sVal,"Fault")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Fault\"\n");
		return false;
	}
	sXRay.flag1=nTmp;

	// Flag1
	if (!MOOSValFromString(nTmp,sVal,"Flag1")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag1\"\n");
		return false;
	}
	sXRay.flag1=nTmp;

	// Flag2
	if (!MOOSValFromString(nTmp,sVal,"Flag2")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag2\"\n");
		return false;
	}
	sXRay.flag2=nTmp;
	// Flag3
	if (!MOOSValFromString(nTmp,sVal,"Flag3")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag3\"\n");
		return false;
	}
	sXRay.flag3=nTmp;
	// Flag4
	if (!MOOSValFromString(nTmp,sVal,"Flag4")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag4\"\n");
		return false;
	}
	sXRay.flag4=nTmp;
	// Flag5
	if (!MOOSValFromString(nTmp,sVal,"Flag5")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag5\"\n");
		return false;
	}
	sXRay.flag5=nTmp;
	// Flag6
	if (!MOOSValFromString(nTmp,sVal,"Flag6")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag6\"\n");
		return false;
	}
	sXRay.flag6=nTmp;
	// Flag7
	if (!MOOSValFromString(nTmp,sVal,"Flag7")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag7\"\n");
		return false;
	}
	sXRay.flag7=nTmp;
	// Flag8
	if (!MOOSValFromString(nTmp,sVal,"Flag8")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Flag8\"\n");
		return false;
	}
	sXRay.flag8=nTmp;
	// Contacts
	if (!MOOSValFromString(nTmp,sVal,"Contacts")){
		MOOSTrace("pFramer: ERROR: OnMessage(): no \"Contacts\"\n");
		return false;
	}
	sXRay.contacts=nTmp;
	
        // pack it
	//
	psXRayPacked =(mdat_xray_status *)m_sTheFrame.caData ;
	encodeXRayStatus(&sXRay,psXRayPacked);  // in XRayPackets.cpp
        return true;
}

//
//  Parse PLUSNet Report Messages
//
bool CFramer::OnMessagePlusnetReport(const std::string & sMessageIn)
{
	FIELDMESSAGE fieldMsg;
	CCLSensorReport_t *SensorReport;
	
	// fill struct from string
	CFieldUnpackFunctor()((void *)&fieldMsg,(void *)&sMessageIn);

	// fill frame struct
        //
	SensorReport=&(fieldMsg.sensorReport);        // Data
	EncodeSensorReport(*SensorReport,(PackedCCLSensorReport_t *)m_sTheFrame.caData);

	m_sTheFrame.bAck = (m_bDoAckStatus)?(1):(0);  // Ack
	m_sTheFrame.iType = MDAT_SENSOR_RPT;	      // Type
	m_sTheFrame.iSize = 32 ;	                  // Size


	switch (fieldMsg.sensorReport.SensorReportType) {

	case StatusReport_e:
		m_sTheFrame.iDest = fieldMsg.sensorReport.SensorReport.StatusReport.DestinationPlatformId ; 
		m_sTheFrame.iSrc = fieldMsg.sensorReport.SensorReport.StatusReport.SourcePlatformId ;
		break;

	case ContactReport_e:
		m_sTheFrame.iDest = fieldMsg.sensorReport.SensorReport.ContactReport.DestinationPlatformId ; 
		m_sTheFrame.iSrc = fieldMsg.sensorReport.SensorReport.ContactReport.SourcePlatformId ;
		break;

	case TrackReport_e:
		m_sTheFrame.iDest = fieldMsg.sensorReport.SensorReport.TrackReport.DestinationPlatformId ; 
		m_sTheFrame.iSrc = fieldMsg.sensorReport.SensorReport.TrackReport.SourcePlatformId ;
		break;

	case NoiseReport_e:
		m_sTheFrame.iDest = fieldMsg.sensorReport.SensorReport.NoiseReport.DestinationPlatformId ; 
		m_sTheFrame.iSrc = fieldMsg.sensorReport.SensorReport.NoiseReport.SourcePlatformId ;
		break;

	default:
		MOOSTrace("pFramer: unknown sensor report type, can't parse addresses, using 0->15\n");
		m_sTheFrame.iDest = 15;
		m_sTheFrame.iSrc = 0; 

	}
	                                                  // Addresses hack

        return true;
}

//
//  Parse PLUSNet Command Messages
//
bool CFramer::OnMessagePlusnetCommand(const std::string & sMessageIn)
{
	FIELDMESSAGE fieldMsg;
	CCLSensorCommand_t *SensorCommand;
	
	// fill struct from string
	CFieldUnpackFunctor()((void *)&fieldMsg,(void *)&sMessageIn);

	// fill frame struct
        //
	SensorCommand=&(fieldMsg.sensorCommand);          // Data
	EncodeSensorCommand(*SensorCommand,(PackedCCLSensorCommand_t *)m_sTheFrame.caData);

	m_sTheFrame.bAck = (m_bDoAckCommands)?(1):(0);    // Ack
	m_sTheFrame.iType = MDAT_SENSOR_CMD;	          // Type
	m_sTheFrame.iSize = 32 ;	                  // Size

	                                                  // Addresses 

	switch (fieldMsg.sensorCommand.SensorCommandType) {
	case DeployCommand_e:
		m_sTheFrame.iDest = fieldMsg.sensorCommand.SensorCommand.DeployCommand.DestinationPlatformId ; 
		m_sTheFrame.iSrc = fieldMsg.sensorCommand.SensorCommand.DeployCommand.SourcePlatformId ;
		break;
	case ProsecuteCommand_e:
		m_sTheFrame.iDest = fieldMsg.sensorCommand.SensorCommand.ProsecuteCommand.DestinationPlatformId ; 
		m_sTheFrame.iSrc = fieldMsg.sensorCommand.SensorCommand.ProsecuteCommand.SourcePlatformId ;
		break;

	default:
		MOOSTrace("pFramer: unknown sensor command type, can't parse addresses, using 0->15\n");
		m_sTheFrame.iDest = 15;
		m_sTheFrame.iSrc = 0; 

	}
	
        return true;
}


////////////////////////////////////////////////
// 
//  Form a user message from the frame
//
bool CFramer::SendMessage()
{    

	switch(m_sTheFrame.iType) {
		
	case MDAT_EDGESERVE:
		MOOSTrace("pFramer: Sending Message Type: HUXLEY\n");
		return SendMessageHuxley();
		
	case MDAT_COMMAND:
		MOOSTrace("pFramer: Sending Message Type: MDAT_COMMAND\n");
		return SendMessageRecorder();

	case MDAT_XRAY_BEHAVIOR:
		MOOSTrace("pFramer: Sending Message Type: XRAY_BEHAVIOR\n");
		return SendMessageXRayBehavior();

	case MDAT_XRAY_STATUS:
		MOOSTrace("pFramer: Sending Message Type: XRAY_STATUS\n");
		return SendMessageXRayStatus();

	case MDAT_SENSOR_RPT:
		MOOSTrace("pFramer: Sending Message Type: MDAT_SENSOR_RPT\n");
		return SendMessagePlusnetReport();
		
	case MDAT_SENSOR_CMD:
		MOOSTrace("pFramer: Sending Message Type: MDAT_SENSOR_CMD\n");
		return SendMessagePlusnetCommand();

	default:
		MOOSTrace("pFramer: UNKNOWN FRAME TYPE - sent as SENSOR_REPORT\n");
		  // MINUS'07 hack by HS. 053007. Unpredictable behavior 
		  // suggests that it's a timing issue putting the message 
		  // into this category.
		  // return SendMessagePlusnetReport();
		  	return SendMessageUnknown();
	}
	return false;
}

// Message for Huxley Type
bool CFramer::SendMessageHuxley()
{    
	std::string sDataMsg;
	char caHexData[640];

	if (m_sTheFrame.iSize) {           // if there is data, post it
		DataToHex(m_sTheFrame.caData,m_sTheFrame.iSize,caHexData);
		sDataMsg = MOOSFormat("MessageType=HUXLEY,SourcePlatformId=%d,DestinationPlatformId=%d,HexData=%s",
				      m_sTheFrame.iSrc,m_sTheFrame.iDest,caHexData);
		m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());
		return true;
	}
	return false;
}

// Message for Recorder Type
bool CFramer::SendMessageRecorder()
{    
	std::string sDataMsg;

	if (m_sTheFrame.iSize) {           // if there is data, post it

		if (m_sTheFrame.caData[2]==VC_RECORDER_ON) {	
			sDataMsg = MOOSFormat("MessageType=SENSOR_RECORDER,SourcePlatformId=%d,DestinationPlatformId=%d,RecorderOn=1",
					      m_sTheFrame.iSrc,m_sTheFrame.iDest);
			m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());
			return true;
		}
		
		if (m_sTheFrame.caData[2]==VC_RECORDER_OFF){
			sDataMsg = MOOSFormat("MessageType=SENSOR_RECORDER,SourcePlatformId=%d,DestinationPlatformId=%d,RecorderOn=0",
					      m_sTheFrame.iSrc,m_sTheFrame.iDest);
			m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());
			return true;
		}

		// otherwise, another CCL command
		sDataMsg = MOOSFormat("MessageType=COMMAND,SourcePlatformId=%d,DestinationPlatformId=%d,CommandNumber=%d",
				      m_sTheFrame.iSrc,m_sTheFrame.iDest,m_sTheFrame.caData[2]+(m_sTheFrame.caData[3]*256));
		m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());
		return true;

	}
	return false;
}

// Message for Generic Command
bool CFramer::SendMessageCommand()
{    
	std::string sDataMsg;

	if (m_sTheFrame.iSize) {           // if there is data, post it

		sDataMsg = MOOSFormat("MessageType=COMMAND,SourcePlatformId=%d,DestinationPlatformId=%d,CommandNumber=%d",
				      m_sTheFrame.iSrc,m_sTheFrame.iDest,m_sTheFrame.caData[2]+(m_sTheFrame.caData[3]*256));
		m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());

		return true;

	}
	return false;
}

// Messages for Plusnet Reports
bool CFramer::SendMessagePlusnetReport()
{    

	FIELDMESSAGE fieldMsg;
        std::string sDataMsg;

	if (m_sTheFrame.iSize) {           // if there is data, post it

                // unpack the frame to struct
		if (False_e==DecodeSensorReport(*(PackedCCLSensorReport_t *)m_sTheFrame.caData,&(fieldMsg.sensorReport))){
			MOOSTrace("pFramer: DecodeSensorReport failed.\n");
			return(false);
		}
	
		fieldMsg.type = FIELD_NODE_SENSOR_REPORT ;
		
		// pack the struct to string
		CFieldPackFunctor()(&sDataMsg,&fieldMsg);

		// post the string
		m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());

		return true;
	}
	return false;
}

// Message for Plusnet Command
bool CFramer::SendMessagePlusnetCommand()
{    

	FIELDMESSAGE fieldMsg;
        std::string sDataMsg;

	if (m_sTheFrame.iSize) {           // if there is data, post it

                // unpack the frame to struct
		if (False_e==DecodeSensorCommand(*(PackedCCLSensorCommand_t *)m_sTheFrame.caData,&(fieldMsg.sensorCommand))){
			MOOSTrace("pFramer: DecodeSensorCommand failed.\n");
			return(false);
		}
	
		fieldMsg.type = FIELD_NODE_SENSOR_COMMAND ;
		
		// pack the struct to string
		CFieldPackFunctor()(&sDataMsg,&fieldMsg);

		// post the string
		m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());

		return true;
	}
	return false;
}

// Message for XRay behavior
bool CFramer::SendMessageXRayBehavior()
{    
	std::string sDataMsg;
	xray_behavior sXRay;
	mdat_xray_behavior *psXRayPacked;

	if (m_sTheFrame.iSize) {           // if there is data, post it
		// parse frame to struct
		psXRayPacked = (mdat_xray_behavior *)m_sTheFrame.caData ;
		decodeXRayBehavior(psXRayPacked,&sXRay); // in XRayPackets.cpp

		sDataMsg=MOOSFormat("MessageType=XRAY_BEHAVIOR,SourcePlatformId=%d,DestinationPlatformId=%d,"
				    "BehaviorType=%d,Depth1=%.1f,Depth2=%.1f,Depth3=%.1f,Latitude1=%.6f,"
				    "Longitude1=%.6f,Latitude2=%.6f,Longitude2=%.6f,Heading=%.2f,"
				    "HeadingRate=%.1f,Speed=%.1f,TimeStamp=%d,Flag1=%d,Flag2=%d,Flag3=%d,Flag4=%d,"
				    "Flag5=%d,Flag6=%d,Flag7=%d,Flag8=%d,Password=%c%c",
				    m_sTheFrame.iSrc,m_sTheFrame.iDest,sXRay.behaviorType,sXRay.depth1,
				    sXRay.depth2,sXRay.depth3,sXRay.latitude1,sXRay.longitude1,
				    sXRay.latitude2,sXRay.longitude2,sXRay.heading,sXRay.headingRate,
				    sXRay.speed,sXRay.time,sXRay.flag1,sXRay.flag2,sXRay.flag3,sXRay.flag4,
				    sXRay.flag5,sXRay.flag6,sXRay.flag7,sXRay.flag8,sXRay.password[0],sXRay.password[1]);

		m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());

		return true;

	}
	return false;
}

// Message for XRay status
bool CFramer::SendMessageXRayStatus()
{    
	std::string sDataMsg;
	xray_status sXRay;
	mdat_xray_status *psXRayPacked;
	

	if (m_sTheFrame.iSize) {           // if there is data, post it

		// parse frame to struct
		psXRayPacked = (mdat_xray_status *)m_sTheFrame.caData ;
		decodeXRayStatus(psXRayPacked,&sXRay); // in XRayPackets.cpp

		// form message
		sDataMsg=MOOSFormat("MessageType=XRAY_STATUS,SourcePlatformId=%d,DestinationPlatformId=%d,"
				    "TimeStamp=%ld,Energy=%.1f,Depth=%.1f,DepthRate=%.2f,Latitude=%.6f,"
				    "Longitude=%.6f,Altitude=%.1f,Heading=%.2f,HeadingRate=%.2f,Roll=%.1f,Pitch=%.1f,"
				    "BEVolume=%.1f,BehaviorResponse=%d,Fault=%d,Flag1=%d,Flag2=%d,Flag3=%d,Flag4=%d,"
				    "Flag5=%d,Flag6=%d,Flag7=%d,Flag8=%d,Contacts=%d",
				    m_sTheFrame.iSrc,m_sTheFrame.iDest,sXRay.time,sXRay.energy,sXRay.depth,
				    sXRay.depthRate,sXRay.latitude,sXRay.longitude,sXRay.altitude,sXRay.heading,
				    sXRay.headingRate,sXRay.roll,sXRay.pitch,sXRay.beVolume,sXRay.behaviorResponse,
				    sXRay.fault,sXRay.flag1,sXRay.flag2,sXRay.flag3,sXRay.flag4,sXRay.flag5,sXRay.flag6,
				    sXRay.flag7,sXRay.flag8,sXRay.contacts);

		m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());

		return true;

	}
	return false;
}

// Message for Unknown Type
bool CFramer::SendMessageUnknown()
{    
	std::string sDataMsg;
	char caHexData[640];

	if (m_sTheFrame.iSize) {           // if there is data, post it
		DataToHex(m_sTheFrame.caData,m_sTheFrame.iSize,caHexData);
		sDataMsg = MOOSFormat("MessageType=UNKNOWN,Src=%d,Dest=%d,Ack=%d,HexData=%s",
				      m_sTheFrame.iSrc,m_sTheFrame.iDest,m_sTheFrame.bAck,caHexData);
		m_Comms.Notify(m_sOutputMsgName,sDataMsg,MOOSTime());
		return true;
	}
	return false;
}

////////////////////////////////////////////////
// 
//  Form a modem frame from the frame
//
bool CFramer::SendFrame()
{
	std::string sDataMsg;
	char caHexData[640];

	if (m_sTheFrame.iSize) {           // if there is data, post it
		DataToHex(m_sTheFrame.caData,m_sTheFrame.iSize,caHexData);

		if (m_dLastMessageTimestamp>0.0)
		  sDataMsg = MOOSFormat("Command=DATA,MessageType=%s,Timestamp=%.1f,Src=%d,Dest=%d,Ack=%d,HexData=%s",
				      m_sLastMessageType.c_str(),m_dLastMessageTimestamp,m_sTheFrame.iSrc,m_sTheFrame.iDest,
				      m_sTheFrame.bAck,caHexData);
		else
		  sDataMsg = MOOSFormat("Command=DATA,MessageType=%s,Src=%d,Dest=%d,Ack=%d,HexData=%s",
				      m_sLastMessageType.c_str(),m_sTheFrame.iSrc,m_sTheFrame.iDest,
				      m_sTheFrame.bAck,caHexData);

		m_Comms.Notify(m_sOutputFrameName,sDataMsg,MOOSTime());
		return true;
	}
	return false;
}

////////////////////////////////////////////////
// 
//  Parse modem frames
//
bool CFramer::OnFrame(const std::string & sFrameIn)
{
        std::string sCmd;
        std::string sVal=sFrameIn;
	std::string sHexData;
        
        MOOSRemoveChars(sVal," \t");

        if ((!MOOSValFromString(sCmd,sVal,"Command"))&&(!MOOSValFromString(sCmd,sVal,"Data")))
                return MOOSFail("Could not extract \"Command\" or \"Data\" field %s",MOOSHERE);

        else if(MOOSStrCmp("DATA",sCmd)) // command modem driver with data for transmit 
        {	
		// Src
                if (!MOOSValFromString(m_sTheFrame.iSrc,sVal,"Src")){	
			MOOSTrace("pFramer: ERROR: OnFrame(): no \"Src\"\n");
			return false;
		}
		
		// Dest
                if (!MOOSValFromString(m_sTheFrame.iDest,sVal,"Dest")){
			MOOSTrace("pFramer: ERROR: OnFrame(): no \"Dest\"\n");
			return false;
		}
		
		// Ack
                if (!MOOSValFromString(m_sTheFrame.bAck,sVal,"Ack")){
			MOOSTrace("pFramer: ERROR: OnFrame(): no \"Ack\"\n");
			return false;
		}
		
		// Data
                if (!MOOSValFromString(sHexData,sVal,"HexData")){
			MOOSTrace("pFramer: ERROR: OnFrame(): no \"HexData\"\n");
			return false;
		}
	
		m_sTheFrame.iSize=HexToData((char *)sHexData.c_str(),m_sTheFrame.caData);

		m_sTheFrame.iType = m_sTheFrame.caData[0];

                MOOSTrace("pFramer: Got Modem Frame: %d bytes, type=%d\n",
			  m_sTheFrame.iSize,m_sTheFrame.iType);
		
		return true;
        }
        return false;
}


//
// HexToData - parse a string to a data buf
//
int CFramer::HexToData(char *hex, unsigned char *buf){

	int l,nbytes;
	char bs[4];
	
	nbytes = strlen(hex)/2;
	
	for (l=0;l<nbytes;l++){
		bs[0]=hex[l*2];
		bs[1]=hex[l*2+1];
		bs[2]='\0';
		buf[l]=strtoul(bs,NULL,16);
	}

	return (nbytes);
}

//
// DataToHex - form a string from a data buf
//
int CFramer::DataToHex(unsigned char *buf, int nbytes, char *hex){
	
	int l;
	char hexbyte[8];
	
	hex[0]=0;
	for (l=0;l<nbytes;l++){
		sprintf(hexbyte,"%02x",buf[l]);
		strcat(hex,hexbyte);
	}
	return (nbytes);
}
