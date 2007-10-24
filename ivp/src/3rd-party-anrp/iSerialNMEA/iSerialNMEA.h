// t. schneider tes@mit.edu 10.1.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is iSerialNMEA.h 
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

#ifndef ISERIALNMEAH
#define ISERIALNMEAH

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include <string>
#include "MBUtils.h"
#include "MOOSGeodesy.h"
#include <math.h>
#include <iostream>

class CiSerialNMEA : public CMOOSApp  
{
public:
	//standard construction and destruction
	CiSerialNMEA();
	virtual ~CiSerialNMEA();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

	bool processGPGGA(std::vector<std::string>);
	bool processGPHGT(std::vector<std::string>);
	bool processPASHR(std::vector<std::string>);

	int m_baud;
	std::string m_port_name;
	CMOOSLinuxSerialPort m_port;

	std::string m_vessel_name;
	std::string m_vessel_type;
	double m_nav_x;
	double m_nav_y;
	double m_lat;
	double m_long;
	double m_nav_speed;
	double m_nav_heading;
	double m_blackout_interval;
	double m_last_post_time;
	
	// newest timestamp available from the serial stream
	double m_best_time;

	CMOOSGeodesy m_Geodesy;

	double m_last_x;
	double m_last_y;
	double m_last_time;
	
	char m_i;

	double m_speed [];
};

#endif 
