/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
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
  m_clean         = false;

  m_lines_removed  = 0;
  m_lines_retained = 0;
  m_chars_removed  = 0;
  m_chars_retained = 0;

  m_file_overwrite = false;

  // A "bad" line is a line that is not a comment, and does not begin
  // with a timestamp. As found in entries with CRLF's like DB_VARSUMMARY
  m_badlines_retained = true;
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

  // If DB_VARSUMMARY is explicitly on the variable rm list, then
  // discard all its bad lines (lines not starting with a timestamp)
  for(unsigned int i=0; i<m_keys.size(); i++) {
    if("DB_VARSUMMARY" == m_keys[i])
      m_badlines_retained = false;
  }

  // Begin the filtering line by line by line by line by line by line
  bool done  = false;
  while(!done) {
    string line_raw = getNextRawLine(m_file_in);

    // Part 1: Check if the line is a comment and handle or ignore
    if((line_raw.length() > 0) && (line_raw.at(0) == '%')) {
      outputLine(line_raw);
      continue;
    }

    // Part 2: Check for end of file
    if((line_raw == "eof") || (line_raw == "err"))
      break;


    // Part 3: Handle lines that do not begin with a number (comment
    // lines are already handled above)
    if(!isNumber(line_raw.substr(0,1))) {
      if(m_clean || !m_badlines_retained)
	ignoreLine(line_raw);
      else
	outputLine(line_raw);
      continue;
    }
    
    // Part 4: Check if this line matches a named var or src
    string varname = getVarName(line_raw);
    string srcname = getSourceNameNoAux(line_raw);

    bool match = false;
    for(unsigned int i=0; (i<m_keys.size()) && !match; i++) {
      if((varname == m_keys[i]) || (srcname == m_keys[i])) 
	match = true;
      else if(m_pmatch[i] && (strContains(varname, m_keys[i]) ||
			 strContains(srcname, m_keys[i]))) 
	match = true;
    }
    if(match) {
      ignoreLine(line_raw, varname);
      continue;
    }
    
    // Part 5: If not filtered out yet, possibly filter out here if 
    // filtering out all string or all doubles
    string data_field = getDataEntry(line_raw);
      
    if((m_chuck_strings && !isNumber(data_field)) ||
       (m_chuck_numbers && isNumber(data_field))) {
      addVectorKey(m_keys, m_pmatch, varname);
      ignoreLine(line_raw, varname);
      continue;
    }

    outputLine(line_raw, varname);
  }

  if(m_file_out)
    fclose(m_file_out);
  m_file_out = 0;
  if(m_file_in)
    fclose(m_file_in);
  m_file_in = 0;

  return(true);
}

//--------------------------------------------------------
// Procedure: outputLine()

void FiltHandler::outputLine(const string& line, const string& var)
{
  if(m_file_out)
    fprintf(m_file_out, "%s\n", line.c_str());
  else
    cout << line << endl;

  m_lines_retained++;
  m_chars_retained += line.length();
  if(var.length() > 0)
    m_vars_retained.insert(var);
}

//--------------------------------------------------------
// Procedure: ignoreLine()

void FiltHandler::ignoreLine(const string& line, const string& var)
{
  m_lines_removed++;
  m_chars_removed += line.length();
  if(var.length() > 0)
    m_vars_removed.insert(var);
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










