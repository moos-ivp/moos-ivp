/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
    return(false);
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
// Procedure: addMarkListVar

bool PareEngine::addMarkListVar(string mark_var)
{
  if(!vectorContains(m_marklist_vars, mark_var))
    m_marklist_vars.push_back(mark_var);
  return(true);
}

//--------------------------------------------------------
// Procedure: addHitListVar

bool PareEngine::addHitListVar(string hitlist_var)
{
  if(!vectorContains(m_hitlist_vars, hitlist_var))
    m_hitlist_vars.push_back(hitlist_var);
  return(true);
}

//--------------------------------------------------------
// Procedure: addPareListVar

bool PareEngine::addPareListVar(string parelist_var)
{
  if(!vectorContains(m_parelist_vars, parelist_var))
    m_parelist_vars.push_back(parelist_var);
  return(true);
}

//--------------------------------------------------------
// Procedure: pareFile

void PareEngine::pareFile()
{
  passOneFindTimeStamps();
  passTwoPareTimeStamps();
}

//--------------------------------------------------------
// Procedure: defaultHitList()

void PareEngine::defaultHitList()
{
  addHitListVar("*ITER_GAP");
  addHitListVar("*ITER_LEN");
  addHitListVar("PSHARE*");
  addHitListVar("NODE_REPORT*");
  addHitListVar("DB_QOS");
}

//--------------------------------------------------------
// Procedure: defaultPareList()

void PareEngine::defaultPareList()
{
  addPareListVar("BHV_IPF");
  addPareListVar("VIEW_SEGLIST");
  addPareListVar("VIEW_POINT");
  addPareListVar("CONTACTS_RECAP");
}

//--------------------------------------------------------
// Procedure: passOneFindTimeStamps()

void PareEngine::passOneFindTimeStamps()
{
  FILE *file_ptr = fopen(m_alog_file_in.c_str(), "r");
  if(!file_ptr)
    return;

  if(m_verbose)
    cout << "Gathering timestamps from file : " << m_alog_file_in << endl;

  m_timestamps.clear();
  
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
  FILE *file_ptr_in  = fopen(m_alog_file_in.c_str(), "r");
  FILE *file_ptr_out = fopen(m_alog_file_out.c_str(), "a+");

  if(!file_ptr_in)
    return;

  unsigned int lines_pared = 0;
  unsigned int line_count  = 0;
  while(1) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr_in);

    if(m_verbose) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }
    // SAVE! Line is a comment line
    if((line_raw.length() > 0) && (line_raw.at(0) == '%')) {
      writeLine(file_ptr_out, line_raw);
      continue;
    }
    if(line_raw == "eof") 
      break;

    string var = getVarName(line_raw);

    // If var on hitlist, just skip past it now.
    if(varOnHitList(var)) {
      lines_pared++;
      continue;
    }      
    
    // SAVED! Variable is not on the hit list
    if(!varOnPareList(var)) {
      writeLine(file_ptr_out, line_raw);
      continue;
    }

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
  if(m_verbose) {
    cout << endl;
    cout << uintToCommaString(line_count)  << " lines total." << endl;
    cout << uintToCommaString(lines_pared) << " lines pared." << endl;
  }
  
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
  
  cout << "Total Timestamps: " << m_timestamps.size() << endl;

  list<double>::iterator p;
  unsigned int i=0;
  for(p=m_timestamps.begin(); p!=m_timestamps.end(); p++, i++) {
    cout << "[" << i << "]: " << *p << endl;
  }
}



