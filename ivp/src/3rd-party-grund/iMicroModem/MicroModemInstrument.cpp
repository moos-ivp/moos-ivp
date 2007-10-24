
// iMicroModem MOOS Modem driver, was developed
// by Matt Grund, Woods Hole Oceanographic Institution
//
// This code is licensed under a Creative Commons
// Attribution Non-Commercial Share-A-Like License,
// version 2.5.
//
// For more information, see the file "License.html"
// or the web site:
//
//  http://creativecommons.org/licenses/by-nc-sa/2.5/
//
// Copyright(c)2004, Matt Grund, WHOI. <mgrund@whoi.edu>

// #define MIKERB_BUILD_ENV
#ifdef MIKERB_BUILD_ENV
#include "../MOOSLIB/MOOSLib.h"
#include "../MOOSGenLib/MOOSGenLib.h"
#else
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#endif


#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <list>

#include "MicroModemInstrument.h"
#define MYPI (3.14159)
#include "umodem.h"
#include "version.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMicroModemInstrument::CMicroModemInstrument()
{

    MOOSTrace("==========================================================\n");
    MOOSTrace("\n");
    MOOSTrace("                   %s Version %d.%02d\n",VERSION_NAME,VERSION_MAJOR,VERSION_MINOR);
    MOOSTrace("\n");
    MOOSTrace(" Version $Id: MicroModemInstrument.cpp,v 1.3 2007-10-07 05:36:01 arjunab Exp $\n");
    MOOSTrace("\n");
    MOOSTrace("            (c) Copyright, 2006 Matt Grund,WHOI\n");
    MOOSTrace("==========================================================\n\n");

    m_iNumSettings = 0;
    m_iCurrentSetting = 0;
    m_iNumConfigs = 0;
    m_bIsConfigured = false;
    m_bHaveTxFrame=false;
    vehicleID = 55;
}


CMicroModemInstrument::~CMicroModemInstrument()
{

}




//=========================================================
bool CMicroModemInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
	
        MOOSMSG_LIST::iterator p;
        for(p = NewMail.begin();p!=NewMail.end();p++)
        {
                CMOOSMsg & rMsg = *p;
                if(rMsg.IsSkewed(MOOSTime())) {
			MOOSTrace("Got skewed message: %s=\"%s\"\n",rMsg.m_sKey.c_str(),rMsg.GetAsString().c_str());
			
		}
		
		if(rMsg.GetKey()==m_sCommandVarName)
		{
			if ((m_bIsConfigured)&&(m_iOldModemStatus>2)) {
				// form low level command from high level command and send it to the modem
				MOOSTrace("Got a Command: \"%s\"\n",rMsg.GetString().c_str());
				if (!OnCommand(rMsg.GetString()))
					MOOSTrace("=> Command UNKNOWN.\n");
			} else if (m_iOldModemStatus>2) {
				MOOSTrace("Ignoring Command: \"%s\"  (modem not configured yet)\n",rMsg.GetString().c_str());
			} else
				MOOSTrace("Ignoring Command: \"%s\"  (modem not ready yet)\n",rMsg.GetString().c_str());

			
			
		}
		if(rMsg.GetKey()==m_sFromModemVarName)
		{
                        // Got serial data via variable, parse it
			MOOSTrace("Got a Modem Serial Message: \"%s\"\n",rMsg.GetString().c_str());
			sLastModemMessage=rMsg.GetAsString();
			m_Comms.Notify(m_sRawVarName,sLastModemMessage,MOOSTime());
			ParseModemMessage();
		}

//Arjuna Added	
		if(rMsg.GetKey() == "VEHICLE_ID")
		{
			std::string vehi_ID = rMsg.GetAsString();
			MOOSTrace("The vehicle ID %s\n",vehi_ID.c_str());
			vehicleID = atoi(vehi_ID.c_str());
		}	
        }
        UpdateMOOSVariables(NewMail);
    return true;
}

//=========================================================
bool CMicroModemInstrument::OnConnectToServer()
{
        int i;

        DoSubscribe();

        std::string sQoS;
	
        // we know nothing about any other nodes, yet
        for (i=0;i<UMODEM_MAXNODES;i++) {
                sQoS = MOOSFormat("Node=%d, Quality=%d",i,0);
                m_Comms.Notify(m_sServiceVarName,sQoS,MOOSTime());
        }
        return true;
}

//=========================================================
bool CMicroModemInstrument::Iterate()
{
	// simple parsing
	if(GetModemMessage())
	{
		ParseModemMessage();
	}
	
	// stateful config
	if (!m_bIsConfigured)
		ConfigIterate();

        CheckTimeouts();
	
	return true;
}

//=========================================================
bool CMicroModemInstrument::ConfigIterate()
{


	// if waiting for reply, do nothing
	if (m_iOldModemStatus==UMODEM_CONFIGURING) {
		return true;
	}

	// if READY for a new command (reply was good) send next, or finish
	if (m_iOldModemStatus==UMODEM_IDLE) {

		std::string sCmd;

		MOOSTrace("Modem Configure[%d/%d]: setting \"%s\" to %d\n",m_iCurrentSetting+1,m_iNumSettings,
			  m_svSettingNames[m_iCurrentSetting].c_str(),m_ivSettingValues[m_iCurrentSetting]);

		sCmd=UmodemConfig((char *)(m_svSettingNames[m_iCurrentSetting].c_str()),m_ivSettingValues[m_iCurrentSetting]);

		SendModemString((char *)(sCmd.c_str()));

		m_iCurrentSetting++;

		if (m_iCurrentSetting==m_iNumSettings) {
			MOOSTrace("Configured %d settings, done.\n",m_iNumSettings);
			m_bIsConfigured=true;
			SetModemClock();
		}
		
		return true;
	}
	
	return false;
}



//=========================================================
bool CMicroModemInstrument::OnStartUp()
{
	CMOOSInstrument::OnStartUp();

	// parse modem config
	STRING_LIST Params;
	if(m_MissionReader.GetConfiguration(m_sAppName,Params))
	{
		//this will make columns in order
		//were declared in *.moos file
		Params.reverse();
		
		STRING_LIST::iterator p;
		
		MOOSTrace("\n");
		for(p=Params.begin();p!=Params.end();p++)
		{
			std::string sParam = *p;
			std::string sWhat = MOOSChomp(sParam,"=");
			int iValue;
			if(MOOSStrCmp(sWhat,"CONFIG"))
			{
				std::string sSettingName = MOOSChomp(sParam,",");
				std::string sSettingValue = sParam;
				
				// notify 
				iValue=atoi(sSettingValue.c_str());
				m_iNumSettings++;
				MOOSTrace("Modem Configuration #%d: \"%s\" => %d\n",
					  m_iNumSettings,sSettingName.c_str(),iValue);
				m_svSettingNames.push_back(sSettingName);
				m_ivSettingValues.push_back(iValue);
				if (sSettingName=="SRC")
					m_nThisModemAddress=iValue;

			}
		}
		if(m_iNumSettings)
			MOOSTrace("\nConfiguring %d settings, total.\n\n",m_iNumSettings);
		else {
			m_bIsConfigured = true;
			MOOSTrace("\nNot configuring modem\n\n");
		}
	} else {
		MOOSTrace("No Mission Configuration (moos file).\n");
		
	}
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
// here we initialise the sensor, getting the start up values
bool CMicroModemInstrument::InitialiseSensor()
{
	if (m_iNumSettings==0) {
		char caCmdStr[UMODEM_MAXMSG];
		MOOSTrace("Fetching Modem Settings.\n");
		strcpy(caCmdStr,UmodemMakeCfgQueryCmd("ALL"));
		SendModemString(caCmdStr);
	}
	
	return true;
}

bool CMicroModemInstrument::OnCommand(const std::string & sVal1)
{
//
//  In the case where modem transmissions are initiated on the AUV rather than the
//  ship/buoy/etc. pSchedule or some other process can post a message to "ACOUSTIC_COMMAND".
//  which will make a transmission.
//
//      Currently, Commands are:
//
//          Command=PING,Dest=<acoustic address: int>
//          Command=TX,Dest=<acoustic address: int>,Ack=<0|1>
//          Command=RX,Src=<acoustic address: int>,Ack=<0|1>
//          Command=NOISE,Duration=<milliseconds>
//          Command=CFG,Name=<name: 3 char array>,Value=<value: int>
//          Command=DATA,Src=<0:15>,Dest=<0:15>,Ack=<0|1>,HexData=<hex string>
//
        int nDestinationAddress = 15 ; 
        int nSourceAddress = 14 ; 
	int nAck = 0;
	
        int nDataRate = 0;
        char caCmdStr[UMODEM_MAXMSG];
        std::string sCmd;
        std::string sVal=sVal1;
        std::string sCfgName;
	std::string sHexData;
	std::string sMessageType;
        int nCfgValue;
	int nDuration;

        MOOSRemoveChars(sVal," \t");

        if(!MOOSValFromString(sCmd,sVal,"Command"))
                return MOOSFail("Could not extract \"Command\" field %s",MOOSHERE);

        if(MOOSStrCmp("PING",sCmd))    // command modem driver to PING
        {
                MOOSTrace("Got PING Command.\n");
                MOOSValFromString(nDestinationAddress,sVal,"Dest");
                strcpy(caCmdStr,UmodemMakePingCmd(m_nThisModemAddress,nDestinationAddress));
                MOOSTrace("Sent: %s",caCmdStr);
                SendModemString(caCmdStr);
                return true;
        }
        else if(MOOSStrCmp("TX",sCmd)) // command modem driver to start tx cycle
        {
                MOOSTrace("Got TX Command.\n");
                MOOSValFromString(nDestinationAddress,sVal,"Dest");
		if (!MOOSValFromString(nDataRate,sVal,"Rate"))
			nDataRate=0;
		if (!MOOSValFromString(nAck,sVal,"Ack"))
		  nAck=0;
                strcpy(caCmdStr,UmodemMakeCICmd(m_nThisModemAddress,nDestinationAddress,nDataRate,0));
                SendModemString(caCmdStr);
                return true;
        }
        else if(MOOSStrCmp("RX",sCmd)) // command modem driver to start rx cycle
        {
                MOOSTrace("Got RX Command.\n");
		
                MOOSValFromString(nSourceAddress,sVal,"Src");
		if (!MOOSValFromString(nDataRate,sVal,"Rate"))
		  nDataRate=0;
		if (!MOOSValFromString(nAck,sVal,"Ack"))
		  nAck=0;
                strcpy(caCmdStr,UmodemMakeCICmd(nSourceAddress,m_nThisModemAddress,nDataRate,nAck));
                SendModemString(caCmdStr);
                return true;
        }
        else if(MOOSStrCmp("CFG",sCmd)) // command modem driver to configure the modem
        {
                MOOSTrace("Got CFG Command.\n");
                MOOSValFromString(nCfgValue,sVal,"Value");
                MOOSValFromString(sCfgName,sVal,"Name");
                strcpy(caCmdStr,UmodemMakeCfgCmd((char *)sCfgName.c_str(),nCfgValue));
                SendModemString(caCmdStr);
                return true;
        }
        else if(MOOSStrCmp("NOISE",sCmd)) // command modem driver to make noise measurement
        {
                MOOSTrace("Got NOISE Command.\n");
                if (!MOOSValFromString(nDuration,sVal,"Duration"))
		  nDuration=200; // milliseconds
                strcpy(caCmdStr,UmodemMakeCFRCmd(nDuration));
                SendModemString(caCmdStr);
                return true;
        }
        else if(MOOSStrCmp("DATA",sCmd)) // command modem driver with data for transmit 
        {
                MOOSTrace("Got DATA Command.\n");
		m_bHaveTxFrame=true;
		// MessageType
                if (!MOOSValFromString(sMessageType,sVal,"MessageType"))
			// default: our address
			MOOSTrace("DATA MessageType is %s.\n",sMessageType.c_str());
		
		// Src
                if (!MOOSValFromString(m_sTxFrame.iSrc,sVal,"Src"))
			// default: our address
			m_sTxFrame.iSrc=m_nThisModemAddress;
		// Dest
                if (!MOOSValFromString(m_sTxFrame.iDest,sVal,"Dest"))
			// default: 0
			m_sTxFrame.iDest=0;
		// Ack
                if (!MOOSValFromString(m_sTxFrame.bAck,sVal,"Ack"))
			// default: no Ack
			m_sTxFrame.bAck=0;
		// Data
                if (!MOOSValFromString(sHexData,sVal,"HexData"))
			// default:  test data
			sHexData="000102030405060708090a0b0c0d0e0f101112134115161718191a1b1c1d1e1f";
		
		m_sTxFrame.iSize=NMEA_HexToData((char *)sHexData.c_str(),m_sTxFrame.caData);
		
		// if waiting for data, form and send command
		if (m_iOldModemStatus==UMODEM_REQUESTINGDATA){
			strcpy(caCmdStr,UmodemMakeTxString(&(m_sTxFrame)));
			SendModemString(caCmdStr);
			m_bHaveTxFrame=false;
		} else {
			strcpy(caCmdStr,UmodemMakeTxString(&(m_sTxFrame)));
			SendModemString(caCmdStr);
			m_bHaveTxFrame=true;
		}		
		return true;
        }
        return false;
}

//=========================================================
bool CMicroModemInstrument::OnModemConfig(char *name,int value)
{

	int l,nIndex=-1;
	std::string sName=name;
	
	// find it
	for (l=0;l<m_iNumConfigs;l++)
		if(m_svConfigNames[l]==sName)
			nIndex=l; // found it
	
	// if found, overwrite setting, else append setting to vector
	if (nIndex!=-1)
		m_ivConfigValues[l]=value;
	else {
		m_svConfigNames.push_back(sName);
		m_ivConfigValues.push_back(value);
		m_iNumConfigs++;
	}
	
	return true;
}



//=========================================================
bool CMicroModemInstrument::OnModemData(void)
{
      //  we got new acoustic data
      sDataFrame *sRx;
      std::string sDataMsg;
      char caHexData[514];
      
      sRx = UmodemGetRXData();
      if (sRx->iSize) {           // if there is data, post it
	      NMEA_DataToHex(sRx->caData,sRx->iSize,caHexData);
	      sDataMsg = MOOSFormat("Data=DATA,Src=%d,Dest=%d,Ack=%d,HexData=%s",
				    sRx->iSrc,sRx->iDest,sRx->bAck,caHexData);
	      m_Comms.Notify(m_sDataVarName,sDataMsg,MOOSTime());
	
//	MOOSTrace("OnModemData Act %d\n",sRx->bAck);

      }
      return true;
}

//=========================================================
bool CMicroModemInstrument::CheckTimeouts(void)
{

        return true;
}

//=========================================================
bool CMicroModemInstrument::PublishLastQoS(void)
{
        std::string sQoS;
        int iSrc;

	if (m_iLastModemMsgType==UMODEM_BINDATA)
                iSrc=UmodemGetRXSrc();
	else if ((m_iLastModemMsgType==UMODEM_PING)||(m_iLastModemMsgType==UMODEM_PINGREP))
                iSrc=UmodemGetLastPingSrc();
        else if (m_iLastModemMsgType==UMODEM_ACK)
		iSrc=UmodemGetLastAckSrc();
        else
                return false;
	
        sQoS = MOOSFormat("Node=%d,Quality=%d",iSrc,UmodemGetLastDQF());
	m_Comms.Notify(m_sServiceVarName,sQoS,MOOSTime());
	
	MOOSTrace("Quality of Service:%s\n", sQoS.c_str());
	
	dtLastGood[iSrc]=MOOSTime();
	
        return true;
}





//=========================================================
bool CMicroModemInstrument::GetModemMessage()
{
	if(!IsSimulateMode())
	{
		//here we actually access serial ports etc
		
                std::string sModemMessage;
		double dfModemMessageTime;
		
		if(m_Port.IsStreaming())
		{
//			if(!m_Port.GetLatest(sModemMessage,dfModemMessageTime))
			if(!m_Port.GetEarliest(sModemMessage,dfModemMessageTime))
			{
				return false;
			}
		}
		else
		{
			if(!m_Port.GetTelegram(sModemMessage,10.0))
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
	
        m_Comms.Notify(m_sRawVarName,sLastModemMessage,MOOSTime());

//Arjuna Included this for getting the ACK bit
	std::string modem_data = sLastModemMessage;

	MOOSChomp(modem_data,"$");
	std::string TYPE = MOOSChomp(modem_data,",");

	MOOSTrace("TYPE = %s\n",TYPE.c_str());

	if(TYPE == "CACYC")
	{
		std::string CMD = MOOSChomp(modem_data,",");
		std::string ADR1 = MOOSChomp(modem_data,",");
		std::string ADR2 = MOOSChomp(modem_data,",");

		m_Comms.Notify("POLLING_ADDRESS",ADR2);

		std::string PACKETYPE = MOOSChomp(modem_data,",");
		std::string ACK_VAL = MOOSChomp(modem_data,",");
		MOOSTrace("Address %s, ACK_VAL = %s\n",ADR1.c_str(),ACK_VAL.c_str());
		if(atoi(ADR1.c_str()) == vehicleID)	
		 m_Comms.Notify("ACK_VAL",atoi(ACK_VAL.c_str()));
	}
//  End of Arjuna's code	
	return true;
}

//=========================================================
bool CMicroModemInstrument::ParseModemMessage()
{
	int iModemStatus;
	int iParserRet;
	std::string sModemStatusMessage;
	std::string sModemMsg;
	char caCmdStr[640];
	
	
	// post message to modem stack
	iParserRet=UmodemParse((char *)sLastModemMessage.c_str());
	
	iModemStatus = UmodemGetStatusInt();
	if (iModemStatus != m_iOldModemStatus) {
		m_iOldModemStatus=iModemStatus;
		sModemStatusMessage=MOOSFormat("%s",UmodemGetStatusString());
		MOOSTrace("New Modem Status: %s [%d]\n",UmodemGetStatusString(),m_iOldModemStatus);
		m_Comms.Notify(m_sStatusVarName,sModemStatusMessage,MOOSTime());   // text status
		m_Comms.Notify(m_sStatusCodeVarName,iModemStatus,MOOSTime());      // numeric status code
		
	}
	
	m_iLastModemMsgType = iParserRet ;
	
	switch (iParserRet) {
		
	case UMODEM_REVERT:      // set modem clock at every revert for SNV
		if (m_bIsConfigured)
			SetModemClock();
		break;

	case UMODEM_BINDATA:
		OnModemData();
		// parse the received data, publish qos data
		PublishLastQoS();
		break;
		
	case UMODEM_DATREQ:
	
		MOOSTrace("Got Data Request: Dest=%d, Size=%d, Ack=%d\n",
			  UmodemGetTXDataDest(),UmodemGetTXDataSize(),UmodemGetTXDataAckRequested());
		m_iOldModemStatus=UMODEM_REQUESTINGDATA;

        //Arjuna Added
		m_Comms.Notify("LAST_POLLED_TIME",MOOSTime(),MOOSTime());

		// Always ask for new data - required for PSU NAFCON Ack bit compliance
		//
		m_bHaveTxFrame = false ;

		// if we hav new data or unack'ed data, send it, else get more
		if (m_bHaveTxFrame) {
			MOOSTrace("Sending cached data: Dest=%d, Size=%d\n",m_sTxFrame.iDest,m_sTxFrame.iSize);
			strcpy(caCmdStr,UmodemMakeTxString(&(m_sTxFrame)));
			SendModemString(caCmdStr);
			if (m_sTxFrame.bAck==0)
				m_bHaveTxFrame = false;
		} else {
			MOOSTrace("No cached data, posting request to DB.\n");
			sModemMsg=MOOSFormat("Data=DATAREQUEST,Dest=%d,Size=%d,Ack=%d",
				UmodemGetTXDataDest(),
				UmodemGetTXDataSize(),
				UmodemGetTXDataAckRequested());
			m_Comms.Notify(m_sDataVarName,sModemMsg,MOOSTime());
		}
		break;
		
	case UMODEM_PINGREP:
		// publish range data, publish qos data
		OnPingReply();
		PublishLastQoS();
		break;
		
	case UMODEM_ACK:
		// ack frame, or
		if (m_sTxFrame.bAck) {
			m_bHaveTxFrame=false;
			MOOSTrace("Got ACK - flushing frame.\n");
		}
		PublishLastQoS();
		break;
		
	case UMODEM_PING:
		// publish qos data
		PublishLastQoS();
		break;
		
	case UMODEM_TOA:
		// publish ToA data
		sModemMsg=MOOSFormat("Data=ARRIVALTIME,Src=%d,Time=%.4f,Mode=%d",
				     UmodemGetArrivalTimeSrc(),
				     UmodemGetArrivalTime(),
				     UmodemGetArrivalTimeMode());
		m_Comms.Notify(m_sDataVarName,sModemMsg,MOOSTime());

		if ((m_bIsConfigured)&&(UmodemGetArrivalTimeMode()<3))
			SetModemClock();   // set the modem clock

		break;
		
	case UMODEM_QUALITY:
		MOOSTrace("last DQF = %d\n",UmodemGetLastDQF());
		break;
		
	case UMODEM_CFG:
		// remember modem settings
		OnModemConfig(UmodemGetLastCfgName(),UmodemGetLastCfgValue());		
		MOOSTrace("Got Config: %s=%d    (%d total)\n",
			  UmodemGetLastCfgName(),UmodemGetLastCfgValue(),m_iNumConfigs);
		if (0==strncmp("SRC",UmodemGetLastCfgName(),3)){
			m_nThisModemAddress=UmodemGetAddress();
			MOOSTrace("Got Address [%d]\n",m_nThisModemAddress);
		}
		break;
	}
	
	return true;
}



//=========================================================
bool CMicroModemInstrument::OnPingReply(){

		std::string sPingMsg;
		int nSrc,nDest;
		double dOWTT;

		nSrc=UmodemGetLastPingSrc();
		nDest=UmodemGetLastPingDest();
		if (nDest==m_nThisModemAddress) {
				dOWTT=(double)UmodemGetLastPingOWTT();
				sPingMsg = MOOSFormat("Data=PINGREPLY,Src=%d,Dest=%d,OWTT=%7.4f",nSrc,nDest,dOWTT);
				m_Comms.Notify(m_sDataVarName,sPingMsg,MOOSTime());
		} else {
			sPingMsg = MOOSFormat("Data=PING,Src=%d,Dest=%d",nSrc,nDest);
			m_Comms.Notify(m_sDataVarName,sPingMsg,MOOSTime());
		}
		return true;
}


//=========================================================
bool CMicroModemInstrument::SendModemString(char *caStr)
{
	std::string sStr;

	 m_Port.Write(caStr,strlen(caStr));
	 sStr.assign(caStr);
	 m_Comms.Notify(m_sToModemVarName,sStr,MOOSTime());

	 return true;
}


//=========================================================
//
//   Configurable MOOS variable names: the prefix is set in 
//   the .moos file (or not: defaults to MICROMODEM). 
//
bool CMicroModemInstrument::DoFormVarNames()
{

	m_MissionReader.GetConfigurationParam("VarNamePrefix",m_sVarNamePrefix);
	
	// use the default, if not set somehow
	if (m_sVarNamePrefix.length()==0)
		m_sVarNamePrefix="MICROMODEM";

	MOOSTrace("Variable Prefix: \"%s\"\n",m_sVarNamePrefix.c_str());
	
	// DATA
	m_sDataVarName=MOOSFormat("%s_DATA",m_sVarNamePrefix.c_str());
	// COMMAND
	m_sCommandVarName=MOOSFormat("%s_COMMAND",m_sVarNamePrefix.c_str());
	// RAW
	m_sRawVarName=MOOSFormat("%s_RAW",m_sVarNamePrefix.c_str());
	// SERVICE
	m_sServiceVarName=MOOSFormat("%s_SERVICE",m_sVarNamePrefix.c_str());
	// STATUS TEXT
	m_sStatusVarName=MOOSFormat("%s_STATUS",m_sVarNamePrefix.c_str());
	// STATUS CODE
	m_sStatusCodeVarName=MOOSFormat("%s_STATUS_CODE",m_sVarNamePrefix.c_str());
	// TO_MODEM
	m_sToModemVarName=MOOSFormat("%s_TO_MODEM",m_sVarNamePrefix.c_str());
	// FROM_MODEM
	m_sFromModemVarName=MOOSFormat("%s_FROM_MODEM",m_sVarNamePrefix.c_str());
	
	MOOSTrace("Forming variable names: done.\n");

	return true;
}

//========================================================
//  subscribe to MOOSVariables using string names
//
bool CMicroModemInstrument::DoSubscribe()
{

        // add all input variable names
        //
        DoFormVarNames();


	MOOSTrace("\n================= INPUT VARIABLES ==================\n");
        MOOSTrace("Input Variable:        Command: %s\n",m_sCommandVarName.c_str());
        AddMOOSVariable("Command",m_sCommandVarName,"",0);       // our control input
        MOOSTrace("Input Variable:   Serial Input: %s\n",m_sFromModemVarName.c_str());
        AddMOOSVariable("SerialIn",m_sFromModemVarName,"",0);      // serial tee, for pHuxley, etc.

	MOOSTrace("\n================ OUTPUT VARIABLES ==================\n");
        MOOSTrace("Output Variable:            Raw: %s\n",m_sRawVarName.c_str());
        MOOSTrace("Output Variable:        Service: %s\n",m_sServiceVarName.c_str());
        MOOSTrace("Output Variable:         Status: %s\n",m_sStatusVarName.c_str());
        MOOSTrace("Output Variable:    Status Code: %s\n",m_sStatusCodeVarName.c_str());
        MOOSTrace("Output Variable:  Serial Output: %s\n",m_sToModemVarName.c_str());

	m_Comms.Register("VEHICLE_ID",0);
        RegisterMOOSVariables();
        return true;
}

//========================================================
//  set the modems clock from the PC clock
//
bool CMicroModemInstrument::SetModemClock()
{
	struct timeval sTV;
	time_t tNow;
	struct tm *sNow;
	char caClkCmd[128];

	MOOSTrace("iMicroModem: setting modem clock.\n");
	
	// check for proper time to send clock command ie. frac sec < 0.75
	gettimeofday(&sTV,NULL);
	while (sTV.tv_usec>750000) {
		usleep(500);
		gettimeofday(&sTV,NULL);
	}

        // cool to send CLK command  now
	time(&tNow);
	sNow=localtime(&tNow);
	sprintf(caClkCmd,"$CCCLK,%04d,%02d,%02d,%02d,%02d,%02d",
		sNow->tm_year+1900,sNow->tm_mon+1,sNow->tm_mday,
		sNow->tm_hour,sNow->tm_min,sNow->tm_sec);
	NMEA_AppendChecksum(caClkCmd);
	SendModemString(caClkCmd);

	return true;
}
