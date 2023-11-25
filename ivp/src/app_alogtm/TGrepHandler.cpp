/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TGrepHandler.cpp                                     */
/*    DATE: October 5th, 2020                                    */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "MBUtils.h"
#include "TGrepHandler.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Constructor

TGrepHandler::TGrepHandler()
{
  // Init config vars
  m_file_in = 0;
  m_min_gap = 0;
  m_verbose = false;
  m_test_format = false;
  m_max_tdelta = 0;
  m_max_vdelta = 0;
  m_sig_digits = 5;
  
  // Init state vars
  m_mark_time = 0;
  m_mark_made = false;
}

//--------------------------------------------------------
// Procedure: setLogFile()

bool TGrepHandler::setLogFile(string alogfile)
{
  m_file_in = fopen(alogfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input not found or unable to open - exiting" << endl;
    return(false);
  }

  if(m_verbose)
    cout << "Ok input file: " << alogfile << endl;
  return(true);
}

//--------------------------------------------------------
// Procedure: setTimeMark()

bool TGrepHandler::setTimeMark(string str)
{
  m_mark_var = biteStringX(str, '=');
  m_mark_val = str;

  if((m_mark_var == "") && (m_mark_val == ""))
    return(false);

  return(true);
}


//--------------------------------------------------------
// Procedure: setMinGap()

bool TGrepHandler::setMinGap(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval < 0)
    return(false);

  m_min_gap = dval;
  return(true);
}


//--------------------------------------------------------
// Procedure: setMaxTDelta()

bool TGrepHandler::setMaxTDelta(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval < 0)
    return(false);

  m_max_tdelta = dval;
  return(true);
}


//--------------------------------------------------------
// Procedure: setMaxVDelta()

bool TGrepHandler::setMaxVDelta(string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval < 0)
    return(false);

  m_max_vdelta = dval;
  return(true);
}

//--------------------------------------------------------
// Procedure: setSigDigits()

bool TGrepHandler::setSigDigits(string str)
{
  if(!isNumber(str))
    return(false);

  int int_sig_digits = atoi(str.c_str());
  if(int_sig_digits < 0)
    return(false);

  m_sig_digits = (unsigned int)(int_sig_digits);
  return(true);
}


//--------------------------------------------------------
// Procedure: addKey()

bool TGrepHandler::addKey(string str)
{
  if(m_keys.count(str))
    return(false);

  m_keys.insert(str);
  return(true);
}


//--------------------------------------------------------
// Procedure: handle()

bool TGrepHandler::handle()
{
  if(m_verbose) {
    cout << "Starting handle: " << endl;
    cout << "Mark_var:" << m_mark_var << endl;
    cout << "Mark_val:" << m_mark_val << endl;
  }

  // Sanity checks
  if(!m_file_in)
    return(false);
  if((m_mark_var == "") || (m_mark_val == ""))
    return(false);

  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(m_file_in);
    
    // Part 1: Ignore comment lines
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    // Part 2: Check for end of file
    if(line_raw == "eof") 
      break;
    // Part 3: Ignore lines that do not begin with a number
    if(!isNumber(line_raw.substr(0,1)))
      continue;

    // Part 4: If there is a condition, see if it has been met
    string varname = getVarName(line_raw);
    string varval  = stripBlankEnds(getDataEntry(line_raw));

    
    if(m_verbose)
      cout << "VarName:[" << varname << "]  VarVal:[" << varval << "]" << endl;
    
    if(!m_mark_made) {
      if((varname == m_mark_var) && (varval == m_mark_val)) {
	string tstamp = getTimeStamp(line_raw);
	m_mark_time = atof(tstamp.c_str());
	m_mark_made = true;
      }
      continue;
    }

    if(m_verbose)
      cout << "." << flush;
    
    if(m_keys.count(varname)) {
      shiftTimeStamp(line_raw, m_mark_time);
      string tstamp_str = getTimeStamp(line_raw);
      double tstamp_dbl = atof(tstamp_str.c_str());
      
      string full_post = varname + varval + tstamp_str;
      if(m_unique_posts.count(full_post) == 0) {

	bool ok_gap = true;
	if(m_latest_rep.count(varname)) {
	  double gap = tstamp_dbl - m_latest_rep[varname];
	  if(gap < m_min_gap)
	    ok_gap = false;
	}

	if(ok_gap) {	
	  if(m_test_format) {
	    if(isNumber(varval)) {
	      double dval = atof(varval.c_str());
	      varval = doubleToString(dval, m_sig_digits);
	    }

	    cout << "vcheck = ";
	    cout << "var=" << varname;
	    cout << ", val=" << varval;
	    cout << ", time=" << tstamp_str;
	    if(m_max_tdelta > 0)
	      cout << ", max_tdelta=" << doubleToStringX(m_max_tdelta,2);
	    if(m_max_vdelta > 0)
	      cout << ", max_vdelta=" << doubleToStringX(m_max_vdelta,2);
	    cout << endl;
	  }
	  else {
	    cout << padString(tstamp_str, 12, false);
	    cout << padString(varname, 18, false);
	    cout << varval << endl;
	  }

	  m_latest_rep[varname] = tstamp_dbl;
	}
      }

      m_unique_posts.insert(full_post);
    }
  }

  if(m_file_in)
    fclose(m_file_in);
  m_file_in = 0;
  
  return(true);
}





