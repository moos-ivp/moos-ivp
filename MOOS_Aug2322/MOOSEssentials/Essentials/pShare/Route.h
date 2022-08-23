/*
 * Route.h
 *
 *  Created on: Sep 1, 2012
 *      Author: pnewman
 */

#ifndef ROUTE_H_
#define ROUTE_H_

/*
 *
 */

#include "MOOS/libMOOS/Utils/IPV4Address.h"
#include <string>

namespace MOOS {

class Route {
public:
	Route();
	virtual ~Route();
	bool IsActive(const double time_now);

	bool operator==(const Route & r) const;
	MOOS::IPV4Address dest_address;
	std::string dest_name;
	std::string src_name;
	bool multicast;
	std::string to_string() const;
	double frequency;
	double last_time_sent;
	double duration_of_share;
	double creation_time;
	int num_shares_completed;
	int max_shares;

};

}

#endif /* ROUTE_H_ */
