// t. schneider tes@mit.edu 10.2.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is pVirtualTether.h 
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

#ifndef PVIRTUALTETHERH
#define PVIRTUALTETHERH

#include <string>
#include "MOOSLib.h"
#include "MBUtils.h"
#include <math.h>

class CpVirtualTether : public CMOOSApp  
{
public:
	//standard construction and destruction
	CpVirtualTether();
	virtual ~CpVirtualTether();

protected:
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
        void DoRegistrations();

	// config parameters
	std::string m_contact;
	double m_range;
	double m_original_range;
	double m_timeout;
	double m_last_report_time;
	bool m_override;
	double m_nav_x;
	double m_nav_y;
	double m_grace;
	double m_start_time;
	bool m_pos_ok;
	double m_lsquare;
	
};

#endif 
