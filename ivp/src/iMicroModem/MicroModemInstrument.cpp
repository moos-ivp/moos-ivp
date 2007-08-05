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
// MicroModemInstrument.cpp: implementation of the CMicroModemInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include "../MOOSLIB/MOOSLib.h"
#include "../MOOSGenLib/MOOSGenLib.h"

#include <sys/timeb.h>
#include <time.h>
#include <iostream>
#include <strstream>
#include <iomanip>
#include <algorithm>

#include "MicroModemInstrument.h"
#define MYPI (3.14159)
#include "ccl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMicroModemInstrument::CMicroModemInstrument()
{


}

CMicroModemInstrument::~CMicroModemInstrument()
{

}




//=========================================================
bool CMicroModemInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
	int nSourceAddress;
	MOOSMSG_LIST::iterator p;
	for(p = NewMail.begin();p!=NewMail.end();p++)
	{
		CMOOSMsg & rMsg = *p;
		if(!rMsg.IsSkewed(MOOSTime()))
		{
			if(rMsg.GetKey()=="TRACKER_OUT")
			{
				// parse the tracker message and queue it
				MOOSTrace("Got Tracker Fix: \"%s\"\n",rMsg.GetString().c_str());
				if (!OnTrackerFix(rMsg.GetString()))
					MOOSTrace("=> Fix Format UNKNOWN.\n");
			}
			if(rMsg.GetKey()=="MICROMODEM_COMMAND")
			{
                        // form low level command from high level command and send it to the modem
				MOOSTrace("Got a Command: \"%s\"\n",rMsg.GetString().c_str());
				if (!OnCommand(rMsg.GetString()))
					MOOSTrace("=> Command UNKNOWN.\n");
			}
		}
		// even if it's skewed

		// if(rMsg.GetKey()=="GPS_TIME")
		//{
		//     MOOSTrace("Got the GPS Time: \"%.2f\"\n",rMsg.GetDouble());
		//}

	}
	UpdateMOOSVariables(NewMail);
    return true;
}

//=========================================================
bool CMicroModemInstrument::OnConnectToServer()
{
 	int i;

	RegisterMOOSVariables();

	std::string sQoS;

        for (i=0;i<UMODEM_MAXNODES;i++) {
		sQoS = MOOSFormat("Node=%d, Quality=%d",i,0);	
		m_Comms.Notify("MICROMODEM_SERVICE",sQoS,MOOSTime());		
	}
	return true;
}

///this is where it all happens..
bool CMicroModemInstrument::Iterate()
{		
    if(GetModemMessage())
    {
        ParseModemMessage();
    }

	CheckTimeouts();
    
 return true;
}

//=========================================================
bool CMicroModemInstrument::OnStartUp()
{
    CMOOSInstrument::OnStartUp();
    std::string sDefautlTxType;

    // add all the variables we are interested in
	//
	MOOSTrace("Subscribing to Navigation data.\n");
        AddMOOSVariable("X","NAV_X","",0);                     // the NAV, TIME,etc. for telemetry messages
	AddMOOSVariable("Y","NAV_Y","",0);                  
	AddMOOSVariable("Depth","NAV_DEPTH","",0);  
	AddMOOSVariable("Yaw","NAV_YAW","",0);  
	AddMOOSVariable("Pitch","NAV_PITCH","",0);  
	AddMOOSVariable("Roll","NAV_ROLL","",0);  
	AddMOOSVariable("Altitude","NAV_ALTITUDE","",0); 
	AddMOOSVariable("Speed","NAV_SPEED","",0); 

	AddMOOSVariable("Time","DB_TIME","",0); 

	MOOSTrace("Subscribing to Tracker data.\n");
	AddMOOSVariable("TrackerOut","TRACKER_OUT","",0);      // contacts from Tracker to other units  
	AddMOOSVariable("TrackerIn","TRACKER_IN","",0);        // contacts from other units to Tracker

	MOOSTrace("Subscribing to ACS I/O: MICROMODEM_COMMAND, MICROMODEM_DATA, MICROMODEM_STATUS, MICROMODEM_QUALITY.\n");
	AddMOOSVariable("Command","MICROMODEM_COMMAND","",0);       // our control input 
	AddMOOSVariable("Data","MICROMODEM_DATA","",0);             // our data output 
	AddMOOSVariable("Raw","MICROMODEM_RAW","",0);               // our raw output 
	AddMOOSVariable("Service","MICROMODEM_SERVICE","",0);       // our QoS output 
	AddMOOSVariable("GPSTime","GPS_TIME","",0);                 // the GPS time 
	RegisterMOOSVariables();

	// moos file stuff
	m_bDefaultUseNAVXY = false;
	m_MissionReader.GetConfigurationParam("UseNAVXY",m_bDefaultUseNAVXY);
	m_MissionReader.GetConfigurationParam("DefaultTxType",m_sDefaultTxType);

	if (m_sDefaultTxType=="NAV")
	  MOOSTrace("Default Data Type is NAVXY data.\n");
	else if (m_sDefaultTxType=="TEST")
	  MOOSTrace("Default Data Type is TEST data.\n");    
	else 
	  MOOSTrace("Default Data Type is STATEXY data.\n");    
	  
    // open the port
			
    if(IsSimulateMode())
    {
        //not much to do...
    }
    else
    {
        //try to open 
        if(!SetupPort())
        {
            return false;
        }
        
        //try 10 times to initialise sensor
        //if(!InitialiseSensorN(10,"MicroModem"))
        //{
        //    return false;
        //}          
        UmodemInit();
        InitialiseSensor();

    }

	return true;
}

//=========================================================
// here we initialise the sensor, giving it start up values
bool CMicroModemInstrument::InitialiseSensor()
{    
      char caCmdStr[UMODEM_MAXMSG];

      MOOSTrace("Fetching Modem Address.\n");
      strcpy(caCmdStr,UmodemMakeCfgQueryCmd("SRC"));
      m_Port.Write(caCmdStr,strlen(caCmdStr));     
      return true;
}

bool CMicroModemInstrument::OnCommand(const std::string & sVal1)
{
//
//  In the case where modem transmissions are initiated on the AUV rather than the
//  ship/buoy/etc. pSchedule or some other process can post a message to "ACOUSTIC_COMMAND".
//  which will make a transmission.
//	
//	Currently, Commands are:
//
//          Command=PING,Dest=<acoustic address: int>
//          Command=TX,Unit=<acoustic address: int>
//
	
	int nDestinationAddress = 15 ; // 
	int nSourceAddress = 14 ; // 

	char caCmdStr[UMODEM_MAXMSG];
	std::string sCmd;
	std::string sVal=sVal1;
	std::string sCfgName;
	int nCfgValue;

        MOOSRemoveChars(sVal," \t");

	if(!MOOSValFromString(sCmd,sVal,"Command"))
		return MOOSFail("Could not extract \"Command\" field %s",MOOSHERE);

	if(MOOSStrCmp("PING",sCmd))    // command modem driver to PING
	{
		MOOSTrace("Got PING Command.\n");
		MOOSValFromString(nDestinationAddress,sVal,"Dest");
		strcpy(caCmdStr,UmodemMakePingCmd(m_nThisModemAddress,nDestinationAddress));
		MOOSTrace("Sent: %s",caCmdStr);
		m_Port.Write(caCmdStr,strlen(caCmdStr));
		return true;
	}
	else if(MOOSStrCmp("TX",sCmd)) // command modem driver to start tx cycle
	{
		MOOSTrace("Got TX Command.\n");
		MOOSValFromString(nDestinationAddress,sVal,"Dest");
		strcpy(caCmdStr,UmodemMakeCICmd(m_nThisModemAddress,nDestinationAddress,0,0));
		m_Port.Write(caCmdStr,strlen(caCmdStr));
		return true;
	}
	else if(MOOSStrCmp("RX",sCmd)) // command modem driver to start rx cycle
	{
		MOOSTrace("Got RX Command.\n");
		MOOSValFromString(nSourceAddress,sVal,"Src");
		strcpy(caCmdStr,UmodemMakeCICmd(nSourceAddress,m_nThisModemAddress,0,0));
		m_Port.Write(caCmdStr,strlen(caCmdStr));
		return true;
	}
        else if(MOOSStrCmp("CFG",sCmd)) // command modem driver to start rx cycle
	{
		MOOSTrace("Got CFG Command.\n");
		MOOSValFromString(nCfgValue,sVal,"Value");
		MOOSValFromString(sCfgName,sVal,"Name");
		strcpy(caCmdStr,UmodemMakeCfgCmd((char *)sCfgName.c_str(),nCfgValue));
		m_Port.Write(caCmdStr,strlen(caCmdStr));
		return true;
	}
	return false;
}


bool CMicroModemInstrument::OnModemData(void)
{
//  process new acoustic data

      sDataFrame *sRx;

      sRx = UmodemGetRXData();
      if (sRx->iSize)
            DoDecodeFrame(sRx);

      return true;
}

bool CMicroModemInstrument::CheckTimeouts(void)
{

	return true;
}

bool CMicroModemInstrument::PublishLastQoS(void)
{
	std::string sQoS;
	int iSrc;

    if (iLastModemMsgType==UMODEM_BINDATA)
		iSrc=UmodemGetRXSrc();
    else if ((iLastModemMsgType==UMODEM_PING)||(iLastModemMsgType==UMODEM_PINGREP))
		iSrc=UmodemGetLastPingSrc();
	else if (iLastModemMsgType==UMODEM_ACK)
	    iSrc=UmodemGetLastAckSrc();
	else
		return false;

	sQoS = MOOSFormat("Node=%d, Quality=%d",iSrc,UmodemGetLastDQF());	
    m_Comms.Notify("MICROMODEM_SERVICE",sQoS,MOOSTime());		

    MOOSTrace("Quality of Service:%s\n", sQoS.c_str());

    dtLastGood[iSrc]=MOOSTime();

	return true;
}

bool CMicroModemInstrument::OnTrackerFix(const std::string & sVal)
{
// When we get tracks, we add them to the track queue, awaiting transmit.
//
// note: By default, the modem does not transmit, just queues waiting for transmit.
//	     To queue transmit, post the command: "Command=TX,Unit=<Dest>" to send data.
//
// input is:
// ID=<int>,Type=<int>,Range=<meters>,Bearing=<deg>,Heading=<deg>,Speed=<knots>,Time=<hh:mm:ss>

    CCL_contact_det_t sDet;
	std::string sTime,sData;

    MOOSValFromString(sDet.id,sVal,"ID");
    MOOSValFromString(sDet.type,sVal,"Type");
    MOOSValFromString(sDet.range,sVal,"Range");
    MOOSValFromString(sDet.bearing,sVal,"Bearing");
    MOOSValFromString(sDet.heading,sVal,"Heading");
    MOOSValFromString(sDet.speed,sVal,"Speed");
    MOOSValFromString(sTime,sVal,"Time");
    
	sData=MOOSChomp(sTime,":");
    sDet.hrs=atoi(sData.c_str());

    sData=MOOSChomp(sTime,":");
    sDet.min=atoi(sData.c_str());

    sDet.sec=atoi(sTime.c_str());
	MOOSTrace("Queuing Det: ID=%d,Type=%d,Range=%.1f,Bearing=%.1f,Heading=%.1f,Speed=%.1f,Time=%02d:%02d:%02d\n",
				sDet.id,sDet.type,sDet.range,sDet.bearing,sDet.heading,sDet.speed,sDet.hrs,sDet.min,sDet.sec);

	m_ContactsToTx.push_back(sDet);

	MOOSTrace("Det Queue: %d contacts.\n",m_ContactsToTx.size());


	return true;
}
//
//    use the MOOS DB to make a NAVXY message frame for transmit
//
CCL_navxy_t * CMicroModemInstrument::MakeNavXYFromDB()
{
	// we use "local" variable names here - the mapping is set in CACS::OnStartup()
	//
        m_sNavXY.mode = CCLNAVXY;
	m_sNavXY.src = m_nThisModemAddress ;
	m_sNavXY.x = GetMOOSVar("X")->GetDoubleVal();
	m_sNavXY.y = GetMOOSVar("Y")->GetDoubleVal();
	MOOSTrace("NAV_X=%6.2f, NAV_Y=%6.2f\n",m_sNavXY.x,m_sNavXY.y);
        m_sNavXY.depth = GetMOOSVar("Depth")->GetDoubleVal();
	m_sNavXY.heading = GetMOOSVar("Yaw")->GetDoubleVal()/MYPI*(-180.0);
	m_sNavXY.speed = GetMOOSVar("Speed")->GetDoubleVal();
	m_sNavXY.fix_type = FIXISACOUSTIC;   // FIXME not always acoustic - could be GPS, INS, etc.

	// time
	double dfFixTime = GetMOOSVar("X")->GetTime(); 
        double dfPacketTime = MOOSTime()+1.0;
        m_sNavXY.fix_time = (unsigned long)dfFixTime;
        m_sNavXY.packet_time = (unsigned long)dfPacketTime;

	// FIXME - still need cep, c_sound, gps_nsat, gpsstd, synch_min

	return &(m_sNavXY);
}


//
//    use the MOOS DB to make a STATEXY message frame for transmit
//
CCL_statexy_t * CMicroModemInstrument::MakeStateXYFromDB()
{
	// we use "local" variable names here - the mapping is set in CACS::OnStartup()
	//
	m_sStateXY.mode = CCLSTATEXY;
	m_sStateXY.src = m_nThisModemAddress ;
	m_sStateXY.x =(int)GetMOOSVar("X")->GetDoubleVal();
        m_sStateXY.y =(int)GetMOOSVar("Y")->GetDoubleVal();
        m_sStateXY.depth = GetMOOSVar("Depth")->GetDoubleVal();
	m_sStateXY.heading = GetMOOSVar("Yaw")->GetDoubleVal()/MYPI*(-180.0);
	m_sStateXY.alt = GetMOOSVar("Altitude")->GetDoubleVal();
	m_sStateXY.est_velocity = GetMOOSVar("Speed")->GetDoubleVal();
	m_sStateXY.pitch = GetMOOSVar("Pitch")->GetDoubleVal()/MYPI*(-180.0); // sign??
	m_sStateXY.roll = GetMOOSVar("Roll")->GetDoubleVal()/MYPI*(-180.0); // sign??
	
	// how to use this?
	// double dfTX = GetMOOSVar("X")->GetTime(); 

        // FIXME - we still need battery_percent, fix_age, mission_mode, goal_id, goal_x, goal_y, watts_encoded

        return &(m_sStateXY);
}


//
//    use the DB and the ContactList to make a CONTACTXY message frame for transmit
//
CCL_contactxy_t * CMicroModemInstrument::MakeContactXYFromQ()
{
        CCL_contact_det_t Contact;
	int l;
	// we use "local" variable names here - the mapping is set in CACS::OnStartup()
	//
	m_sContactXY.mode = CCLCONTACTXY;
	m_sContactXY.src = (unsigned int)GetMOOSVar("ModemSource")->GetDoubleVal();
	m_sContactXY.x = GetMOOSVar("X")->GetDoubleVal();
        m_sContactXY.y = GetMOOSVar("Y")->GetDoubleVal();
        m_sContactXY.heading = GetMOOSVar("Yaw")->GetDoubleVal()/MYPI*(-180.0);

	double dfTX = GetMOOSVar("X")->GetTime(); // we need hrs,min,sec from this
        // m_sContactXY.hrs = 
        // m_sContactXY.min = 
        // m_sContactXY.sec = 
  
	// pop contact(s) from list
	m_sContactXY.ncontacts=0;

	// get 3 contacts if possible
	for (l=0;l<3;l++){
		if (m_ContactsToTx.size()) {
			Contact=m_ContactsToTx.front(); // not possible - assign a method for "="
			m_ContactsToTx.pop_front(); 
			m_sContactXY.det[l] = Contact;
			m_sContactXY.ncontacts++;
		}
	}
        //
	if (m_ContactsToTx.size())
		m_sContactXY.ncontacts++;

	return &(m_sContactXY);
}

bool CMicroModemInstrument::DoDecodeFrame(sDataFrame *f)
{
	std::string sTx;
	double TxTime=0.0;
        double RxTime=0.0;
        double FixTime=0.0;
        double dX;
        int iX;

	// take the type and run the appropriate
	switch (f->iType) {
	case CCLTEST:
		sTx = MOOSFormat("Data=RXDATA,Src=%d,Dest=%d,Type=TEST",
			f->iSrc,f->iDest);	
    	m_Comms.Notify("MICROMODEM_DATA",sTx,MOOSTime());		
		break;

	case CCLCONTACTXY:
		CCL_RxContactXYFrame(f,&m_sContactXY);
		sTx = MOOSFormat("Data=RXDATA,Src=%d,Dest=%d,Type=CONTACTXY,Time=%02d:%02d:%02d,X=%.1f,Y=%.1f,Heading=%.1f",
			f->iSrc,f->iDest,m_sContactXY.hrs,m_sContactXY.min,m_sContactXY.sec,m_sContactXY.x,m_sContactXY.y
			,m_sContactXY.heading);	

    	m_Comms.Notify("MICROMODEM_DATA",sTx,MOOSTime());		
		return true;

	case CCLNAVXY:
	      // MOOSTrace("RxFrame: f[15..12]=0x%02x%02x%02x%02x\n",f->caData[15],f->caData[14],
	      //			f->caData[13],f->caData[12]);
	      // MOOSTrace("iX=%d,dX=%.2f",iX,dX);
	      CCL_RxNavXYFrame(f,&m_sNavXY);
              // do travel time here RxTime=%%%

	      sTx = MOOSFormat("Data=RXDATA,Src=%d,Dest=%d,Type=NAVXY,X=%.1f,Y=%.1f,Depth=%.1f,Heading=%.2f,FixTime=%.1f,TxTime=%.2f,RxTime=%.2f",
			f->iSrc,f->iDest,m_sNavXY.x,m_sNavXY.y,m_sNavXY.depth,m_sNavXY.heading,m_sNavXY.fix_time,m_sNavXY.packet_time,RxTime);	
    	      m_Comms.Notify("MICROMODEM_DATA",sTx,MOOSTime());		
	      MOOSTrace(sTx);
	      return true;

	case CCLSTATEXY:
		CCL_RxStateXYFrame(f,&m_sStateXY);
		sTx = MOOSFormat("Data=RXDATA,Src=%d,Dest=%d,Type=STATEXY,Time=%.1f,X=%.1f,Y=%.1f,Heading=%.1f",
			f->iSrc,f->iDest,m_sStateXY.x,m_sStateXY.x,m_sStateXY.depth,m_sStateXY.heading);	
    	        m_Comms.Notify("MICROMODEM_DATA",sTx,MOOSTime());		
		return true;
	}
	return false;
}


bool CMicroModemInstrument::GetModemMessage()
{
    if(!IsSimulateMode())
    {
        //here we actually access serial ports etc
        
		std::string sModemMessage;
        double dfModemMessageTime;
        
        if(m_Port.IsStreaming())
        {
            if(!m_Port.GetLatest(sModemMessage,dfModemMessageTime))
            {
                return false;
            }	    
        }
        else
        {
            if(!m_Port.GetTelegram(sModemMessage,0.5))
            {
                return false;
            }
        }
        
        // publish to local struct, for parsing
        sLastModemMessage = sModemMessage ;
    }
    else
    {
        // put message to sLastModemMessage, for parsing
    }

	m_Comms.Notify("MICROMODEM_RAW",sLastModemMessage,MOOSTime());         
    
    return true;
    
}

bool CMicroModemInstrument::ParseModemMessage()
{
    int iModemStatus;
    int iParserRet;
    char caCmdStr[UMODEM_MAXMSG];
    std::string sModemStatusMessage;

    // post message to modem stack
    iParserRet=UmodemParse((char *)sLastModemMessage.c_str());

    iModemStatus = UmodemGetStatusInt();
    if (iModemStatus != iOldModemStatus) {
		iOldModemStatus=iModemStatus;
		sModemStatusMessage=MOOSFormat("%s",UmodemGetStatusString());
		MOOSTrace("New Modem Status: %s [%d]\n",UmodemGetStatusString(),iOldModemStatus);
     	m_Comms.Notify("MICROMODEM_STATUS",sModemStatusMessage,MOOSTime());         
	}

    iLastModemMsgType = iParserRet ;

	switch (iParserRet) {

	case UMODEM_BINDATA:
	      OnModemData();
	      // parse the received data, publish qos data
	      PublishLastQoS();
	      break;

	case UMODEM_DATREQ:
	      OnDataRequest();
	      // supply data for transmit,
	      break;

	case UMODEM_PINGREP:
	      // publish range data, publish qos data
	      OnPingReply();
	      PublishLastQoS();
	      break;

        case UMODEM_ACK:
	      // publish QoS
	      PublishLastQoS();
	      break;

	case UMODEM_PING:
	      // publish qos data
	      PublishLastQoS();
	      break;
  
        case UMODEM_TOA:
	      // publish ToA data
              break;

	case UMODEM_QUALITY:
	      MOOSTrace("last DQF = %d\n",UmodemGetLastDQF());
	      break;
   
        case UMODEM_ADDRESS:	
	      MOOSTrace("Got Address: SRC=[%d]\n",UmodemGetAddress());
	      m_nThisModemAddress=UmodemGetAddress();

	      // if (! m_nThisModemAddress==UmodemGetAddress()){
	      //	strcpy(caCmdStr,UmodemMakeCfgCmd("SRC",2));
	      //	m_Port.Write(caCmdStr,strlen(caCmdStr));     
	      // }
              break;
	}

	return true;
    
}

bool CMicroModemInstrument::OnDataRequest(){

  sDataFrame *asTxFrame;
  std::string sTx;        
  int nSrc,nDest;
  int SendTestData=0;
  CCL_navxy_t *cnNavXY;
  char caDataMsg[UMODEM_MAXMSG];
  char *cpMsg;
  nSrc=m_nThisModemAddress;
  nDest=UmodemGetCIDest();
  
  //fill data with hex from one of our data sources
		if (SendTestData) {
		      MOOSTrace("Sending TEST data.\n");
		      asTxFrame=CCL_MakeTestFrame(nSrc,nDest,1,UmodemGetTXDataSize());
		} else if (m_ContactsToTx.size()) { 
		      MOOSTrace("Sending CONTACTXY data.\n");
		      asTxFrame=CCL_MakeContactXYFrame(nSrc,nDest,1,MakeContactXYFromQ());  // highest priority tracker contacts
		      MOOSTrace("Sending CONTACTXY data.\n");
		} else if (m_sDefaultTxType=="NAV") {
		      MOOSTrace("Sending NAVXY data.\n");
		      cnNavXY=MakeNavXYFromDB();
		      // MOOSTrace("Got NAVXY struct from DB.\n");
		      asTxFrame=CCL_MakeNavXYFrame(nSrc,nDest,1,cnNavXY);      // if PPS, be a NAV beacon
		      // MOOSTrace("TxFrame: f[15..12]=0x%02x%02x%02x%02x\n",asTxFrame->caData[15],asTxFrame->caData[14],
		      //		asTxFrame->caData[13],asTxFrame->caData[12]);
		      // MOOSTrace("Made Frame\n");
		} else {
		      MOOSTrace("Sending STATEXY data.\n");
		      asTxFrame=CCL_MakeStateXYFrame(nSrc,nDest,1,MakeStateXYFromDB());  // if no Contacts, no PPS, send State
		}
		cpMsg=UmodemMakeTxString(asTxFrame);
		// MOOSTrace("MakeTxString(): cpMsg[%0x%08x]: %d chars: %s\n",(long)cpMsg,strlen(cpMsg),cpMsg);
		strcpy(caDataMsg,cpMsg);
		// MOOSTrace("strcpy(): caDataMsg[%0x%08x]: %d chars: %s\n",(long)caDataMsg,strlen(caDataMsg),caDataMsg);
		m_Port.Write(caDataMsg,strlen(caDataMsg));

		return true;
}

bool CMicroModemInstrument::OnPingReply(){

		std::string sPingMsg;        
		int nSrc,nDest;
        double dOWTT;

		nSrc=UmodemGetLastPingSrc();
		nDest=UmodemGetLastPingDest();
		if (nDest==m_nThisModemAddress) {
			dOWTT=(double)UmodemGetLastPingOWTT();
			sPingMsg = MOOSFormat("Data=PINGREPLY,Src=%d,Dest=%d,OWTT=%7.4f",nSrc,nDest,dOWTT);
    		m_Comms.Notify("MICROMODEM_DATA",sPingMsg,MOOSTime());		
		} else {
			sPingMsg = MOOSFormat("Data=PING,Src=%d,Dest=%d,",nSrc,nDest);
    		m_Comms.Notify("MICROMODEM_DATA",sPingMsg,MOOSTime());		
		}
		return true;

}
