// Class which manages a data base consisting of a list of structs 
// where each struct is a vehicle and the struct elements are various 
// parameters of the vehicle such as name,x-position, y-position, 
// depth, acomms-address, etc.
//
// Author: Alexander Bahr abahr@mit.edu
// Created: 16 July 2007
// Last modified: 16 July 2007

///////////////////////////////////////////////////////////////////////////////

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "C_vlist.h"
using namespace std;

C_vlist::C_vlist()
{
}

C_vlist::~C_vlist()
{
}

///////////////////////////////////////////////////////////////////////////////
// adds vehicle to the v_list if it doesn't exist already
//
// INPUT ARGUMENTS:
//		v_name:				name of vehicle to be added
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if vehicle did not exists already
//
//////////////////////////////////////////////////////////////////////////////

bool C_vlist::AddVehicle(std::string v_name, bool debug)
{	
	bool success_flag=false;
	
	v_element actual_vehicle;
	
	// set vehicle_name
	actual_vehicle.name=v_name;
	
	// set all other values to 0
	actual_vehicle.x=0;
	actual_vehicle.y=0;
	actual_vehicle.depth=0;
	actual_vehicle.acomms_address=0;
	
	v_list.push_back(actual_vehicle);
	
	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// removes vehicle from the list if it is in v_list
//
// INPUT ARGUMENTS:
//		v_name:				name of vehicle to be removed
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if vehicle did exist already
//
//////////////////////////////////////////////////////////////////////////////

bool C_vlist::RemoveVehicle(std::string v_name, bool debug)
{	
	bool success_flag=false;
	
	v_element actual_vehicle;
	
	// set pointer to begin of vehicle list
	v_list_pointer=v_list.begin();
	
	while((v_list_pointer!=v_list.end()) && (success_flag==false))
	{
		actual_vehicle=(*v_list_pointer);
		
		// check if the name of the vehicle in the list is the one we are looking for
		if(strcmp(actual_vehicle.name.c_str(),v_name.c_str())==0)
		{
			success_flag=true;
			
			// remove element from list
			v_list.erase(v_list_pointer);
			
			if(debug)
			{
				MOOSTrace("C_vlist.RemoveVehicle: Vehicle REMOVED\n");
				success_flag=false;
			}
		}
		v_list_pointer++;
	}
	
	if((debug) && (~success_flag))
	{
		MOOSTrace("C_vlist.RemoveVehicle: Vehicle NOT found\n");
	}

	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// check if a vehicle is already in the v_list
//
// INPUT ARGUMENTS:
//		v_name:				name of vehicle
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if vehicle exists in list FALSE if not
//
///////////////////////////////////////////////////////////////////////////////

bool C_vlist::CheckExistVehicle(std::string v_name, bool debug)
{
	bool success_flag=false;
	
	v_element actual_vehicle;
	
	// set pointer to begin of vehicle list
	v_list_pointer=v_list.begin();
	
	while((v_list_pointer!=v_list.end()) && (success_flag==false))
	{
		actual_vehicle=(*v_list_pointer);
		
		// check if the name of the vehicle in the list is the one we are looking for
		if(strcmp(actual_vehicle.name.c_str(),v_name.c_str())==0)
		{
			success_flag=true;
		}
		v_list_pointer++;
	}
	
	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// check if a list of vehicles is already in the list
//
// INPUT ARGUMENTS:
//		v_name_list:		list with name of vehicles
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if all vehicles exist in list FALSE if one or more
//							do not
//
// MODIFIED INPUT ARGUMENTS:
//		v_name_out_list:	list with strings of vehicle names which 
//							are not in the v_list
//
///////////////////////////////////////////////////////////////////////////////

bool C_vlist::CheckExistVehicle(std::list<string> v_name_list, std::list<string> *v_name_out_list, bool debug)
{
	bool success_flag=false;
	
	v_element v_list_vehicle;
	string v_name_list_vehicle;
	
	// clear list with names which are not in v_list
	(*v_name_out_list).clear();
	
	// define local iterators for v_name_list and v_name_out_list
	std::list<string>::iterator v_name_list_pointer;
	std::list<string>::iterator v_name_out_list_pointer;
	
	// set pointer to begin of v_list
	v_list_pointer=v_list.begin();
	
	// iterate trough all vehicles in v_list
	while((v_list_pointer!=v_list.end()) && (!v_name_list.empty()))
	{
		v_list_vehicle=(*v_list_pointer);
		
		// set pointer to begin of v_name_list
		v_name_list_pointer=v_name_list.begin();
		
		// iterate through all vehicles in v_name_list 
		while((v_name_list_pointer!=v_name_list.end()) && (!v_name_list.empty()))
		{
			v_name_list_vehicle=(*v_name_list_pointer);
			
			// check if the name of the vehicle in the list is the one in v_name_list which the pointer points to
			if(strcmp(v_list_vehicle.name.c_str(),v_name_list_vehicle.c_str())==0)
			{
				// if yes, remove element from v_name_list
				v_name_list.erase(v_name_list_pointer);
			}
		v_name_list_pointer++;;
		}	
		v_list_pointer++;
		
	}
	
	(*v_name_out_list)=v_name_list;
	
	if(!(*v_name_out_list).empty()) success_flag=false;
	
	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// get DOUBLE parameter from vehicle
//
// INPUT ARGUMENTS:
//		v_name:				name of vehicle
//		v_param:			name of parameter 
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if vehicle exists in list FALSE if not
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_double:	if parameter was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool C_vlist::GetVehicleParam(std::string v_name,std::string v_param, double *parameter_double, bool debug)
{	
	bool success_flag=false;
	
	v_element actual_vehicle;
	
	// set pointer to begin of vehicle list
	v_list_pointer=v_list.begin();
	
	while((v_list_pointer!=v_list.end()) && (success_flag==false))
	{
		actual_vehicle=(*v_list_pointer);
		
		// check if the name of the vehicle in the list is the one we are looking for
		if(strcmp(actual_vehicle.name.c_str(),v_name.c_str())==0)
		{
			success_flag=true;
			
			GetEntryParam(actual_vehicle,v_param, parameter_double, debug);
			
			if(debug)
			{
				MOOSTrace("C_vlist.GetVehicleParam: Vehicle FOUND\n");
				success_flag=false;
			}
		}
		v_list_pointer++;
	}
	
	if((debug) && (~success_flag))
	{
		MOOSTrace("C_vlist.GetVehicleParam: Vehicle NOT found\n");
	}

	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// get STRING parameter from vehicle
//
// INPUT ARGUMENTS:
//		v_name:				name of vehicle
//		v_param:			name of parameter 
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if vehicle exists in list FALSE if not
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_string:	if parameter was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool C_vlist::GetVehicleParam(std::string v_name,std::string v_param, string *parameter_string, bool debug)
{	
	bool success_flag=false;
	
	v_element actual_vehicle;
	
	// set pointer to begin of vehicle list
	v_list_pointer=v_list.begin();
	
	while((v_list_pointer!=v_list.end()) && (success_flag==false))
	{
		actual_vehicle=(*v_list_pointer);
		
		// check if the name of the vehicle in the list is the one we are looking for
		if(strcmp(actual_vehicle.name.c_str(),v_name.c_str())==0)
		{
			success_flag=true;
			
			GetEntryParam(actual_vehicle,v_param, parameter_string, debug);			
			
			if(debug)
			{
				MOOSTrace("C_vlist.GetVehicleParam: Vehicle FOUND\n");
				success_flag=false;
			}
		}
		v_list_pointer++;
	}
	
	if((debug) && (~success_flag))
	{
		MOOSTrace("C_vlist.GetVehicleParam: Vehicle NOT found\n");
	}

	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// set DOUBLE parameter for vehicle
//
// INPUT ARGUMENTS:
//		v_name:				name of vehicle
//		v_param:			name of parameter
//		parameter_double:	value of parameter 
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if vehicle exists in list FALSE if not
//		
//////////////////////////////////////////////////////////////////////////////

bool C_vlist::SetVehicleParam(std::string v_name,std::string v_param, double parameter_double, bool debug)
{	
	bool success_flag=false;
	
	v_element actual_vehicle;
	
	// set pointer to begin of vehicle list
	v_list_pointer=v_list.begin();
	
	while((v_list_pointer!=v_list.end()) && (success_flag==false))
	{
		actual_vehicle=(*v_list_pointer);
		
		// check if the name of the vehicle in the list is the one we are looking for
		if(strcmp(actual_vehicle.name.c_str(),v_name.c_str())==0)
		{
			success_flag=true;
			
			SetEntryParam(&actual_vehicle,v_param, parameter_double, debug);
			
			if(debug)
			{
				MOOSTrace("C_vlist.GetVehicleParam: Vehicle FOUND\n");
				success_flag=false;
			}
		}
		v_list_pointer++;
	}
	
	if((debug) && (~success_flag))
	{
		MOOSTrace("C_vlist.GetVehicleParam: Vehicle NOT found\n");
	}

	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// set STRING parameter for vehicle
//
// INPUT ARGUMENTS:
//		v_name:				name of vehicle
//		v_param:			name of parameter
//		parameter_string:	value of parameter 
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if vehicle exists in list FALSE if not
//		
//////////////////////////////////////////////////////////////////////////////

bool C_vlist::SetVehicleParam(std::string v_name,std::string v_param, std::string parameter_string, bool debug)
{	
	bool success_flag=false;
	
	v_element actual_vehicle;
	
	// set pointer to begin of vehicle list
	v_list_pointer=v_list.begin();
	
	while((v_list_pointer!=v_list.end()) && (success_flag==false))
	{
		actual_vehicle=(*v_list_pointer);
		
		// check if the name of the vehicle in the list is the one we are looking for
		if(strcmp(actual_vehicle.name.c_str(),v_name.c_str())==0)
		{
			success_flag=true;
			
			SetEntryParam(&actual_vehicle,v_param, parameter_string, debug);
			
			if(debug)
			{
				MOOSTrace("C_vlist.GetVehicleParam: Vehicle FOUND\n");
				success_flag=false;
			}
		}
		v_list_pointer++;
	}
	
	if((debug) && (~success_flag))
	{
		MOOSTrace("C_vlist.GetVehicleParam: Vehicle NOT found\n");
	}

	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// get DOUBLE parameter from a struct
//
// INPUT ARGUMENTS:
//		actual_vehicle:		struct of a single vehicle which we 
//							want to retrieve the parameter from
//		v_param:			name of parameter 
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if retrieval was successful
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_double:	if parameter was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool C_vlist::GetEntryParam(v_element actual_vehicle,std::string v_param, double *parameter_double, bool debug)
{
	bool success_flag=true;
	
	if(v_param=="x") *parameter_double=actual_vehicle.x;
	else if(v_param=="y") *parameter_double=actual_vehicle.y;
	else if(v_param=="depth") *parameter_double=actual_vehicle.depth;
	else if(v_param=="acomms_address") *parameter_double=actual_vehicle.acomms_address;
	else success_flag=false;
	
	if(debug)
	{
		if(success_flag) MOOSTrace("C_vlist.GetEntryParam: %s.%s=%lf\n",actual_vehicle.name.c_str(),v_param.c_str(),*parameter_double);
		else MOOSTrace("C_vlist.GetEntryParam: Parameter NOT found\n");
	}
}

///////////////////////////////////////////////////////////////////////////////
// get STRING parameter from a struct
//
// INPUT ARGUMENTS:
//		actual_vehicle:			struct of a single vehicle which we 
//							want to retrieve the parameter from
//		v_param:			name of parameter 
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if retrieval was successful
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_string:	if parameter was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool C_vlist::GetEntryParam(v_element actual_vehicle,std::string v_param,std::string *parameter_string, bool debug)
{
	bool success_flag=true;
		
	if(debug)
	{
		if(success_flag) MOOSTrace("C_vlist.GetEntryParam: %s.%s=%s\n",actual_vehicle.name.c_str(),v_param.c_str(),(*parameter_string).c_str());
		else MOOSTrace("C_vlist.GetEntryParam: Parameter NOT found\n");
	}
}

///////////////////////////////////////////////////////////////////////////////
// set DOUBLE parameter in struct
//
// INPUT ARGUMENTS:
//		v_param:			name of parameter
//		parameter_double:	value which the parameter is set to 
//		debug:				print debug messages if set to TRUE 
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if setting was successful
//
// MODIFIED INPUT ARGUMENTS:
//		actual_vehicle:		struct with of a single vehicle which parameter we 
//							to set the parameter in
//
///////////////////////////////////////////////////////////////////////////////

bool C_vlist::SetEntryParam(v_element *actual_vehicle,std::string v_param, double parameter_double, bool debug)
{
	bool success_flag=true;
	
	if(v_param=="x") (*actual_vehicle).x=parameter_double;
	else if(v_param=="y") (*actual_vehicle).y=parameter_double;
	else if(v_param=="depth") (*actual_vehicle).depth=parameter_double;
	else if(v_param=="acomms_address") (*actual_vehicle).acomms_address=parameter_double;
	else success_flag=false;
	
	if(debug)
	{
		if(success_flag) MOOSTrace("C_vlist.SetEntryParam: %s.%s set to %lf\n",(*actual_vehicle).name.c_str(),v_param.c_str(),parameter_double);
		else MOOSTrace("C_vlist.SetEntryParam: Parameter NOT found\n");
	}
}

///////////////////////////////////////////////////////////////////////////////
// set STRING parameter in a struct
//
// INPUT ARGUMENTS:
//		v_param:			name of parameter
//		parameter_string:	value which the parameter is set to 
//		debug:				print debug messages if set to TRUE 
//bb
// RETURN ARGUMENT:
//		success_flag:		TRUE, if setting was successful
//
// MODIFIED INPUT ARGUMENTS:
//		actual_vehicle:		struct with of a sbingle vehicle which parameter we
//							to set the parameter in
//
///////////////////////////////////////////////////////////////////////////////

bool C_vlist::SetEntryParam(v_element *actual_vehicle,std::string v_param,std::string parameter_string, bool debug)
{
	bool success_flag=true;
		
	if(debug)
	{
		if(success_flag) MOOSTrace("C_vlist.GetEntryParam: %s.%s set to %s\n",(*actual_vehicle).name.c_str(),v_param.c_str(),parameter_string.c_str());
		else MOOSTrace("C_vlist.GetEntryParam: Parameter NOT found\n");
	}
}

