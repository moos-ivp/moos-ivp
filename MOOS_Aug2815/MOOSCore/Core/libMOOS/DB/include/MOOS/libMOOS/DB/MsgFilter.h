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
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
#ifndef MSGFILTERH
#define MSGFILTERH

#include <string>
#include <map>
class CMOOSMsg;

namespace MOOS
{
class MsgFilter
{
public:
	MsgFilter();
	MsgFilter(const std::string & A, const std::string & V, double p=0.0);
	bool Matches(const CMOOSMsg & M) const;
	std::string as_string() const;
	bool operator< (const MsgFilter & F) const;
	std::string app_filter() const;
	std::string var_filter() const;
	double period() const;

protected:
	std::pair<std::string,std::string> filters_;
	double period_;
};
}
#endif
