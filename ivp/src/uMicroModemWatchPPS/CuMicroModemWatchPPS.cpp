// Watch traffic from modem and check for a
// “$CAERR,EXTSYNC timeout on txput,0*CS” message
// if it occurs, set the PPS sync to 0
//
// Author: Alexander Bahr abahr@mit.edu
// Created: 4 August 2007
// Last modified: 22 August 2007

// CuMicroModemWatchPPS.cpp: implementation of the CuMicroModemWatchPPS class.
//////////////////////////////////////////////////////////////////////////////

#include <iterator>
#include "CuMicroModemWatchPPS.h"

CuMicroModemWatchPPS::CuMicroModemWatchPPS()
{
	// constructor
}

CuMicroModemWatchPPS::~CuMicroModemWatchPPS()
{
	// destructor
}

///////////////////////////////////////////////////////////////////////////////
// mail handler
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemWatchPPS::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	
	for(p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		
		// check if is a <MODEM>_RAW string
		if(msg.m_sKey == modem_raw)
		{
			// check if it contains a $CAERR string
			if(msg.m_sVal.find("$CAERR") != string::npos)
			{
				
				if(verbose) 
				{
					MOOSTrace("CuMicroModemWatchPPS: $CAERR string detected:\n");
					MOOSTrace("                      %s\n",msg.m_sVal.c_str());
				}
				
				if(msg.m_sVal.find("EXTSYNC timeout on") != string::npos)
				{
					MOOSTrace("CuMicroModemWatchPPS: EXTSYNC timeout detected - PPS is missing\n");
					timeout_counter++;
					
					if(timeout_counter>=max_timeout)
					{
					
						MOOSTrace("CuMicroModemWatchPPS: maximum allowed number PPS timeouts (%i) reached\n",max_timeout);
						DisablePPS();
						MOOSTrace("CuMicroModemWatchPPS: PPS disabled\n");
						timeout_counter=0;
					}
					else
					{
						MOOSTrace("CuMicroModemWatchPPS: %i PPS timeout(s) detected (max allowed=%i)\n",timeout_counter,max_timeout);
					}
				}
				else
				{
					if(verbose) MOOSTrace("CuMicroModemWatchPPS: not a PPS related error\n");
				}			
			}
			
			// check if is is a message from the modem confirming disabled PPS 
			if(msg.m_sVal.find("$CACFG,SNV,0*") != string::npos) MOOSTrace("CuMicroModemWatchPPS: Modem confirms PPS sync DISABLED\n");
			
			// check if is is a message from the modem confirming enabled PPS 
			if(msg.m_sVal.find("$CACFG,SNV,1*") != string::npos) MOOSTrace("CuMicroModemWatchPPS: Modem confirms PPS sync ENABLED\n");
			
			// check if is is a message from the modem confirming disabled PPS 
			if(msg.m_sVal.find("$CACFG,TOA,0*") != string::npos) MOOSTrace("CuMicroModemWatchPPS: Modem confirms TOA report DISABLED\n");
			
			// check if is is a message from the modem confirming enabled PPS 
			if(msg.m_sVal.find("$CACFG,TOA,1*") != string::npos) MOOSTrace("CuMicroModemWatchPPS: Modem confirms TOA report ENABLED\n");
		} // if(msg.m_sKey == modem_raw)
	}

	NewMail.clear();
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// OnConnectToServer
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemWatchPPS::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet
	
	DoReadConfig();
	DoFormVarNames();
	DoRegister();
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Iterate
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemWatchPPS::Iterate()
{
	// happens AppTick times per second
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// OnStartUp
///////////////////////////////////////////////////////////////////////////////

bool CuMicroModemWatchPPS::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	timeout_counter=0;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// read .moos configuration file
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemWatchPPS::DoReadConfig()
{
	MOOSTrace("\n");
	
	if (m_MissionReader.GetConfigurationParam("verbose", sVal)) 
	{
		if(sVal=="true")
		{
			verbose=true;
		}
		else
		{
			verbose=false;
		}
		MOOSTrace("CuMicroModemWatchPPS: verbose = %s\n",sVal.c_str());
	}
	
	else
	{
		verbose=false;
		MOOSTrace("CuMicroModemWatchPPS: verbose NOT set -setting to default (false)\n");
	}
	
	if (m_MissionReader.GetConfigurationParam("modem_prefix", sVal)) 
	{
		modem_prefix = sVal;
		MOOSTrace("CuMicroModemWatchPPS: modem_prefix = %s\n",modem_prefix.c_str());
	} 
	
	else 
	{
		MOOSTrace("CuMicroModemWatchPPS: modem_prefix NOT set - setting to default (MODEM)\n");
		modem_prefix ="MODEM";
	}
	
	if (m_MissionReader.GetConfigurationParam("max_timeout", sVal)) 
	{
		max_timeout=int(atof(sVal.c_str()));
		MOOSTrace("CuMicroModemWatchPPS: max_timeout = %d\n",max_timeout);
	} 
	
	else 
	{
		MOOSTrace("CuMicroModemWatchPPS: max_timeout NOT set - setting to default (3)\n");
		max_timeout=3;
	}
}

///////////////////////////////////////////////////////////////////////////////
// form variable names using the prefixes specified in the configuration file
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemWatchPPS::DoFormVarNames()
{
	// Variable names for communication from uMicroModemAddPPS to iMicroModem
	
	// TO_MODEM
	modem_to_modem=MOOSFormat("%s_TO_MODEM",modem_prefix.c_str());
	// COMMAND
	modem_command=MOOSFormat("%s_COMMAND",modem_prefix.c_str());
	// FROM_MODEM
	modem_from_modem=MOOSFormat("%s_FROM_MODEM",modem_prefix.c_str());
	// RAW
	modem_raw=MOOSFormat("%s_RAW",modem_prefix.c_str());
}
	
///////////////////////////////////////////////////////////////////////////////
// register for variables
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemWatchPPS::DoRegister()
{

	// register for variables published by iMicroModem
	m_Comms.Register(modem_raw,0);
}

///////////////////////////////////////////////////////////////////////////////
// disable the PPS synchronization
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemWatchPPS::DisablePPS()
{
	string modem_string,check_sum;	
	
	// build command string to disable PPS sync
	modem_string="Command=CFG,Name=SNV,Value=0";
	
	// generate checksum
	//if(!GenerateNMEAchecksum(modem_string,&check_sum,verbose))
	//{
	//	if(verbose) MOOSTrace("CuMicroModemAddPPS: No checksum could be generated for %s\n",modem_string.c_str());
	//}
	
	//modem_string=modem_string+check_sum;
	
	//if(verbose) 
	//{
	//	MOOSTrace("CuMicroModemAddPPS: sending modem configuration string\n");
	//	MOOSTrace("                    %s\n",modem_string.c_str());
	//}
	
	// send configuration string
	m_Comms.Notify(modem_command,modem_string);
	
	// build command string to disable TOA report
	modem_string="Command=CFG,Name=TOA,Value=0";
	
	// generate checksum
	//if(!GenerateNMEAchecksum(modem_string,&check_sum,verbose))
	//{
	//	if(verbose) MOOSTrace("CuMicroModemAddPPS: No checksum could be generated for %s\n",modem_string.c_str());
	//}
	
	//modem_string=modem_string+check_sum;
	
	//if(verbose) 
	//{
	//	MOOSTrace("CuMicroModemAddPPS: sending modem configuration string\n");
	//	MOOSTrace("                    %s\n",modem_string.c_str());
	//}
	
	// send configuration string
	m_Comms.Notify(modem_command,modem_string);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// enable the PPS synchronization
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemWatchPPS::EnablePPS()
{
	string modem_string,check_sum;	
	
	// build command string to enable PPS sync
	modem_string="Command=CFG,Name=SNV,Value=1";
		
	// generate checksum
	//if(!GenerateNMEAchecksum(modem_string,&check_sum,verbose))
	//{
	//	if(verbose) MOOSTrace("CuMicroModemAddPPS: No checksum could be generated for %s\n",modem_string.c_str());
	//}
	
	//modem_string=modem_string+check_sum;
	
	//if(verbose) 
	//{
	//	MOOSTrace("CuMicroModemAddPPS: sending modem configuration string\n");
	//	MOOSTrace("                    %s\n",modem_string.c_str());
	//}
	
	// send configuration string
	m_Comms.Notify(modem_command,modem_string);
	
	// build command string to enable TOA report
	modem_string="Command=CFG,Name=TOA,Value=1";
	
	// generate checksum
	//if(!GenerateNMEAchecksum(modem_string,&check_sum,verbose))
	//{
	//	if(verbose) MOOSTrace("CuMicroModemAddPPS: No checksum could be generated for %s\n",modem_string.c_str());
	//}
	
	//modem_string=modem_string+check_sum;
	
	//if(verbose) 
	//{
	//	MOOSTrace("CuMicroModemAddPPS: sending modem configuration string\n");
	//	MOOSTrace("                    %s\n",modem_string.c_str());
	//}

	// send configuration string
	m_Comms.Notify(modem_command,modem_string);
	
	return true;
}
