// modify modem data to simulate PPS synchronization and non-zero time 
// of arrival (TOA) when using modem hardware in a simulation environment
//
// Author: Alexander Bahr abahr@mit.edu
// Created: 16 July 2007
// Last modified: 

// CuMicroModemAddPPS.cpp: implementation of the CuMicroModemAddPPS class.
///////////////////////////////////////////////////////////////////////////////

#include <iterator>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>	// for itoa() call
#include <math.h>
#include "../MOOSLIB/MOOSLib.h"
#include "../MOOSGenLib/MOOSGenLib.h"
#include "CuMicroModemAddPPS.h"
#include "MU.h"

CuMicroModemAddPPS::CuMicroModemAddPPS()
{
	// constructor
}

CuMicroModemAddPPS::~CuMicroModemAddPPS()
{
	// destructor
}

///////////////////////////////////////////////////////////////////////////////
// mail handler
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	
	for(p = NewMail.begin(); p != NewMail.end(); p++) 
	{
		CMOOSMsg &msg = *p;
		
		//if(verbose) MOOSTrace("CuMicroModemAddPPS: new mail in variable %s\n",msg.m_sKey.c_str());
		
		var_pointer=msg.m_sKey.find(":");
		
		//if(verbose) MOOSTrace("CuMicroModemAddPPS: Parsing mail %s\n",msg.m_sKey.c_str());
		
		// check if this is a REMOTE variable transferred via MOOSBridge by looking for a colon (:) in the variable name
		if(var_pointer!=string::npos)
		{
			
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: %s is foreign variable\n",msg.m_sKey.c_str());
			
			// if this is a remote variable extract the vehicle name
			v_name=msg.m_sKey.substr(0,var_pointer);
			
			// capitalize node name
			v_name=MU_capitalize(v_name);
			
			// then extract the parameter_name
			parameter_name=msg.m_sKey.substr(var_pointer+1,msg.m_sKey.size()-1);
			
			// check if the vehicle is already in the list
			map<string,v_data_type>::iterator ret=v_map.find(v_name);
			
			if(ret!=v_map.end())
			{
				// update vehicle map based on vehicle name and parameter name
				if(parameter_name=="NAV_X") v_map[v_name].x=msg.m_dfVal;
				else if(parameter_name=="NAV_Y") v_map[v_name].y=msg.m_dfVal;
				else if(parameter_name=="NAV_DEPTH") v_map[v_name].depth=msg.m_dfVal;
			
				else
				{	
					if(verbose) MOOSTrace("CuMicroModemAddPPS: Parameter %s from remote vehicles is not processed\n",parameter_name.c_str());
				}
			}
			else
			{
				if(verbose) MOOSTrace("CuMicroModemAddPPS: %s is not in vehicle list\n",v_name.c_str());
			}
			
		} // end check for foreign variable
			
		// check if it is a <MODEM>_RAW string
		if(msg.m_sKey == raw_to_modem)
		{
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: %s = \n",msg.m_sKey.c_str()); 
			//if(verbose) MOOSTrace("                    %s\n",msg.m_sVal.c_str());
			
			// check if it contains a $CATOA string
			if(msg.m_sVal.find("$CATOA") != string::npos)
			{
				
				string modified_string;
				
				if(verbose) MOOSTrace("CuMicroModemAddPPS: $CATOA string detected, inserting non-zero time of flight\n");
				
				// modify string to include non-zero time of flight and rebroadcast
				ModifyCATOAString(msg.m_sVal,&modified_string,verbose);
				
				// publish modfied string
				m_Comms.Notify(raw_to_stack,modified_string);				
				
			}
			
			// check if it contains a $CADRQ string - a data request
			else if(msg.m_sVal.find("$CADRQ") != string::npos)
			{
				if(verbose) MOOSTrace("CuMicroModemAddPPS: $CADRQ data request detected\n");
				
				// get the fourth comma-seperated value
				if(MU_get_nth_from_comma_string(msg.m_sVal,4, &last_transmitter, false))
				{
					if(verbose) MOOSTrace("CuMicroModemAddPPS: last_transmitter updated to %i\n",last_transmitter);
				}
				else
				{
					MOOSTrace("CuMicroModemAddPPS: parsing ERROR: invalid CADRQ string\n");
				}

				// in any case, rebroadcast
				m_Comms.Notify(raw_to_stack,msg.m_sVal);
				
			}
			
			
			// if no $CATOA or $CADRQ string is present, simply rebroadcast
			else
			{
				m_Comms.Notify(raw_to_stack,msg.m_sVal);
				//if(verbose) MOOSTrace("CuMicroModemAddPPS: no $CATOA string detected\n");
				//if(verbose) MOOSTrace("                    rebroadcasting %s unmodified -> pRouter\n",msg.m_sKey.c_str());
			}
		}
		
		// check if it is <MODEM>_DATA string
		if(msg.m_sKey == data_to_modem)
		{
			// check if it contains Data=ARRIVALTIME
			if(msg.m_sVal.find("Data=ARRIVALTIME") != string::npos)
			{
				
				string modified_string;
				
				if(verbose) MOOSTrace("CuMicroModemAddPPS: Data=ARRIVALTIME string detected, inserting non-zero time of flight\n");
				
				// modify string to include non-zero time of flight and rebroadcast
				ModifyARRIVALTIMEString(msg.m_sVal,&modified_string,verbose);
				
				// publish modfied string
				m_Comms.Notify(data_to_stack,modified_string);				
				
			}
			
			// if no Data=ARRILVALTIME string is present, simply rebroadcast
			else
			{
				m_Comms.Notify(data_to_stack,msg.m_sVal);
				//if(verbose) MOOSTrace("CuMicroModemAddPPS: no Data=ARRILVALTIME string detected\n");
				//if(verbose) MOOSTrace("                    rebroadcasting %s unmodified -> pRouter\n",msg.m_sKey.c_str());
			}
		}
		
		if(msg.m_sKey == status_to_modem)
		{
			// check if status is "Receiving" 
			if(msg.m_sVal.find("Receiving")!=string::npos)
			{
				if(verbose) MOOSTrace("CuMicroModemAddPPS: detected mode status switch to \"Receiving\"\n");
				
				if(MU_parse_string(msg.m_sVal , "s:s=n", &parameter_list, false))
				{
					// set iterator to beginning of parameter list
					parameter_list_iterator=parameter_list.begin();
				
					// skip keywords "Receiving:" "src="
					parameter_list_iterator++;
					parameter_list_iterator++;
					
					// acomms id of transmitter
					last_transmitter=int(atof((*parameter_list_iterator).c_str()));
					
					if(verbose) 
					{
						MOOSTrace("CuMicroModemAddPPS: <modem>_STATUS: %s\n",msg.m_sVal.c_str());
						MOOSTrace("                    last_transmitter updated to %d\n",last_transmitter);
					}
				}
				else
				{
					MOOSTrace("CuMicroModemAddPPS: invalid format of <modem>_STATUS \"Receiving\"\n");
				}
			}
			
			// check if status is "Monitoring" 
			if(msg.m_sVal.find("Monitoring")!=string::npos)
			{
				if(verbose) MOOSTrace("CuMicroModemAddPPS: detected mode status switch to \"Monitoring\"\n");
				
				if(MU_parse_string(msg.m_sVal , "s:s=n,s=n", &parameter_list, false))
				{
					// set iterator to beginning of parameter list
					parameter_list_iterator=parameter_list.begin();
				
					// skip keywords "Monitoring:" and "src="
					parameter_list_iterator++;
					parameter_list_iterator++;
					
					// acomms id of transmitter
					last_transmitter=int(atof((*parameter_list_iterator).c_str()));
					
					if(verbose) 
					{
						MOOSTrace("CuMicroModemAddPPS: <modem>_STATUS: %s\n",msg.m_sVal.c_str());
						MOOSTrace("                    last_transmitter updated to %d\n",last_transmitter);
					}
				}
				else
				{
					MOOSTrace("CuMicroModemAddPPS: invalid format of <modem>_STATUS \"Monitoring\"\n");
				}
			}
			
			// check if status is "Got data" 
			if(msg.m_sVal.find("Got data")!=string::npos)
			{
				if(verbose) MOOSTrace("CuMicroModemAddPPS: detected mode status switch to \"Got data\"\n");
				
				if(MU_parse_string(msg.m_sVal , "s:s=n,s=n", &parameter_list, false))
				{
					// set iterator to beginning of parameter list
					parameter_list_iterator=parameter_list.begin();
				
					// skip "Got data: size=n,src="
					parameter_list_iterator++;
					parameter_list_iterator++;
					parameter_list_iterator++;
					parameter_list_iterator++;
					
					
					// acomms id of transmitter
					last_transmitter=int(atof((*parameter_list_iterator).c_str()));
					
					if(verbose) 
					{
						MOOSTrace("CuMicroModemAddPPS: <modem>_STATUS: %s\n",msg.m_sVal.c_str());
						MOOSTrace("                    last_transmitter updated to %d\n",last_transmitter);
					}
				}
				else
				{
					MOOSTrace("CuMicroModemAddPPS: invalid format of <modem>_STATUS \"Got data\"\n");
				}
			}
			
			// rebroadcast
			m_Comms.Notify(status_to_stack,msg.m_sVal);
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: rebroadcasting %s -> pRouter\n",msg.m_sKey.c_str());
		}
		
		// check if the variable is one that just will get rebroadcasted
		// rebroadcast down the stack to iMicroModem
		if(msg.m_sKey == command_to_stack) 
		{
			m_Comms.Notify(command_to_modem,msg.m_sVal);
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: rebroadcasting %s -> iMicroModem\n",msg.m_sKey.c_str());
		}
		if(msg.m_sKey == from_modem_to_stack)
		{
			m_Comms.Notify(from_modem_to_modem,msg.m_sVal);
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: rebroadcasting %s -> iMicroModem\n",msg.m_sKey.c_str());
		}
		
		// rebroadcast up the stack to pRouter
		if(msg.m_sKey == service_to_modem)
		{
			m_Comms.Notify(service_to_stack,msg.m_sVal);
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: rebroadcasting %s -> pRouter\n",msg.m_sKey.c_str());
		}
		if(msg.m_sKey == status_to_modem)
		{
			m_Comms.Notify(status_to_stack,msg.m_sVal);
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: rebroadcasting %s -> pRouter\n",msg.m_sKey.c_str());
		}
		if(msg.m_sKey == status_code_to_modem)
		{
			m_Comms.Notify(status_code_to_stack,msg.m_dfVal);
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: rebroadcasting %s -> pRouter\n",msg.m_sKey.c_str());
		}
		if(msg.m_sKey == to_modem_to_modem)
		{
			m_Comms.Notify(to_modem_to_stack,msg.m_sVal);
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: rebroadcasting %s -> pRouter\n",msg.m_sKey.c_str());
		}
						
		// check if it is a local NAV variable
		if(msg.m_sKey == "NAV_X")
		{ 
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: updating %s\n",msg.m_sKey.c_str());
			nav_x=msg.m_dfVal;
		}
		if(msg.m_sKey == "NAV_Y")
		{
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: updating %s\n",msg.m_sKey.c_str());
			nav_y=msg.m_dfVal;
		}
		if(msg.m_sKey == "NAV_DEPTH")
		{
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: updating %s\n",msg.m_sKey.c_str());
			nav_depth=msg.m_dfVal;
		}
		
		// check for speed of sound if it is sensor derived
		if((sound_speed_from_sensor==true) && (msg.m_sKey == sound_speed_sensor)) speed_of_sound=msg.m_dfVal;
		
	}

	NewMail.clear();
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// OnConnectToServer
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::OnConnectToServer()
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
bool CuMicroModemAddPPS::Iterate()
{
	// happens AppTick times per second
	
	// build list with all variable names in MOOSDB
	BuildVarNameList(&var_list,verbose);
	
	// extract names of foreign vehicles
	BuildVehicleNameSetFromList(var_list,&v_map,verbose);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// OnStartUp
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// read .moos configuration file
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::DoReadConfig()
{
	std::list<string> config_list;
	std::list<string>::iterator p;
	
	if (m_MissionReader.GetValue("verbose", sVal)) 
	{
		if(sVal=="true")
		{
			verbose=true;
		}
		else
		{
			verbose=false;
		}
		MOOSTrace("CuMicroModemAddPPS: verbose = %s\n",sVal.c_str());
	}
	
	else
	{
		verbose=false;
		MOOSTrace("CuMicroModemAddPPS: verbose NOT set -setting to default (false)\n");
	}
	
	if (m_MissionReader.GetValue("to_modem_prefix", sVal)) 
	{
		to_modem_prefix = sVal;
		MOOSTrace("CuMicroModemAddPPS: to_modem_prefix = %s\n",to_modem_prefix.c_str());
	} 
	
	else 
	{
		MOOSTrace("CuMicroModemAddPPS: to_modem_prefix NOT set - setting to default (TO_MODEM)\n");
		to_modem_prefix ="TO_MODEM";
	}
	
	if (m_MissionReader.GetValue("to_stack_prefix", sVal)) 
	{
		to_stack_prefix = sVal;
		MOOSTrace("CuMicroModemAddPPS: to_stack_prefix = %s\n",to_stack_prefix.c_str());
	} 
	
	else 
	{
		MOOSTrace("CuMicroModemAddPPS: to_stack_prefix NOT set - setting to default (MICROMODEM)\n");
		to_stack_prefix ="MICROMODEM";
	}
	
	if (m_MissionReader.GetValue("speed_of_sound", sVal)) 
	{
		
		// check if it is a number
		if(MU_is_number(sVal,false))
		{
			sound_speed_from_sensor=false;
			speed_of_sound=atof(sVal.c_str());
			MOOSTrace("CuMicroModemAddPPS: speed of sound is set constant to %lf m/s\n",speed_of_sound);
		}
		else
		{
			sound_speed_from_sensor=true;
			sound_speed_sensor=sVal;
			MOOSTrace("CuMicroModemAddPPS: speed of sound is derived from MOOS variable %s\n",sound_speed_sensor.c_str()); 
		}		
	}
	else 
	{
		MOOSTrace("CuMicroModemAddPPS: speed of sound not set - setting to default (1500 m/s)\n");
		sound_speed_from_sensor=false;
		speed_of_sound=1500;
	}
	
	// retrieve origin to convert absolute coordinates of fixed acomms nodes 
	if (m_MissionReader.GetValue("LatOrigin", sVal)) 
	{
		dfLatOrigin = atof(sVal.c_str());
	} 
	
	else 
	{
		MOOSTrace("CuMicroModemAddPPS: LatOrigin not set - FAIL\n");
	}

	if (m_MissionReader.GetValue("LongOrigin", sVal)) 
	{
		dfLongOrigin = atof(sVal.c_str());
	} 
	else 
	{
		MOOSTrace("CuMicroModemAddPPS: LongOrigin not set - FAIL\n");
	}

	if (!m_Geodesy.Initialise(dfLatOrigin, dfLongOrigin)) 
	{
		MOOSTrace("CuMicroModemAddPPS: Geodesy Init failed - FAIL\n");
	}
	
	// read mobile and static modem nodes
	
	// get all parameters form configuration block
	m_MissionReader.GetConfiguration(GetAppName(), config_list);
	
	for(p=config_list.begin();p!=config_list.end();p++)
	{
		config_param = *p;
		
		static_node_entry_valid=true;
		
		// look for "static" or "STATIC" keyword in line
		if((config_param.find("STATIC")!=string::npos) || (config_param.find("static")!=string::npos))
		{
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: STATIC entry found - parsing ...\n");
			
			// separate configuration parameter string for static node into list
			// the structure of a STATIC entry is as follows
			// STATIC = <name>,<acomms_address?,<coordinate_format>,[<local_X>,<latitude>],[<local_y>,<longitutde>],<depth>
			// where <coordinate_format> is either "a"/"A" indicating absolute coordinates (lat/lon) or 
			// "l"/"L" indicating coordinates in a local reference frame
			// examples:
			// STATIC = buoy1,12,l,1500,200,5
			// STATIC = buoy2,14,a,42.123,-71.348,0
			
			if(MU_parse_string(config_param , "s=s,n,s,n,n,n", &parameter_list, false))
			{
				// set iterator to beginning of parameter list
				parameter_list_iterator=parameter_list.begin();
				
				// skip keyword (STATIC)
				parameter_list_iterator++;
				
				// extract name
				static_node_name=*parameter_list_iterator;
				parameter_list_iterator++;
				
				// extract acomms address
				static_node_address=int(atof((*parameter_list_iterator).c_str()));
				parameter_list_iterator++;
				
				// extract coordinate format
				static_node_format=*parameter_list_iterator;
				parameter_list_iterator++;
				
				// extract x coordinate (in absolut or local coordinates)
				static_node_x_lat=atof((*parameter_list_iterator).c_str());
				parameter_list_iterator++;
				
				// extract y coordinate (in absolut or local coordinates)
				static_node_y_lon=atof((*parameter_list_iterator).c_str());
				parameter_list_iterator++;
				
				// extract depth
				static_node_depth=atof((*parameter_list_iterator).c_str());
				
				// convert absolute coordinates to local coordinates if necessary
				if((static_node_format=="a") || (static_node_format=="A"))
				{
					if (m_Geodesy.LatLong2LocalGrid(static_node_x_lat, static_node_y_lon, static_node_local_y, static_node_local_x)) 
					{
					
					}
					else
					{
						MOOSTrace("CuMicroModemAddPPS: geodesy conversion ERROR\n");
					}
				}
				// no conversion to absolute coordinates is necessary
				else if((static_node_format=="l") || (static_node_format=="L"))
				{
					static_node_local_x=static_node_x_lat;
					static_node_local_y=static_node_y_lon;
				}
				else
				{
					MOOSTrace("CuMicroModemAddPPS: static entry parsing ERROR:\n");
					MOOSTrace("CuMicroModemAddPPS: %s is not a a valid coordinate system descriptor (must be \"a\" or \"l\")\n");
					static_node_entry_valid=false;
				}
				
				// add node to maps
				if(static_node_entry_valid)
				{
					
					// capitalize node name
					static_node_name=MU_capitalize(static_node_name);
					
					MOOSTrace("CuMicroModemAddPPS: adding new static acomms node:\n");
					MOOSTrace("                    name=%s ; address=%d\n",static_node_name.c_str(),static_node_address);
					MOOSTrace("                    x=%lf ; y=%lf ; depth=%lf\n",static_node_local_x,static_node_local_y,static_node_depth);
					
					acomms_id_map[static_node_address]=static_node_name;
					v_map[static_node_name].x=static_node_local_x;
					v_map[static_node_name].y=static_node_local_y;
					v_map[static_node_name].depth=static_node_depth;
					v_map[static_node_name].acomms_address=static_node_address;
				}
				
			}
			else
			{
				MOOSTrace("CuMicroModemAddPPS: parsing ERROR: static entry does not comply with format\n");
			}
			
		}	// look for "static" or "STATIC" keyword in line
		
		if((config_param.find("MOBILE")!=string::npos) || (config_param.find("mobile")!=string::npos))
		{
			// if(verbose) MOOSTrace("CuMicroModemAddPPS: MOBILE entry found - parsing ...\n");
				
			// separate configuration parameter string for mobile node into list
			// the structure of a MOBILE entry is as follows
			// MOBILE = <name>,<acomms_address>
			
			// MOBILE = macrura,3
			
			if(MU_parse_string(config_param , "s=s,n", &parameter_list, false))
			{
				// set iterator to beginning of parameter list
				parameter_list_iterator=parameter_list.begin();
				
				// skip keyword (MOBILE)
				parameter_list_iterator++;
				
				// extract name
				mobile_node_name=*parameter_list_iterator;
				parameter_list_iterator++;
				
				// capitalize node name
				mobile_node_name=MU_capitalize(mobile_node_name);
				
				// extract acomms address
				mobile_node_address=int(atof((*parameter_list_iterator).c_str()));
				parameter_list_iterator++;
				
				MOOSTrace("CuMicroModemAddPPS: adding new mobile acomms node:\n");
				MOOSTrace("                    name=%s ; address=%d\n",mobile_node_name.c_str(),mobile_node_address);
					
				acomms_id_map[mobile_node_address]=mobile_node_name;
				v_map[mobile_node_name].acomms_address=mobile_node_address;
				
				// register for variables of mobile node
				m_Comms.Register(mobile_node_name+":"+"NAV_X",0);
				m_Comms.Register(mobile_node_name+":"+"NAV_Y",0);
				m_Comms.Register(mobile_node_name+":"+"NAV_DEPTH",0);
				
			}
			else
			{
				MOOSTrace("CuMicroModemAddPPS: parsing ERROR: mobile entry does not comply with format\n");
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// form variable names using the prefixes specified in the configuration file
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::DoFormVarNames()
{
	// Variable names for communication from uMicroModemAddPPS to iMicroModem
	
	// DATA
	data_to_modem=MOOSFormat("%s_DATA",to_modem_prefix.c_str());
	// COMMAND
	command_to_modem=MOOSFormat("%s_COMMAND",to_modem_prefix.c_str());
	// RAW
	raw_to_modem=MOOSFormat("%s_RAW",to_modem_prefix.c_str());
	// SERVICE
	service_to_modem=MOOSFormat("%s_SERVICE",to_modem_prefix.c_str());
	// STATUS TEXT
	status_to_modem=MOOSFormat("%s_STATUS",to_modem_prefix.c_str());
	// STATUS CODE
	status_code_to_modem=MOOSFormat("%s_STATUS_CODE",to_modem_prefix.c_str());
	// TO_MODEM
	to_modem_to_modem=MOOSFormat("%s_TO_MODEM",to_modem_prefix.c_str());
	// FROM_MODEM
	from_modem_to_modem=MOOSFormat("%s_FROM_MODEM",to_modem_prefix.c_str());
	
	// Variable names for communication from uMicroModemAddPPS to pRouter
	
	// DATA
	data_to_stack=MOOSFormat("%s_DATA",to_stack_prefix.c_str());
	// COMMAND
	command_to_stack=MOOSFormat("%s_COMMAND",to_stack_prefix.c_str());
	// RAW
	raw_to_stack=MOOSFormat("%s_RAW",to_stack_prefix.c_str());
	// SERVICE
	service_to_stack=MOOSFormat("%s_SERVICE",to_stack_prefix.c_str());
	// STATUS TEXT
	status_to_stack=MOOSFormat("%s_STATUS",to_stack_prefix.c_str());
	// STATUS CODE
	status_code_to_stack=MOOSFormat("%s_STATUS_CODE",to_stack_prefix.c_str());
	// TO_MODEM
	to_modem_to_stack=MOOSFormat("%s_TO_MODEM",to_stack_prefix.c_str());
	// FROM_MODEM
	from_modem_to_stack=MOOSFormat("%s_FROM_MODEM",to_stack_prefix.c_str());
	
	return true;
}
	
	
///////////////////////////////////////////////////////////////////////////////
// register for variables
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::DoRegister()
{

	// register for variables published by iMicroModem
	m_Comms.Register(data_to_modem,0);
	m_Comms.Register(raw_to_modem,0);
	m_Comms.Register(service_to_modem,0);
	m_Comms.Register(status_to_modem,0);
	m_Comms.Register(status_code_to_modem,0);
	m_Comms.Register(to_modem_to_modem,0);
	
	// register for variables published by pRouter
	m_Comms.Register(command_to_stack,0);
	m_Comms.Register(from_modem_to_stack,0);
	
	// register for sound speed sensor if speed of sound is not fixed to a constant value
	if(sound_speed_from_sensor) m_Comms.Register(sound_speed_sensor,0);
	
	// register for local nav variables
	m_Comms.Register("NAV_X",0);
	m_Comms.Register("NAV_Y",0);
	m_Comms.Register("NAV_DEPTH",0);
	
	// register for list of all variables
	m_Comms.Register("VAR_SUMMARY",0);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// build a list of all variable names in MOOSDB
//
// INPUT ARGUMENTS:
//		verbose:			print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if there is a least one variable
//		
//
// MODIFIED INPUT ARGUMENTS:
//		var_list:			list of variables 
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::BuildVarNameList(std::list<string> *var_list,bool verbose)
{
    bool success_flag=false;
    
    MOOSMSG_LIST::reverse_iterator p;
    MOOSMSG_LIST InMail;
    
    // clear list first
    (*var_list).clear();
    
    // get all new mail, but do not clear it
    //if(verbose)MOOSTrace("CuMicroModemAddPPS: requesting all variables\n");   
    if(m_Comms.ServerRequest("ALL",InMail,2,false)) 
    {
      success_flag=true;
      
    	for(p = InMail.rbegin(); p != InMail.rend(); p++) 
		{
			CMOOSMsg &msg = *p;
			(*var_list).push_back(msg.m_sKey);
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: %s variable added to list of all variables\n",msg.m_sKey.c_str());
		}
    }
    
    // clear list added after Andrew's recommendation
    InMail.clear();
  	
  	return success_flag;
  
}

///////////////////////////////////////////////////////////////////////////////
// build a set of vehicle names which are sending variables via MOOSBridge 
// and subsrcibe to <vehicle_name>:NAV_X,<vehicle_name>:NAV_Y,
// <vehicle_name>:NAV_DEPTH and <vehicle_name>:MODEM_RAW 
//
// INPUT ARGUMENTS:
//		var_list:			list with the name of all variables published in MOOSDB 
//		verbose:			print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if at comma_string is not empty, i.e. at least one
//							value is in the list
//
// MODIFIED INPUT ARGUMENTS:
//		v_map:				map of vehicle with pairs <v_name,v_data>
//							where v_name is the name of the vehicle and v_data
//							is a struct containing the various parameters 
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::BuildVehicleNameSetFromList(std::list<string> var_list,std::map<string,v_data_type> *v_map,bool verbose)
{
	string v_name;
	
	string::size_type sep_position; 
	
	std::list<string>::iterator var_pointer;
	
	v_data_type v_data;
	 	
	// initialize struct
	v_data.x=0;
	v_data.y=0;
	v_data.depth=0;
	v_data.acomms_address=0;
	
	//set var_pointer to first element in list
	
	// iterate through list of all var_names
	for(var_pointer=var_list.begin();var_pointer!=var_list.end();var_pointer++)
	{
		
		// check if a variable name contains a colon (:) and retain position
		sep_position=(*var_pointer).find(":");
		
		if(sep_position!=string::npos)
		{
			// now extract the vehicle name
			v_name=(*var_pointer).substr(0,sep_position);
			
			// capitalize node name
			v_name=MU_capitalize(v_name);
			
			//if(verbose) MOOSTrace("CuMicroModemAddPPS: %s is foreign vehicle\n",v_name.c_str());
			
			// add new v_name to map; the map container makes sure that only new names are added
			// insert the name and the dummy v_data struct
			// store the return value pair to check if a name was inserted
			//pair<map<string,v_data_type>::iterator, bool> ret=*v_map.insert(pair<std::string,v_data_type>(v_name,v_data));
			pair<map<string,v_data_type>::iterator, bool> ret=(*v_map).insert(make_pair(v_name,v_data));
			
			// check if an element was inserted
			if(ret.second)
			{
				// if yes, register for its parameters
				m_Comms.Register(v_name+":"+"NAV_X",0);
				m_Comms.Register(v_name+":"+"NAV_Y",0);
				m_Comms.Register(v_name+":"+"NAV_DEPTH",0);
				//m_Comms.Register(v_name+":"+to_stack_prefix+"_TO_MODEM",0);
				
				
				if(verbose) 
				{
					MOOSTrace("CuMicroModemAddPPS: %s was added to map of vehicles\n",v_name.c_str());
					PrintVmap(*v_map);
					MOOSTrace("                    ME: x-pos ; y-pos ; depth \n");
					MOOSTrace("                        %4.1lf ; %4.1lf; %4.1lf\n",nav_x,nav_y,nav_depth);
				} 
			}
			
			else
			{
				//if(verbose) MOOSTrace("CuMicroModemAddPPS: %s is in map of vehicles already, NOT added\n",v_name.c_str());
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// plot the vehicle map with all its data for debug purposes 
//
// INPUT ARGUMENTS:
//		v_map				vehicle map
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::PrintVmap(std::map<string,v_data_type> v_map)
{
	map<string,v_data_type>::const_iterator map_it=v_map.begin();
	
	MOOSTrace("CuMicroModemAddPPS: v_map:\n");
	MOOSTrace("                    Name: x-pos ; y-pos ; depth ; acomms-address\n"); 
	
	while(map_it != v_map.end())
	{
		MOOSTrace("                    %s: %f ; %f ; %f ; %d\n",map_it->first.c_str(),map_it->second.x,map_it->second.y,map_it->second.depth,map_it->second.acomms_address);
		++map_it;
	}
	
	return true;
}	

///////////////////////////////////////////////////////////////////////////////
// check if the speed of sound is valid, i.e.  1300 m/s < v < 1700 m/s 
//
// INPUT ARGUMENTS:
//		speed_of_sound:		speed_of_sound which is to be checked
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if it is within 1300 m/s and 1700 m/s, FALSE
//							if not
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::CheckSOS(double speed_of_sound)
{
	if((speed_of_sound > 1300) && (speed_of_sound < 1700)) return true;
	else 
	{
		MOOSTrace("CuMicroModemAddPPS: invalid speed of sound %lf\n",speed_of_sound);
		return false;
	}
}


///////////////////////////////////////////////////////////////////////////////
// modify the sub-second time to reflect the time of flight 
//
// INPUT ARGUMENTS:
//		original_string		the orignal modem string $CATOA... which has 0000 
//							in its sub-second argument
//		verbose:			print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if there is a least one variable
//		
// MODIFIED INPUT ARGUMENTS:
//		modified_string		modified modem string $CATOA... which has time-of-flight
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::ModifyCATOAString(string original_string, string *modified_string,bool verbose)
{
	bool success_flag=false;
	
	int acomms_id;
	
	double distance,time_of_flight,trunc_time_of_flight;
	
	string::size_type period_position;
	
	std::string time_string,check_sum;
	
	char time_ascii[80];
	
	if(verbose) MOOSTrace("CuMicroModemAddPPS: original string: %s\n",original_string.c_str());
	
	// check if is indeed a $CATOA string
	if(original_string.find("$CATOA")!=string::npos)
	{
		// find period (.) delimiter starting after $CATOA,
		period_position=original_string.find(".",6);
		
		// check if there is a period indeed
		if(period_position!=string::npos)
		{
			// build new string
							
			// get the acomms_id of the last vehicle which transmitted
			acomms_id=last_transmitter;
			
			if(GetDistance(acomms_id, &distance,verbose))
			{				
				
				if(verbose) 
				{
					MOOSTrace("                    ME: x-pos ; y-pos ; depth \n");
					MOOSTrace("                        %4.1lf ; %4.1lf; %4.1lf\n",nav_x,nav_y,nav_depth);
				} 
				
				if(verbose) MOOSTrace("CuMicroModemAddPPS: distance=%f m; speed_of_sound=%f m/s\n",distance,speed_of_sound);
				
				if(CheckSOS(speed_of_sound));
				{
					// compute time_of_flight
					time_of_flight=distance/speed_of_sound;
					
					// remove full seconds
					trunc_time_of_flight=time_of_flight-int(time_of_flight);
					
					if(time_of_flight!=trunc_time_of_flight)
					{
						pps_second_overflow=true;
						if(verbose) MOOSTrace("CuMicroModemAddPPS: time of flight %1.4lfs>1s -> PPS overflow\n",time_of_flight);
					}
					else
					{
						pps_second_overflow=false;
						if(verbose) MOOSTrace("CuMicroModemAddPPS: time of flight %1.4lfs<1s -> PPS OK\n",time_of_flight);
					}
										
					//if(verbose) MOOSTrace("CuMicroModemAddPPS: time=%f ; truncated time=%4.4f\n",time_of_flight,trunc_time_of_flight);
					
					// DO NOT USE sprintf as it may cause buffer overflow
					// generate subsecond part of time string
					snprintf(time_ascii, sizeof (time_ascii), "%4.4f",trunc_time_of_flight);
					
					// convert character array to string
					time_string=string(time_ascii);
					
					// remove 0.
					time_string=time_string.substr(1,5);
					if(verbose) MOOSTrace("CuMicroModemAddPPS: Time as ASCII: %s ; Time string: %s\n",time_ascii,time_string.c_str());
									
					// build the entire new string except for the checksum
					(*modified_string)=original_string.substr(0,period_position)+time_string+",3*";
					
					//if(verbose) MOOSTrace("CuMicroModemAddPPS: Modified string: %s\n",(*modified_string).c_str()); 
					
					// generate checksum
					if(!GenerateNMEAchecksum(*modified_string,&check_sum,false))
					{
						if(verbose) MOOSTrace("CuMicroModemAddPPS: No checksum could be generated for %s\n",(*modified_string).c_str());
					}
					
					//if(verbose) MOOSTrace("CuMicroModemAddPPS: Checksum (hex): %s\n",check_sum.c_str());
					
					// add checksum to string
					(*modified_string)=(*modified_string)+check_sum;
					if(verbose) MOOSTrace("CuMicroModemAddPPS: Modified string with checksum: %s\n",(*modified_string).c_str());
				}
							
			}
			else
			{
				//if(verbose) MOOSTrace("CuMicroModemAddPPS: Vehicle with acomms_id %d not in acomms_id_map\n",acomms_id);
			}
		}
	}
	else
	{
		if(verbose) MOOSTrace("CuMicroModemAddPPS: %s does not contain $CATOA sequence\n",original_string.c_str());
	}		
}

///////////////////////////////////////////////////////////////////////////////
// modify the sub-second time to reflect the time of flight 
//
// INPUT ARGUMENTS:
//		original_string		the orignal modem string Data=ARRIVALTME... which has 0000 
//							in its sub-second argument
//		verbose:			print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if there is a least one variable
//		
// MODIFIED INPUT ARGUMENTS:
//		modified_string		modified modem string Data=ARRIVALTIME... which has time-of-flight
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::ModifyARRIVALTIMEString(string original_string, string *modified_string,bool verbose)
{
	bool success_flag=false;
	
	int acomms_id,sync_mode;
	
	double distance,time_of_flight,trunc_time_of_flight,receive_time;
	
	string::size_type period_position;
	
	std::string time_string,sync_mode_string,check_sum;
	
	std:: stringstream ss;
	
	char time_ascii[80];
	
	if(verbose) MOOSTrace("CuMicroModemAddPPS: original string: %s\n",original_string.c_str());
	
	// check if is indeed a Data=ARRIVALTIME string
	if(original_string.find("Data=ARRIVALTIME")!=string::npos)
	{
		
		// find period (.) delimiter starting after Data=ARRIVALTIME
		period_position=original_string.find(".",10);
		
		// check if there is a period indeed
		if(period_position!=string::npos)
		{
		
			MU_parse_string(original_string , "s=s,s=n,s=n,s=n", &parameter_list, false);
			
			// set iterator to beginning of parameter list
			parameter_list_iterator=parameter_list.begin();
			
			// skip keyword (Data)
			parameter_list_iterator++;
			
			// skip data type (ARRIVALTIME)
			parameter_list_iterator++;
			
			// skip keyword (Src)
			parameter_list_iterator++;
			
			// extract acomms address
			acomms_id=int(atof((*parameter_list_iterator).c_str()));
			parameter_list_iterator++;
			
			// skip keyword (Time)
			parameter_list_iterator++;
			
			// extract receive time
			receive_time=atof((*parameter_list_iterator).c_str());
			parameter_list_iterator++;
			
			// skip keyword (Mode)
			parameter_list_iterator++;
			
			// extract synchronization mode
			sync_mode=int(atof((*parameter_list_iterator).c_str()));
			parameter_list_iterator++;
				
			if(GetDistance(acomms_id, &distance,verbose))
			{				
				
				if(verbose) 
				{
					MOOSTrace("                    ME: x-pos ; y-pos ; depth \n");
					MOOSTrace("                        %4.1lf ; %4.1lf; %4.1lf\n",nav_x,nav_y,nav_depth);
				} 
				
				if(verbose) MOOSTrace("CuMicroModemAddPPS: distance=%f m; speed_of_sound=%f m/s\n",distance,speed_of_sound);
				
				if(CheckSOS(speed_of_sound));
				{
					// compute time_of_flight
					time_of_flight=distance/speed_of_sound;
					
					// remove full seconds
					trunc_time_of_flight=time_of_flight-int(time_of_flight);
					
					if(time_of_flight!=trunc_time_of_flight)
					{
						pps_second_overflow=true;
						if(verbose) MOOSTrace("CuMicroModemAddPPS: time of flight %1.4lfs>1s -> PPS overflow\n",time_of_flight);
					}
					else
					{
						pps_second_overflow=false;
						if(verbose) MOOSTrace("CuMicroModemAddPPS: time of flight %1.4lfs<1s -> PPS OK\n",time_of_flight);
					}
										
					//if(verbose) MOOSTrace("CuMicroModemAddPPS: time=%f ; truncated time=%4.4f\n",time_of_flight,trunc_time_of_flight);
					
					// DO NOT USE sprintf as it may cuase buffer overflow
					// generate subsecond part of time string
					snprintf(time_ascii, sizeof (time_ascii), "%4.4f",trunc_time_of_flight);
					
					// convert character array to string
					time_string=string(time_ascii);
					
					// remove 0.
					time_string=time_string.substr(1,5);
					//if(verbose) MOOSTrace("CuMicroModemAddPPS: Time as ASCII: %s ; Time string: %s\n",time_ascii,time_string.c_str());
					
					// convert mode to string
					ss << sync_mode;
					ss >> sync_mode_string;
									
					// build the entire new string except for the checksum
					(*modified_string)=original_string.substr(0,period_position)+time_string+",Mode="+sync_mode_string;
					
					if(verbose) MOOSTrace("CuMicroModemAddPPS: Modified string: %s\n",(*modified_string).c_str());
				}
			
			}
			else
			{
				if(verbose) MOOSTrace("CuMicroModemAddPPS: Vehicle with acomms_id %d not in acomms_id_map\n",acomms_id);
				
				// resend unmodified string 
				*modified_string=original_string;
				if(verbose) MOOSTrace("CuMicroModemAddPPS: UNmodified string: %s\n",(*modified_string).c_str());
			}
		}
		else
		{
			if(verbose) MOOSTrace("CuMicroModemAddPPS: no . detected in Data=ARRIVALTIME string\n");
		}
	}
	else
	{
		if(verbose) MOOSTrace("CuMicroModemAddPPS: %s does not contain Data=ARRIVALTIME sequence\n",original_string.c_str());
	}		
}


///////////////////////////////////////////////////////////////////////////////
// compute the distance between this vehicle and the other one (with acomms_id)
//
// INPUT ARGUMENTS:
//		acomms_id:			modem address of other vehicle which to compute teh distance to
//		verbose:			print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if acomms_id is in acomms_id_map
//
// MODIFIED INPUT ARGUMENTS:
//		distance:			distance to other vehicle 
//
///////////////////////////////////////////////////////////////////////////////
bool CuMicroModemAddPPS::GetDistance(int acomms_id,double *distance,bool verbose)
{
	bool success_flag=false;
	
	double other_x,other_y,other_depth;
	
	string vehicle_name;
	
	// check if acomms id exists in map
	if(acomms_id_map.find(acomms_id)!=acomms_id_map.end())
	{
	
		if(verbose) PrintVmap(v_map);
		
		// get the vehicle name from acomm_id
		vehicle_name=acomms_id_map[acomms_id];
	
		// get coordinates of other vehicle
		other_x=v_map[vehicle_name].x;
		other_y=v_map[vehicle_name].y;
		other_depth=v_map[vehicle_name].depth;
		
		// compute distance to other vehicle
		(*distance)=fabs(sqrt((pow((other_x-nav_x),2)+pow((other_y-nav_y),2))+pow((other_depth-nav_depth),2)));
		
		if(verbose) MOOSTrace("CuMicroModemAddPPS: 3D-distance to %s is %f m\n",vehicle_name.c_str(),(*distance));
		
		success_flag=true;
	}
	else
	{
		if(verbose) MOOSTrace("CuMicroModemAddPPS: Vehicle with acomms_id %d not in acomms_id_map\n",acomms_id);
	}
	
	return success_flag;
}


///////////////////////////////////////////////////////////////////////////////
// build a set of vehicle names which are sending variables via MOOSBridge 
// and subsrcibe to <vehicle_name>:NAV_X,<vehicle_name>:NAV_Y,
// <vehicle_name>:NAV_DEPTH and <vehicle_name>:MODEM_RAW 
//
// INPUT ARGUMENTS:
//		var_summary:		comma-separated string string with the name of all
//							variables published in MOOSDB 
//		verbose:			print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if at comma_string is not empty, i.e. at least one
//							value is in the list
//
// MODIFIED INPUT ARGUMENTS:
//		v_map:				map of vehicle with pairs <v_name,v_data>
//							where v_name is the name of the vehicle and v_data
//							is a struct containing the various parameters 
///////////////////////////////////////////////////////////////////////////////
//bool CuMicroModemAddPPS::BuildVehicleNameSetFromString(std::string var_summary,std::map<string,v_data_type> *v_map,bool verbose)
//{
//	string v_name;
//	
//	string::size_type var_summary_pointer=0;
//	string::size_type v_name_start=0;
//	string::size_type v_name_end=0;
//	
//	v_data_type v_data;
//	 	
//	// initialize struct
//	v_data.x=0;
//	v_data.y=0;
//	v_data.depth=0;
//	v_data.acomms_address=0;
//	
//	// iterate through var_summary and stop at every colon (:)
//	while(var_summary_pointer!=string::npos)
//	{
//		var_summary_pointer=var_summary.find(":",var_summary_pointer);
//		
//		// check if a variable with a colon (:) inside was found
//		if(var_summary_pointer!=string::npos)
//		{
//			// if yes, mark the position left of the colon (:) as the end of the var_name
//			v_name_end=var_summary_pointer-1;
//			
//			// now search backward until a comma (,) or the beginning of the string is detected
//			v_name_start=var_summary.rfind(",",var_summary_pointer);
//			
//			// if no comma (,) was encountered set beginning of v_name to beginning of var_summary
//			if(v_name_start==string::npos) v_name_start=0;
//			
//			// now extract var_name out of var_summary
//			v_name=var_summary.substr(v_name_start,v_name_end);
//			
//			// add new v_name to map; the map container makes sure that only new names are added
//			// insert the name and the dummy v_data struct
//			// store the return value pair to check if a name was inserted
//			//pair<map<string,v_data_type>::iterator, bool> ret=*v_map.insert(pair<std::string,v_data_type>(v_name,v_data));
//			pair<map<string,v_data_type>::iterator, bool> ret=(*v_map).insert(make_pair(v_name,v_data));
//			
//			// check if an element was inserted
//			if(ret.second)
//			{
//				// if yes, register for its parameters
//				m_Comms.Register(v_name+":"+to_stack_prefix+"_NAV_X",0);
//				m_Comms.Register(v_name+":"+to_stack_prefix+"_NAV_Y",0);
//				m_Comms.Register(v_name+":"+to_stack_prefix+"_NAV_DEPTH",0);
//				m_Comms.Register(v_name+":"+to_stack_prefix+"_RAW",0);
//				
//				if(verbose) MOOSTrace("CuMicroModemAddPPS: %s was added to map of vehicles\n",v_name.c_str());
//			}
//			
//			else
//			{
//				if(verbose) MOOSTrace("CuMicroModemAddPPS: %s is in map of vehicles already, NOT added\n",v_name.c_str());
//			}
//		}
//		++var_summary_pointer;
//	}
//}
