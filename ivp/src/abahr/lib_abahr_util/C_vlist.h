#ifndef C_VLIST_H_
#define C_VLIST_H_

#include <stdio.h>
#include <string>
#include <iterator>
#include <list>

using namespace std;

class C_vlist
{
public:
	C_vlist();
	virtual ~C_vlist();

	bool CheckExistVehicle(std::string v_name, bool debug);
	bool CheckExistVehicle(std::list<string> v_name_list, std::list<string> *v_name_out_list, bool debug);
	
	bool AddVehicle(std::string v_name, bool debug);
	bool RemoveVehicle(std::string v_name, bool debug);

	bool GetVehicleParam(std::string v_name,std::string v_param, double *parameter_double, bool debug);
	bool GetVehicleParam(std::string v_name,std::string v_param, std::string *parameter_string, bool debug);

	bool SetVehicleParam(std::string v_name,std::string v_param, double parameter_double, bool debug);
	bool SetVehicleParam(std::string v_name,std::string v_param, std::string parameter_string, bool debug);
	
	
protected:
	
	struct v
	{
		std::string name;
		double x;
		double y;
		double depth;
		double acomms_address;
	};	
	
	typedef v v_element;
	
	std::list<v_element> v_list;
	std::list<v_element>::iterator v_list_pointer;
	
	bool GetEntryParam(v_element actual_vehicle,std::string v_param, double *parameter_double, bool debug);
	bool GetEntryParam(v_element actual_vehicle,std::string v_param, std::string *parameter_string, bool debug);
	
	bool SetEntryParam(v_element *actual_vehicle,std::string v_param, double parameter_double, bool debug);
	bool SetEntryParam(v_element *actual_vehicle,std::string v_param, std::string parameter_string, bool debug);
	
};
#endif /*C_VLIST_H_*/
