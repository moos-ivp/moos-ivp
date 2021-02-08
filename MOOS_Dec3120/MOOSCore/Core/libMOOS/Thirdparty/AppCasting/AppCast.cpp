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
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCast.cpp                                          */
/*    DATE: June 3rd 2012                                        */
/*                                                               */

#include <cstdlib>
#include <sstream>
#include <iomanip>

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCast.h"

using namespace std;

//----------------------------------------------------------------
// Constructor(s)

AppCast::AppCast()
{
	m_iteration = 0;
	m_cnt_run_warnings = 0;
	m_max_events = 8;
	m_max_run_warnings = 10;
	m_max_config_warnings = 100;
}

//----------------------------------------------------------------
// Procedure: event()

void AppCast::event(string str, double timestamp)
{
	if (timestamp >= 0)
	{
		stringstream ss;
		ss << "[" << fixed << setprecision(2) << timestamp << "]: " << str;
		str = ss.str();
	}
	m_events.push_back(str);
	if (m_events.size() > m_max_events)
		m_events.pop_front();
}

//----------------------------------------------------------------
// Procedure: cfgWarning()
//      Note: Limit the number of configuration warnings, after which
//            no more are accepted. The default number is very large. 
//            The thought is that if more than a huge number of config
//            warnings are generated, this probably warrants shutting
//            things down to take a look.
//            But, we want to guard against a developer that reports
//            a new config warning repeatedly, growing the size of an 
//            appcast published to a unbounded degree.

void AppCast::cfgWarning(const string& str)
{
	if (m_config_warnings.size() > m_max_config_warnings)
		return;

	m_config_warnings.push_back(str);
}

//----------------------------------------------------------------
// Procedure: runWarning()

void AppCast::runWarning(const string& str)
{
	if (m_map_run_warnings.size() < m_max_run_warnings)
		m_map_run_warnings[str]++;
	else
		m_map_run_warnings["Other Run Warnings"]++;

	// Keep separate count of runtime warnings even though the verbatim
	// list is finite in size.
	++m_cnt_run_warnings;
}

//----------------------------------------------------------------
// Procedure: retractRunWarning()

bool AppCast::retractRunWarning(const string& str)
{
	if (m_map_run_warnings.count(str) == 0)
		return (false);

	unsigned int count = m_map_run_warnings[str];
	m_map_run_warnings.erase(str);

	// Decrement the number of run warnings.
	if (m_cnt_run_warnings >= count)
		m_cnt_run_warnings -= count;
	else
		m_cnt_run_warnings = 0;

	return (true);
}

//----------------------------------------------------------------
// Procedure: setRunWarnings()

void AppCast::setRunWarnings(const string& warning, unsigned int count)
{
	if (m_map_run_warnings.size() < m_max_run_warnings)
		m_map_run_warnings[warning] = count;
}

//----------------------------------------------------------------
// Procedure: getAppCastString
//   Example: str = "name=uProc!@#iter=123!@#node=henry!@#
//                   messages=now is the!@time for all!@good men to"

string AppCast::getAppCastString() const
{
	string osep = "!@#"; // outer separator
	string isep = "!@"; // inner separator

	stringstream ss;
	ss << "proc=" << m_proc_name << osep << "iter=" << m_iteration << osep;

	if (m_node_name != "")
		ss << "node=" << m_node_name << osep;

	ss << "iter=" << m_iteration << osep;

	// Add the messages (the free-form content of the appcast)
	ss << "messages=";
	string messages_copy = m_messages;
	while (messages_copy != "")
	{
		ss << MOOSChomp(messages_copy, "\n") << isep;
	}
	ss << osep;

	// Add the messages (the free-form content of the appcast)
	const unsigned int vsize = m_config_warnings.size();
	if (vsize != 0)
	{
		ss << "config_warnings=";
		for (unsigned int i = 0; i < vsize - 1; i++)
		{
			ss << m_config_warnings[i] << isep;
		}
		ss << m_config_warnings[vsize - 1];
	}
	ss << osep;

	// Add the events
	ss << "events_total=" << m_events.size() << osep;
	if (m_events.size() > 0)
	{
		ss << "events=";
		list<string>::const_iterator p;
		for (p = m_events.begin(); p != m_events.end(); ++p)
		{
			ss << *p;
			if (p != --m_events.end())
				ss << isep;
		}
	}
	ss << osep;

	ss << "run_warning_total=" << m_cnt_run_warnings << osep;
	if (m_map_run_warnings.size() != 0)
	{
		ss << "run_warnings=";
		map<string, unsigned int>::const_iterator p;
		for (p = m_map_run_warnings.begin(); p != m_map_run_warnings.end(); ++p)
		{
			string warning = p->first;
			unsigned int wcount = p->second;
			ss << wcount << ":" << warning;
			if (p != --m_map_run_warnings.end())
				ss << isep;
		}
	}

	return (ss.str());
}

//----------------------------------------------------------------
// Procedure: getFormattedString

string AppCast::getFormattedString(bool with_header) const
{
	stringstream ss;

	// Part 1: Header (Optional: caller may make their own)
	if (with_header)
	{
		// pFoobar alpha       ____pad____              1/2 (298)
		stringstream iterss;
		stringstream warnss;
		iterss << "(" << m_iteration << ")";
		warnss << m_config_warnings.size() << "/" << m_cnt_run_warnings;
		string iter_str = iterss.str();
		string warn_str = warnss.str();
		string title_str = m_proc_name + " " + m_node_name;

		unsigned int max_len = 62;
		unsigned int now_len = warn_str.length() + title_str.length()
				+ iter_str.length();

		string pad_str;
		if (now_len < max_len)
			pad_str = string((max_len - now_len), ' ');

		ss
				<< "=============================================================="
				<< endl;
		ss << title_str << pad_str << warn_str << iter_str << endl;
		ss
				<< "=============================================================="
				<< endl;
	}

	// Part 2: Configuration Warnings
	unsigned int cwsize = m_config_warnings.size();
	if (cwsize > 0)
	{
		ss << "Configuration Warnings: " << cwsize << endl;
		for (unsigned int j = 0; j < cwsize; j++)
		{
			ss << " [" << (j + 1) << " of " << cwsize << "]: "
					<< m_config_warnings[j] << endl;
		}
		ss << endl;
	}

	// Part 3: Run Warnings
	if (m_cnt_run_warnings > 0)
	{
		ss << "Runtime Warnings: " << m_cnt_run_warnings << endl;
		map<string, unsigned int>::const_iterator p;
		for (p = m_map_run_warnings.begin(); p != m_map_run_warnings.end(); ++p)
		{
			ss << " [" << p->second << "]: " << p->first << endl;
		}
		ss << endl;
	}

	// Part 4: Messages (The body of the output)
	ss << m_messages << endl;

	// Part 5: Events
	if (m_events.size() > 0)
	{
		ss
				<< "==================================================================="
				<< endl;
		ss << "Most Recent Events (" << m_events.size() << "):" << endl;
		ss
				<< "==================================================================="
				<< endl;
		list<string>::const_reverse_iterator q;
		for (q = m_events.rbegin(); q != m_events.rend(); ++q)
			ss << *q << endl;
	}

	return (ss.str());
}

//----------------------------------------------------------------
// Procedure: string2AppCast
//   Example: str = "name=uProc!@#iter=123!@#node=henry!@#
//                   messages=now is the!@time for all!@good men to"

AppCast string2AppCast(const std::string& str)
{
	string osep = "!@#"; // outer separator
	string isep = "!@"; // inner separator

	AppCast ac;

	// We expand the maximum number of events and warnings since we 
	// essentially want to build what is given to us. If the given 
	// appcast has 300 events, who are we to prune? The event limiting
	// should occur when the original appcast is made. Or job here is
	// just to reconstruct what is given to us.
	ac.setMaxEvents(1000);
	ac.setMaxRunWarnings(1000);

	string ac_str = str;
	while (ac_str != "")
	{
		string pair = MOOSChomp(ac_str, osep);
		string param = MOOSChomp(pair, "=");
		string value = pair;
		MOOSTrimWhiteSpace(param);
		MOOSTrimWhiteSpace(value);
		if (param == "proc")
		{
			ac.setProcName(value);
		}
		else if (param == "iter")
		{
			int ival = atoi(value.c_str());
			ac.setIteration((unsigned int) (ival));
		}
		else if (param == "node")
		{
			ac.setNodeName(value);
		}
		else if (param == "messages")
		{
			stringstream ss;
			while (value != "")
				ss << MOOSChomp(value, isep) << endl;
			ac.msg(ss.str());
		}
		else if (param == "config_warnings")
		{
			while (value != "")
			{
				string config_warning = MOOSChomp(value, isep);
				MOOSTrimWhiteSpace(config_warning);
				ac.cfgWarning(config_warning);
			}
		}
		else if (param == "events")
		{
			while (value != "")
			{
				string event = MOOSChomp(value, isep);
				MOOSTrimWhiteSpace(event);
				ac.event(event);
			}
		}
		else if (param == "run_warning_total")
		{
			ac.setRunWarningCount((unsigned int) (atoi(value.c_str())));
		}
		else if (param == "run_warnings")
		{
			while (value != "")
			{
				string full_warning = MOOSChomp(value, isep);
				MOOSTrimWhiteSpace(full_warning);
				string count = MOOSChomp(full_warning, ":");
				string warning = full_warning;
				MOOSTrimWhiteSpace(count);
				MOOSTrimWhiteSpace(warning);

				int warning_cnt = atoi(count.c_str());
				warning_cnt = (warning_cnt < 0) ? 0 : warning_cnt;
				ac.setRunWarnings(warning, (unsigned int) (warning_cnt));
			}
		}
	}

	return (ac);
}
