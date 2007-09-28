// $Header: /home/cvsroot/project-plus/src/uMicroModemAddPPS/CuMicroModemAddPPS.h,v 1.3 2007-08-22 00:35:41 abahr Exp $
// (c) 2005 
// CuMicroModemAddPPS.h: interface for the CuMicroModemAddPPS class.
////////////////////////////////////////////////

#ifndef __CuMicroModemAddPPS_h__
#define __CuMicroModemAddPPS_h__

#include "MOOSLib.h"
#include "MOOSGeodesy.h"
#include "MU.h"
#include <stdio.h>
#include <string>
#include <map>

using namespace std;

class CuMicroModemAddPPS : public CMOOSApp
{
public:
	CuMicroModemAddPPS();
	virtual ~CuMicroModemAddPPS();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	
	bool verbose,sound_speed_from_sensor,pps_second_overflow,static_node_entry_valid;
	
	int last_transmitter,static_node_address,mobile_node_address;
	
	double nav_x,nav_y,nav_depth,parameter_double,speed_of_sound;
	
	double static_node_x_lat,static_node_y_lon,static_node_local_x,static_node_local_y,static_node_depth,dfLatOrigin,dfLongOrigin;
	
	string sVal,VarNamePrefix,AddOnNamePrefix,sound_speed_sensor;
	
	string to_modem_prefix,to_stack_prefix;
	
	string data_to_modem,command_to_modem,raw_to_modem,service_to_modem,status_to_modem;
	string status_code_to_modem,to_modem_to_modem,from_modem_to_modem;
	
	string data_to_stack,command_to_stack,raw_to_stack,service_to_stack,status_to_stack;
	string status_code_to_stack,to_modem_to_stack,from_modem_to_stack;
	
	string v_name,parameter_name;
	
	string static_node_name,static_node_format,config_param,mobile_node_name;
	
	string::size_type var_pointer;
	
	struct v_data
	{
		double x;
		double y;
		double depth;
		int acomms_address;
	};	
	
	typedef v_data v_data_type;
	
	std::map<string,v_data> v_map;
	std::map<int,string> acomms_id_map;
	
	std::list<string> var_list;
	std::list<string> parameter_list;
	
	std::list<string>::iterator parameter_list_iterator;
	
	std::stringstream ss;
	
	CMOOSGeodesy m_Geodesy;
	
	bool DoSubscribe();
	bool DoReadConfig();
	bool DoFormVarNames();
	bool DoRegister();
	
	bool BuildVehicleNameSetFromString(std::string var_summary,std::map<string,v_data_type> *v_map,bool verbose);
	bool BuildVehicleNameSetFromList(std::list<string> var_list,std::map<string,v_data_type> *v_map,bool verbose);
	bool BuildVarNameList(std::list<string> *var_list,bool verbose);
	bool ModifyCATOAString(string original_string, string *modified_string,bool verbose);
	bool ModifyARRIVALTIMEString(string original_string, string *modified_string,bool verbose);
	bool GetDistance(int acomms_id,double *distance,bool verbose);
	bool PrintVmap(std::map<string,v_data_type> v_map);
	bool CheckSOS(double speed_of_sound);

};

#endif /* __CuMicroModemAddPPS_h__ */
