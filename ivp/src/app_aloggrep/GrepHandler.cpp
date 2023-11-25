/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "ALogSorter.h"
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
  m_file_overwrite = false;
  m_appcast_retained = false;

  m_final_only   = false;
  m_first_only   = false;

  m_format_vals  = false;
  m_format_srcs  = false;
  m_format_vars  = false;
  m_format_time  = false;
  m_make_report  = true;

  // When keep_key is true, and subpat enabled, the column will keep
  // the sub-pattern key, e.g., --subpat=spd --keepkey will result in
  // spd=3.2 (as opposed to just 3.2).
  m_keep_key     = false;
  
  m_cache_size   = 1000;

  m_sort_entries  = false;
  m_rm_duplicates = false;
  
  // A "bad" line is a line that is not a comment, and does not begin
  // with a timestamp. As found in entries with CRLF's like DB_VARSUMMARY
  m_badlines_retained = false;

  // A "gapline" is a line that ends in _GAP or _LEN
  m_gaplines_retained = false;

  m_re_sorts = 0;
  m_colsep = ' ';
}

//--------------------------------------------------------
// Procedure: setALogFile()

bool GrepHandler::setALogFile(string alogfile)
{
  // =====================================================
  // Part 1: Sanity Checks
  if(alogfile == "")
    return(false);
  if(m_file_in && m_file_out) {
    cout << "input and output alog files already specified" << endl;
    return(false);
  }
  
  
  // =====================================================
  // Part 2: If no input file yet, treat this as input file
  if(!m_file_in) {
    m_file_in = fopen(alogfile.c_str(), "r");
    if(!m_file_in) {
      cout << "Unable to open file for reading: " << alogfile << endl;
      return(false);
    }
    m_filename_in = alogfile;
    return(true);
  }

  // =====================================================
  // Part 3: If input file has already been set, treat as output
  if(alogfile == m_filename_in) {
    cout << "Input and output .alog files cannot be the same. " << endl;
    return(false);
  }
  
  if(strContains(alogfile, "vname")) {
    string vname_discovered = quickPassGetVName(m_filename_in);
    cout << "vname_discovered:[" << vname_discovered << "]" << endl;
    if(vname_discovered != "")
      alogfile = findReplace(alogfile, "vname", vname_discovered);
  }

  m_file_out = fopen(alogfile.c_str(), "r");
  if(m_file_out && !m_file_overwrite) {
    bool done = false;
    while(!done) {
      cout << alogfile << " already exists. Replace? (y/n [n])" << endl;
      char answer = getCharNoWait();
      if((answer != 'y') && (answer != 'Y')){
	cout << "Aborted: The file " << alogfile;
	cout << " will not be overwritten." << endl;
	return(false);
      }
      if(answer == 'y')
	done = true;
    }
  }
  m_file_out = fopen(alogfile.c_str(), "w");    
  if(!m_file_out) {
    cout << "unable to open " << alogfile << " for writing" << endl;
    return(false);
  }
  return(true);  
}

//--------------------------------------------------------
// Procedure: handle()

bool GrepHandler::handle()
{
  if(!m_file_in) {
    cout << "No input alog file given - exiting" << endl;    
    return(false);
  }

  
  // If DB_VARSUMMARY is explicitly on the variable grep list, then
  // retain all its bad lines (lines not starting with a timestamp)
  for(unsigned int i=0; i<m_keys.size(); i++) {
    if("DB_VARSUMMARY" == m_keys[i])
      m_badlines_retained = true;
  }
  
  // ==========================================================
  // Phase 2: Handle the lines
  // ==========================================================
  ALogSorter sorter;
  sorter.checkForDuplicates(m_rm_duplicates);
  
  bool done_reading_raw    = false;
  bool done_reading_sorted = false;
  while(!done_reading_sorted) {

    if(!done_reading_raw) {
      string line_raw = getNextRawLine(m_file_in);
    
      // Part 1: Check for end of file
      if(line_raw == "eof") 
	done_reading_raw = true;
      else { 
	if(!checkRetain(line_raw))
	  ignoreLine(line_raw);
	else {
	  if(!m_sort_entries) {
	    outputLine(line_raw);
	    if(m_first_only)
	      done_reading_sorted = true;
	  }
	  else {
	    string stime = getTimeStamp(line_raw);
	    double dtime = atof(stime.c_str());
	    
	    ALogEntry entry; 
	    entry.setTimeStamp(dtime);
	    entry.setRawLine(line_raw);
	    
	    bool re_sort_noted = sorter.addEntry(entry);
	    if(re_sort_noted) 
	      m_re_sorts++;

	  }
	}
      }
    }

    // Part 2: pull back the sorted line from the sorter, if any left
    if((sorter.size() > m_cache_size) || done_reading_raw) {
      if(sorter.size() == 0) 
	done_reading_sorted = true;
      else {
	ALogEntry entry = sorter.popEntry();
	string line_raw = entry.getRawLine();
	outputLine(line_raw);
	if(m_first_only)
	  done_reading_sorted = true;
      }
    }
  }

  // ==========================================================
  // Phase 3: Handle last line only case
  // ==========================================================
  if(m_final_only)
    outputLine(m_final_line, true);

  
  if(m_file_out)
    fclose(m_file_out);
  if(m_file_in)
    fclose(m_file_in);
  
  return(true);
}

//--------------------------------------------------------
// Procedure: checkRetain()

bool GrepHandler::checkRetain(string& line_raw)
{
  // Check if the line is a comment and handle or ignore
  if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
    return(m_comments_retained);

  // Handle lines that do not begin with a number (comment
  // lines are already handled above)
  if(!isNumber(line_raw.substr(0,1)))
    return(m_badlines_retained);
      
  string varname = getVarName(line_raw);
      
  if(!m_gaplines_retained) {
    if(strEnds(varname, "_LEN") || strEnds(varname, "_GAP"))
      return(false);
  }
      
  if((!m_appcast_retained) && (varname == "APPCAST"))
    return(false);
  
  // Part 5: Check if this line matches a named var or src
  string srcname = getSourceNameNoAux(line_raw);

  for(unsigned int i=0; i<m_keys.size(); i++) {
    if((varname == m_keys[i]) || (srcname == m_keys[i]))
      return(true);
    else if(m_pmatch[i] && (strContains(varname, m_keys[i]) ||
			    strContains(srcname, m_keys[i])))
      return(true);
  }
  
  return(false);
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
// Procedure: setColSep()

void GrepHandler::setColSep(char c)
{
  if((c == ',') || (c == ':') || (c == ';') || (c == ' '))
    m_colsep = c;  
}

//--------------------------------------------------------
// Procedure: addKey()

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
// Procedure: setFormat()
//    Format: part:part:part
//  Examples: time:val
//            val
//            time:var:val
//      Note: Ok components: var,val,src,time

bool GrepHandler::setFormat(string str)
{
  if(str == "")
    return(false);
  
  vector<string> svector = parseString(str, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string part = tolower(svector[i]);
    if(part == "var")
      m_format_vars = true;
    else if(part == "time")
      m_format_time = true;
    else if(part == "src")
      m_format_srcs = true;
    else if(part != "val")
      return(false);
  }
  
  m_format_vals = true;
  m_comments_retained = false;
  return(true);
}

//--------------------------------------------------------
// Procedure: outputLine()

void GrepHandler::outputLine(const string& line, bool last)
{
  if(line == "")
    return;
  
  if(!last && m_final_only) {
    m_final_line = line;
    return;
  }
  
  // First handle if just output value field
  if(m_format_vals) {
    string line_val = stripBlankEnds(getDataEntry(line));
    string tstamp   = getTimeStamp(line);
    string line_src = getSourceName(line);
    if(tstamp == m_last_tstamp)
      return;

    if(m_subpat.size() != 0) {
      string maybe_line_val;
      for(unsigned int i=0; i<m_subpat.size(); i++) {
	string line_val_low = tolower(line_val);
	string key = m_subpat[i];
	if(strContains(line_val_low, key)) {
	  string val = tokStringParse(line_val_low, key, ',', '=');
	  if(val != "") {
	    if(maybe_line_val != "") {
	      if(m_keep_key)
		maybe_line_val += ", ";
	      else
		maybe_line_val += " ";
	    }
	    string key_str;
	    if(m_keep_key)
	      key_str = key + "=";
	    maybe_line_val += key_str + val;
	  }
	}
      }
      if(maybe_line_val != "")
	line_val = maybe_line_val;
    }

#if 0
    if(m_subpat != "") {
      string line_val_low = tolower(line_val);
      if(strContains(line_val_low, m_subpat)) {
	string val = tokStringParse(line_val_low, m_subpat, ',', '=');
	if(val != "")
	  line_val = val;
      }
    }
#endif
    
    if(m_format_vars) {
      string line_var = stripBlankEnds(getVarName(line));	
      line_val = line_var + m_colsep + line_val;
    }

    if(m_format_time)
      line_val = tstamp + m_colsep + line_val;

    if(m_format_srcs)
      line_val = line_src;
    
    if(m_file_out)
      fprintf(m_file_out, "%s\n", line_val.c_str());
    else
      cout << line_val << endl;
    m_last_tstamp = tstamp;
    return;
  }

  string varname = getVarName(line);
  
  if(m_file_out)
    fprintf(m_file_out, "%s\n", line.c_str());
  else
    cout << line << endl;

  // If line is a comment, don't include in statistics
  if(strBegins(line, "%%"))
     return;
     
  m_lines_retained++;
  m_chars_retained += line.length();
  if(varname.length() > 0)
    m_vars_retained.insert(varname);
}

//--------------------------------------------------------
// Procedure: ignoreLine()

void GrepHandler::ignoreLine(const string& line)
{
  m_lines_removed++;
  m_chars_removed += line.length();
}


//--------------------------------------------------------
// Procedure: printReport()

void GrepHandler::printReport()
{
  // If explicitly asked not to make the report, then dont
  if(!m_make_report)
    return;

  // Don't print the report if in column-data mode
  if(m_format_vals)
    return;

  if(m_sort_entries) 
    cout << "  Total re-sorts: " << uintToString(m_re_sorts) << endl;
  
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











