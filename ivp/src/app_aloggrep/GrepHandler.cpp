/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GrepHandler.cpp                                      */
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
#include "GrepHandler.h"
#include "LogUtils.h"
#include "TermUtils.h"

using namespace std;


//--------------------------------------------------------
// Constructor

GrepHandler::GrepHandler()
{
  m_file_in  = 0;
  m_file_out = 0;

  m_lines_removed  = 0;
  m_lines_retained = 0;
  m_chars_removed  = 0;
  m_chars_retained = 0;

  m_comments_retained = true;
  m_var_condition_met = true;
  m_file_overwrite = false;
  m_gaplines_retained = true;
  m_appcast_retained = true;
  
  // A "bad" line is a line that is not a comment, and does not begin
  // with a timestamp. As found in entries with CRLF's like DB_VARSUMMARY
  m_badlines_retained = false;

  // A "gapline" is a line that ends in _GAP or _LEN
  m_gaplines_retained = true;
}

//--------------------------------------------------------
// Procedure: handle

bool GrepHandler::handle(string alogfile, string new_alogfile)
{
  if(alogfile == new_alogfile) {
    cout << "Input and output .alog files cannot be the same. " << endl;
    cout << "Exiting now." << endl;
    return(false);
  }

  if(strContains(new_alogfile, "vname")) {
    string vname_discovered = quickPassGetVName(alogfile);
    if(vname_discovered != "")
      new_alogfile = findReplace(new_alogfile, "vname", vname_discovered);
  }


  m_file_in = fopen(alogfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input not found or unable to open - exiting" << endl;
    return(false);
  }
  
  if(new_alogfile != "") {
    m_file_out = fopen(new_alogfile.c_str(), "r");
    if(m_file_out && !m_file_overwrite) {
      bool done = false;
      while(!done) {
	cout << new_alogfile << " already exists. Replace? (y/n [n])" << endl;
	char answer = getCharNoWait();
	if((answer != 'y') && (answer != 'Y')){
	  cout << "Aborted: The file " << new_alogfile;
	  cout << " will not be overwritten." << endl;
	  return(false);
	}
	if(answer == 'y')
	  done = true;
      }
    }
    m_file_out = fopen(new_alogfile.c_str(), "w");
  }

  // If DB_VARSUMMARY is explicitly on the variable grep list, then
  // retain all its bad lines (lines not starting with a timestamp)
  for(unsigned int i=0; i<m_keys.size(); i++) {
    if("DB_VARSUMMARY" == m_keys[i])
      m_badlines_retained = true;
  }
  
  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(m_file_in);
    
    // Part 1: Check if the line is a comment and handle or ignore
    if((line_raw.length() > 0) && (line_raw.at(0) == '%')) {
      if(m_comments_retained)
	outputLine(line_raw);
      continue;
    }

    // Part 2: Check for end of file
    if(line_raw == "eof") 
      break;

    // Part 3: Handle lines that do not begin with a number (comment
    // lines are already handled above)
    if(!isNumber(line_raw.substr(0,1))) {
      if(m_badlines_retained)
	outputLine(line_raw);
      else
	ignoreLine(line_raw);
      continue;
    }

    // Part 4: If there is a condition, see if it has been met
    string varname = getVarName(line_raw);
    if((m_var_condition != "") && (varname == m_var_condition)) {
      string varval = getDataEntry(line_raw);
      if(tolower(varval) == "true")
	m_var_condition_met = true;
      else
	m_var_condition_met = false;
    }

    if(!m_var_condition_met) {
      ignoreLine(line_raw, varname);
      continue;
    }
      
    if(!m_gaplines_retained) {
      if(strEnds(varname, "_LEN") || strEnds(varname, "_GAP")) {
	ignoreLine(line_raw, varname);
	continue;
      }
    }

    if((!m_appcast_retained) && (varname == "APPCAST")) {
      ignoreLine(line_raw, varname);
      continue;
    }

    // Part 5: Check if this line matches a named var or src
    string srcname = getSourceNameNoAux(line_raw);

    bool match = false;

    for(unsigned int i=0; ((i<m_keys.size()) && !match); i++) {
      if((varname == m_keys[i]) || (srcname == m_keys[i]))
	match = true;
      else if(m_pmatch[i] && (strContains(varname, m_keys[i]) ||
			      strContains(srcname, m_keys[i])))
	match = true;
    }

    // Part 6: Depending whether a match was made, output or ignore the line
    if(match) 
      outputLine(line_raw, varname);
    else
      ignoreLine(line_raw, varname);

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
// Procedure: quickPassGetVName()

string GrepHandler::quickPassGetVName(string alogfile)
{
  FILE* f = fopen(alogfile.c_str(), "r");
  if(!f)
    return("");

  string vname;
  while(1) {
    string line_raw = getNextRawLine(f);
    
    // Part 1: Check if the line is a comment and handle or ignore
    if((line_raw.length() > 0) && (line_raw.at(0) == '%')) 
      continue;
    // Part 2: Check for end of file
    if(line_raw == "eof") 
      break;

    // Part 3: Handle lines that do not begin with a number (comment
    // lines are already handled above)
    if(!isNumber(line_raw.substr(0,1)))
      continue;
    
    // Part 4: Look for and handle DB_TIME variable to get vname from
    // the source field, typically of the form "MOOSDB_vname".
    string varname = getVarName(line_raw);
    if(varname == "DB_TIME") {
      string source = getSourceNameNoAux(line_raw);
      vname = rbiteString(source, '_');
      break;
    }
  }
  fclose(f);
  return(vname);
}

//--------------------------------------------------------
// Procedure: addKey
//     Notes: 

void GrepHandler::addKey(string key)
{
  bool pmatch = false;
  int len = key.length();
  if(key.at(len-1) == '*') {
    pmatch = true;
    key.erase(len-1, 1);
  }
  
  int  ksize = m_keys.size();
  bool prior = false;
  int  prior_ix = -1;
  for(int i=0; i<ksize; i++) {
    if(key == m_keys[i]) {
      prior = true;
      prior_ix = i;
    }
  }
  
  if(!prior) {
    m_keys.push_back(key);
    m_pmatch.push_back(pmatch);
  }

  if(prior && pmatch && !m_pmatch[prior_ix])
    m_pmatch[prior_ix] = true;
}


//--------------------------------------------------------
// Procedure: getMatchedKeys()
//     Notes: 

vector<string> GrepHandler::getMatchedKeys()
{
  vector<string> rvector;

  unsigned int i, vsize = m_keys.size();
  for(i=0; i<vsize; i++) {
    if(m_pmatch[i])
      rvector.push_back(m_keys[i]);
  }
  return(rvector);
}


//--------------------------------------------------------
// Procedure: getUnMatchedKeys()
//     Notes: 

vector<string> GrepHandler::getUnMatchedKeys()
{
  vector<string> rvector;

  unsigned int i, vsize = m_keys.size();
  for(i=0; i<vsize; i++) {
    if(!m_pmatch[i])
      rvector.push_back(m_keys[i]);
  }
  return(rvector);
}

//--------------------------------------------------------
// Procedure: outputLine()

void GrepHandler::outputLine(const string& line, const string& var)
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

void GrepHandler::ignoreLine(const string& line, const string& var)
{
  m_lines_removed++;
  m_chars_removed += line.length();
  if(var.length() > 0)
    m_vars_removed.insert(var);
}


//--------------------------------------------------------
// Procedure: printReport
//     Notes: 

void GrepHandler::printReport()
{
  double total_lines = m_lines_retained + m_lines_removed;
  double total_chars = m_chars_retained + m_chars_removed;

  double pct_lines_retained = (m_lines_retained / total_lines);
  double pct_lines_removed  = (m_lines_removed  / total_lines);
  double pct_chars_retained = (m_chars_retained / total_chars);
  double pct_chars_removed  = (m_chars_removed  / total_chars);

  cout << "  Total lines retained: " << doubleToString(m_lines_retained,0);
  cout << " (" << doubleToString((100*pct_lines_retained),2) << "%)" << endl;
  
  cout << "  Total lines excluded: " << doubleToString(m_lines_removed,0);
  cout << " (" << doubleToString((100*pct_lines_removed),2) << "%)" << endl;

  cout << "  Total chars retained: " << doubleToString(m_chars_retained,0);
  cout << " (" << doubleToString((100*pct_chars_retained),2) << "%)" << endl;

  cout << "  Total chars excluded: " << doubleToString(m_chars_removed,0);
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










