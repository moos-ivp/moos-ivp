/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FiltHandler.cpp                                      */
/*    DATE: August 6th, 2008                                     */
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
#include "FiltHandler.h"
#include "LogUtils.h"

using namespace std;


//--------------------------------------------------------
// Constructor

FiltHandler::FiltHandler()
{
  m_file_in       = 0;
  m_file_out      = 0;
  m_chuck_strings = false;
  m_chuck_numbers = false;
  m_logstart      = -1;
  m_timeshift     = false;
  m_clean         = false;

  m_lines_removed  = 0;
  m_lines_retained = 0;
  m_chars_removed  = 0;
  m_chars_retained = 0;

  m_file_overwrite = false;
}

//--------------------------------------------------------
// Procedure: setParam
//     Notes: 

bool FiltHandler::setParam(const string& param, const string& value)
{
  if(param == "nostrings")
    return(setBooleanOnString(m_chuck_strings, value));
  else if(param == "clean")
    return(setBooleanOnString(m_clean, value));
  else if(param == "nonumbers")
    return(setBooleanOnString(m_chuck_numbers, value));
  else if(param == "timeshift")
    return(setBooleanOnString(m_timeshift, value));
  else if(param == "file_overwrite")
    return(setBooleanOnString(m_file_overwrite, value));
  else if(param == "newkey") {
    addVectorKey(m_keys, m_pmatch, value);
    return(true);
  }
  return(false);
}


//--------------------------------------------------------
// Procedure: handle
//     Notes: 

bool FiltHandler::handle(const string& alogfile, const string& new_alogfile)
{
  if(alogfile == new_alogfile) {
    cout << "Input and output .alog files cannot be the same. " << endl;
    cout << "Exiting now." << endl;
    return(false);
  }

  m_file_in = fopen(alogfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input not found or unable to open - exiting now." << endl;
    return(false);
  }
  
  if(new_alogfile != "") {
    m_file_out = fopen(new_alogfile.c_str(), "r");
    if(m_file_out && !m_file_overwrite) {
      cout << new_alogfile << " already exists. " << endl;
      cout << "Use --force to overwrite. Exiting now." << endl;
      fclose(m_file_out);
      return(false);
    }
    m_file_out = fopen(new_alogfile.c_str(), "w");
  }
  
  int  count = 0;
  bool done  = false;
  while(!done) {
    count++;
    string line_raw = getNextRawLine(m_file_in);
    bool   line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(count == 4)
      m_logstart = getLogStart(line_raw);      

    if((line_raw == "eof") || (line_raw == "err"))
      done = true;
    else {
      string varname = getVarName(line_raw);
      string srcname = getSourceNameNoAux(line_raw);

      int ksize = m_keys.size();
      bool match = false;
      for(int i=0; ((i<ksize) && !match); i++) {
	if((varname == m_keys[i]) || (srcname == m_keys[i]))
	  match = true;
	else if(m_pmatch[i] && (strContains(varname, m_keys[i]) ||
				strContains(varname, m_keys[i])))
	  match = true;
      }
      
      if(m_clean && (count >= 6)) {
	string time_stamp = getTimeStamp(line_raw);
	if(!isNumber(time_stamp))
	  match = true;
	string data_field = getDataEntry(line_raw);
	if(data_field == "") 
	  match = true;
      }
      
      
      if((!match || (count < 6)) && (line_raw != "")) {
	string data_field = getDataEntry(line_raw);

	if(m_chuck_strings && (count > 5)  && 
	   (varname != "") && !isNumber(data_field))
	  addVectorKey(m_keys, m_pmatch, varname);
	else if(m_chuck_numbers && (count > 5) &&
		(varname != "") && isNumber(data_field))
	  addVectorKey(m_keys, m_pmatch, varname);
	else {
	  stripInsigDigits(line_raw);
	  if(m_timeshift)
	    shiftTimeStamp(line_raw, m_logstart);
	  if(m_file_out)
	    fprintf(m_file_out, "%s\n", line_raw.c_str());
	  else
	    cout << line_raw << endl;
	}
      }

      if(!match && !line_is_comment) {
	m_lines_retained++;
	m_chars_retained += line_raw.length();
	m_vars_retained.insert(varname);
      }
      
      if(match && !line_is_comment) {
	m_lines_removed++;
	m_chars_removed += line_raw.length();
	m_vars_removed.insert(varname);
      }
    }
  }
  if(m_file_out)
    fclose(m_file_out);
  m_file_out = 0;

  return(true);
}

//--------------------------------------------------------
// Procedure: printReport
//     Notes: 

void FiltHandler::printReport()
{
  double total_lines = m_lines_retained + m_lines_removed;
  double total_chars = m_chars_retained + m_chars_removed;

  double pct_lines_retained = (m_lines_retained / total_lines);
  double pct_lines_removed  = (m_lines_removed  / total_lines);
  double pct_chars_retained = (m_chars_retained / total_chars);
  double pct_chars_removed  = (m_chars_removed  / total_chars);

  cout << "  Total lines retained: " << doubleToString(m_lines_retained,0);
  cout << " (" << doubleToString((100*pct_lines_retained),2) << "%)" << endl;
  
  cout << "   Total lines deleted: " << doubleToString(m_lines_removed,0);
  cout << " (" << doubleToString((100*pct_lines_removed),2) << "%)" << endl;

  cout << "  Total chars retained: " << doubleToString(m_chars_retained,0);
  cout << " (" << doubleToString((100*pct_chars_retained),2) << "%)" << endl;

  cout << "   Total chars deleted: " << doubleToString(m_chars_removed,0);
  cout << " (" << doubleToString((100*pct_chars_removed),2) << "%)" << endl;

  cout << "    Variables retained: (" << m_vars_retained.size() << ") "; 
  set<string>::iterator p;
  for(p=m_vars_retained.begin(); p!=m_vars_retained.end(); p++) {
    string varname = *p;
    if(p!=m_vars_retained.begin())
      cout << ", ";
    cout << varname;
  }
  cout << endl;
}






