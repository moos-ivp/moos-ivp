/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PareEngine.cpp                                       */
/*    DATE: December 25th, 2015                                  */
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
#include "MBUtils.h"
#include "PareEngine.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Constructor

PareEngine::PareEngine()
{
  m_pare_window = 30;  // seconds
  m_verbose = false;
}

//--------------------------------------------------------
// Procedure: setALogFileIn()

bool PareEngine::setALogFileIn(string alog_file)
{
  if(!okFileToRead(alog_file)) {
    cout << "Unable to read from: " << alog_file << endl;
    exit(1);
  }
  
  m_alog_file_in = alog_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: setALogFileOut()
//      Note: Ok if set to empty string. Output would then just
//            go to the terminal.

bool PareEngine::setALogFileOut(string alog_file)
{
  if((alog_file != "") && !okFileToWrite(alog_file)) {
    cout << "Unable to write to file: " << alog_file << endl;
    return(false);
  }

  m_alog_file_out = alog_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: addMarkListVars

bool PareEngine::addMarkListVars(string mark_vars)
{
  vector<string> svector = parseString(mark_vars, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string var = svector[i];
    if((var != "") && !vectorContains(m_marklist_vars, var))
      m_marklist_vars.push_back(var);
  }
  return(true);
}

//--------------------------------------------------------
// Procedure: addHitListVars

bool PareEngine::addHitListVars(string hit_vars)
{
  vector<string> svector = parseString(hit_vars, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string var = svector[i];
    if((var != "") && !vectorContains(m_hitlist_vars, var))
      m_hitlist_vars.push_back(var);
  }
  return(true);
}

//--------------------------------------------------------
// Procedure: addPareListVars

bool PareEngine::addPareListVars(string pare_vars)
{
  vector<string> svector = parseString(pare_vars, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string var = svector[i];
    if((var != "") && !vectorContains(m_parelist_vars, var))
      m_parelist_vars.push_back(var);
  }
  return(true);
}

//--------------------------------------------------------
// Procedure: pareTheFile

void PareEngine::pareTheFile()
{
  passOneFindTimeStamps();
  passTwoPareTimeStamps();
}

//--------------------------------------------------------
// Procedure: defaultHitList()

void PareEngine::defaultHitList()
{
  addHitListVars("*ITER_GAP, *ITER_LEN");
  addHitListVars("PSHARE*");
  addHitListVars("NODE_REPORT*, DB_QOS");
}

//--------------------------------------------------------
// Procedure: defaultPareList()

void PareEngine::defaultPareList()
{
  addPareListVars("BHV_IPF");
  addPareListVars("VIEW_SEGLIST");
  addPareListVars("VIEW_POINT");
  addPareListVars("CONTACTS_RECAP");
}

//--------------------------------------------------------
// Procedure: passOneFindTimeStamps()

void PareEngine::passOneFindTimeStamps()
{
  FILE *file_ptr = fopen(m_alog_file_in.c_str(), "r");
  if(!file_ptr)
    return;

  if(m_verbose)
    cout << "Gathering timestamps from file: " << m_alog_file_in << endl;
  else
    cout << "Paring Phase 1... " << flush;
  
  m_timestamps.clear();

  bool community_name_found = false;
  
  unsigned int line_count  = 0;
  while(1) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    if(m_verbose) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }
    
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    if(line_raw == "eof") 
      break;
    
    string var = getVarName(line_raw);
    if(vectorContains(m_marklist_vars, var)) {
      string time_str = getTimeStamp(line_raw);
      double time_dbl = atof(time_str.c_str());
      m_timestamps.push_back(time_dbl);
    }
    if(!community_name_found && (var == "DB_TIME")) {
      string src = getSourceName(line_raw);
      m_community_name = rbiteString(src, '_');
      community_name_found = true;
    }
      
  }

  if(m_verbose) {
    cout << endl;
    cout << uintToCommaString(line_count) << " lines total." << endl;
  }
  
  if(file_ptr)
    fclose(file_ptr);
}

//--------------------------------------------------------
// Procedure: passTwoPareTimeStamps()

void PareEngine::passTwoPareTimeStamps()
{
  if((m_alog_file_out == "name.alog") && (m_community_name != ""))
    m_alog_file_out = m_community_name + "_pared.alog";

  FILE *file_ptr_in  = fopen(m_alog_file_in.c_str(), "r");
  FILE *file_ptr_out = fopen(m_alog_file_out.c_str(), "w");

  unsigned int total_timestamps = m_timestamps.size();

  bool one_node_report_saved = false;
  if(!file_ptr_in)
    return;

  if(m_verbose)
    cout << "Paring variables from file: " << m_alog_file_in << endl;
  else
    cout << "Phase 2... " << flush;
  
  unsigned int lines_pared = 0;
  unsigned int line_count  = 0;
  while(1) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr_in);

    //===========================================================
    // Part 1: Output task status if in verbose mode
    //===========================================================
    if(m_verbose) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }

    //===========================================================
    // Part 2: Check for easy saves or easy omits
    //===========================================================
    // Easy SAVE: Line is a comment line
    if((line_raw.length() > 0) && (line_raw.at(0) == '%')) {
      writeLine(file_ptr_out, line_raw);
      continue;
    }
    if(line_raw == "eof") 
      break;

    string var = getVarName(line_raw);
    // Easy SAVE: Must save at least one local node report
    if(!one_node_report_saved) {
      if((var == "NODE_REPORT_LOCAL") || (var == "NODE_REPORT_LOCAL_FIRST")) {
	writeLine(file_ptr_out, line_raw);
	one_node_report_saved = true;
	continue;
      }
    }
      
    // Easy OMIT: Var on hitlist, just skip past it now.
    if(varOnHitList(var)) {
      lines_pared++;
      continue;
    }      
    
    // Easy SAVE Variable is not on the parelist
    if(!varOnPareList(var)) {
      writeLine(file_ptr_out, line_raw);
      continue;
    }

    // If variable is a view variable, and active=false, this is
    // most likely an attempt to erase a previous posting. We want
    // to keep the erasures. 
    if(varOnPareList(var) &&
       strBegins(var, "VIEW_") &&
       strContains(line_raw, "active=false")) {
      writeLine(file_ptr_out, line_raw);
      continue;
    }
      

    
    //===========================================================
    // Part 3: Handle vars on parelist by checking timestamps
    //===========================================================
    // Variable IS on the pare list. Let's see if it can be saved
    // based on its timestamp.
    string time_str = getTimeStamp(line_raw);
    double time = atof(time_str.c_str());

    // Adjust the sequence of timestamps. If the front timestamp
    // is more than pare_window seconds behind the time of the
    // current line, then pop. Keep popping until this isnt true
    // or the list is empty.
    bool need_to_pop = true;
    while(need_to_pop) {
      if(m_timestamps.size() == 0)
	need_to_pop = false;
      else if((time - m_timestamps.front()) < m_pare_window)
	need_to_pop = false;
      else
	m_timestamps.pop_front();
    }

    // Easy case: no more relevant timestamps, entry cannot be saved
    if(m_timestamps.size() == 0)
      continue;
    
    double delta_time = time - m_timestamps.front();
    if(delta_time < 0)
      delta_time *= -1;
    
    // SAVED! Variable is within window of valid time stamp
    if(delta_time <= m_pare_window) 
      writeLine(file_ptr_out, line_raw);
    else
      lines_pared++;
  }

  double pct_lines_pared = 0;
  if(line_count > 0)
    pct_lines_pared = (double)(lines_pared) / (double)(line_count);
  
  if(m_verbose) {
    cout << endl;
    cout << uintToCommaString(line_count)  << " lines total." << endl;
  }
  cout << uintToCommaString(lines_pared) << " lines pared. ";
  cout << "(" << doubleToString(pct_lines_pared*100, 2) << "%) ";
  cout << "[" << uintToString(total_timestamps) << "]" << endl;
  
  if(file_ptr_in)
    fclose(file_ptr_in);
  if(file_ptr_out)
    fclose(file_ptr_out);
}

//--------------------------------------------------------
// Procedure: writeLine

void PareEngine::writeLine(FILE* f, const string& line) const
{
  if(f)
    fprintf(f, "%s\n", line.c_str());
  else
    cout << line << endl;
}
  
//--------------------------------------------------------
// Procedure: varOnMarkList()

bool PareEngine::varOnMarkList(string var)
{
  if(m_mark_cache.count(var))
    return(m_mark_cache[var]);
  
  bool is_markvar = varOnList(m_marklist_vars, var);
  m_mark_cache[var] = is_markvar;
  
  return(is_markvar);
}
  
//--------------------------------------------------------
// Procedure: varOnHitList()

bool PareEngine::varOnHitList(string var)
{
  if(m_hit_cache.count(var))
    return(m_hit_cache[var]);
  
  bool is_hitvar = varOnList(m_hitlist_vars, var);
  m_hit_cache[var] = is_hitvar;
  
  return(is_hitvar);
}
  
//--------------------------------------------------------
// Procedure: varOnPareList()

bool PareEngine::varOnPareList(string var)
{
  if(m_pare_cache.count(var))
    return(m_pare_cache[var]);

  bool is_parevar = varOnList(m_parelist_vars, var);
  m_pare_cache[var] = is_parevar;
  
  return(is_parevar);
}
  
//--------------------------------------------------------
// Procedure: varOnList()
//   Purpose: Determine if given variable is on the given list of
//            variable patterns. 

bool PareEngine::varOnList(vector<string> varlist, string var) const
{
  for(unsigned int i=0; i<varlist.size(); i++) {
    string entry = varlist[i];

    // Check against *FOO* patterns
    if(strBegins(entry, "*") && strEnds(entry, "*")) {
      biteString(entry, '*');
      rbiteString(entry, '*');
      if(strContains(var, entry))
	return(true);
    }
    // Check agains *FOO patterns
    else if(strBegins(entry, "*")) {  
      biteString(entry, '*');
      if(strEnds(var, entry))
	return(true);
    }
    // Check agains FOO* patterns
    else if(strEnds(entry, "*")) {   
      rbiteString(entry, '*');
      if(strBegins(var, entry))
	return(true);
    }
    else if(var == entry)
      return(true);
  }
  return(false);
}
  
//--------------------------------------------------------
// Procedure: printReport

void PareEngine::printReport()
{
  if(!m_verbose)
    return;
  
  cout << "Total Pare Events: " << m_timestamps.size() << endl;
  list<double>::iterator p;
  unsigned int i=0;
  for(p=m_timestamps.begin(); p!=m_timestamps.end(); p++, i++) {
    cout << "[" << i << "]: " << *p << endl;
  }
}








