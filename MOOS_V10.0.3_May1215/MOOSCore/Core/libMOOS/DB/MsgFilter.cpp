/**
///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of 
//   Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and 
//   the University of Oxford 2003-2013 
//   
//   email: pnewman@robots.ox.ac.uk. 
//              
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Public License
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/gpl.txt
//   distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/




#include "MOOS/libMOOS/DB/MsgFilter.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include <iostream>

namespace MOOS
{
bool MsgFilter::Matches(const CMOOSMsg & M) const
{
	return MOOSWildCmp(app_filter(),M.GetSource()) &&
			MOOSWildCmp(var_filter(),M.GetKey() );
}

std::string MsgFilter::app_filter() const
{
	return filters_.first;
}
std::string MsgFilter::var_filter() const
{
	return filters_.second;
}

std::string MsgFilter::as_string() const
{
	return var_filter()+":"+app_filter();
}

double MsgFilter::period() const
{
	return period_;
}
MsgFilter::MsgFilter()
{
	period_ = 0.0;
	filters_=std::make_pair("","");
}
MsgFilter::MsgFilter(const std::string & A, const std::string & V, double p)
{
	period_ = p;
	filters_=std::make_pair(A,V);
}

bool  MsgFilter::operator < ( const MsgFilter & F) const
{
	return filters_<F.filters_;
}

}
