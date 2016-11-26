/*
 * Route.cpp
 *
 *  Created on: Sep 1, 2012
 *      Author: pnewman
 */

#include "Route.h"
#include <sstream>
namespace MOOS {

Route::Route() {
	// TODO Auto-generated constructor stub
    last_time_sent = 0.0;
    frequency   = 0.0;
}

Route::~Route() {
	// TODO Auto-generated destructor stub
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
