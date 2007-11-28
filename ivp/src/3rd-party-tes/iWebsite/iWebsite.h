// t. schneider tes@mit.edu {date}
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is iWebsite.h 
//
// see the readme file within this directory for information
// pertaining to usage and purpose of this script.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#ifndef IWEBSITEH
#define IWEBSITEH

#include "MOOSLib.h"
#include <string>
#include <iterator>
#include "MBUtils.h"
#include <iostream>


//for system()
#include <stdlib.h>

class CiWebsite : public CMOOSApp  
{
public:
	//standard construction and destruction
	CiWebsite();
	virtual ~CiWebsite();

protected:
	// methods
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	void DoRegistrations();
	bool ProcessFetchedFile();
	bool addMapping(std::string, std::string);
	bool scpPut(std::string);

	// configuration values
	std::string m_server_address;
	std::string m_local_cache;
	double m_wait;
	std::string m_verbosity;
	std::string m_sequence_string;
	bool m_ignore_sequence;
	std::string m_server_up_address;
	std::string m_server_up_user;

	// unixtime to hit the website again (last fetch time plus m_wait)
	double m_next_fetch;

	// unixtime or sequence number of last processed file
	double m_last_file_time;

	// last sequence number
	double m_last_seq;

	// map for echoing variables
	std::map<std::string, std::string> var_map;
};

#endif 
