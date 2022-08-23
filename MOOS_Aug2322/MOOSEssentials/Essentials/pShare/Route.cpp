/*
 * Route.cpp
 *
 *  Created on: Sep 1, 2012
 *      Author: pnewman
 */

#include <sstream>
#include <iostream>

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "Route.h"

namespace MOOS {

Route::Route() {
    last_time_sent = 0.0;
    frequency   = 0.0;
	duration_of_share = -1.0; 
	max_shares = -1.0;
	num_shares_completed = 0;
	creation_time = Time();
}

Route::~Route() {
	// TODO Auto-generated destructor stub
}


bool Route::IsActive(const double time_now){

	//has the allowable share window passed?
	//if <0 then no limit has been set;

	std::cout<<"duration_of_share "<<duration_of_share<<"\n";

	if(duration_of_share >=0.0 && time_now-creation_time > duration_of_share){
		return false;
	}

	//has the maximum number of shares been exceeded?
	//if <0 then no limit has been set;
	if(max_shares >=0 && num_shares_completed>=max_shares){
		return false;
	}

	//is the immediate frequence of shares too high?
	if(frequency>0.0 && time_now-last_time_sent<(1.0/frequency)){
		return false;
	}
	return true;
}

std::string Route::to_string() const
{
	std::stringstream ss;
	ss<<"ShareInfo:\n"
			<<"add: "<<dest_address.to_string()<<std::endl
			<<"dest_name: "<<dest_name<<std::endl
			<<"src_name: "<<src_name<<std::endl
			<<"multicast: "<<multicast<<std::endl
	        <<"frequency: "<<frequency<<std::endl;


	return ss.str();
}

bool Route::operator==(const Route & r) const
{
	return r.dest_address.port()==dest_address.port() &&
			r.dest_address.host()==dest_address.host() &&
		r.dest_name == dest_name &&
		r.src_name==  src_name &&
		r.multicast == multicast;
}


}
