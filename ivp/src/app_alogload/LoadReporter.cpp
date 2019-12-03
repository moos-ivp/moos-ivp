/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LoadReporter.cpp                                     */
/*    DATE: December 3rd, 2019                                   */
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
#include "LoadReporter.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Constructor

LoadReporter::LoadReporter()
{
  m_verbose = false;
}


//--------------------------------------------------------
// Procedure: report()

void LoadReporter::report()
{
}

//--------------------------------------------------------
// Procedure: addALogFile()

bool LoadReporter::addALogFile(string alogfile)
{
  if(vectorContains(m_alog_files, alogfile)) {
    if(m_verbose)
      cout << "Duplicate alog filename: " << alogfile << endl;
    return(false);
  }

  if(!okFileToRead(alogfile)) {
    if(m_verbose)
      cout << "Unable to read alog file: " << alogfile << endl;
    return(false);
  }

  m_alog_files.push_back(alogfile);
  return(true);
}

#if 0
//--------------------------------------------------------
// Procedure: handle()

bool LoadReporter::handle(const string& alogfile)
{
  if(alogfile == "") {
    cout << termColor("red");
    cout << "Alog file was not specified. Exiting now." << endl;
    cout << termColor();
    return(false);
  }

  FILE *file_ptr = fopen(alogfile.c_str(), "r");
  if(!file_ptr) {
    cout << termColor("red");
    cout << "Alog file not found or unable to open - exiting" << endl;
    cout << termColor();
    return(false);
  }
  
  cout << "Processing on file : " << alogfile << endl;

  unsigned int line_count  = 0;
  unsigned int life_events = 0;
  bool done = false;
  while(!done) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    if(m_report_life_events && !m_report_mode_changes && !m_report_bhv_changes) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }

    bool line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    
    if(!done && !line_is_comment) {
      string varname = getVarName(line_raw);
      string data = getDataEntry(line_raw);
      if(varname == "IVPHELM_LIFE_EVENT") { 
	m_life_events.addLifeEvent(data);
	life_events++;
      }
      if(m_report_bhv_changes && (varname == "IVPHELM_SUMMARY")) {
	string tstamp = getTimeStamp(line_raw);
	handleNewHelmSummary(data, tstamp);
      }
      if((m_report_mode_changes || m_report_bhv_changes) && 
	 (varname == "IVPHELM_MODESET")) {
	m_mode_var = biteString(data, '#');
      }
      if((m_report_mode_changes || m_report_bhv_changes) && 
	 (varname == m_mode_var)) {
	if(data != m_prev_mode_value) {
	  string tstamp = getTimeStamp(line_raw);
	  cout << "====================================================" << endl;
	  cout << tstamp << " Mode: " << data << endl;
	  m_prev_mode_value = data;
	}
      }
      if(vectorContains(m_watch_vars, varname)) {
	if(m_var_trunc)
	  cout << truncString(line_raw, 80) << endl;
	else
	  cout << line_raw << endl;
      }

    }
  }
  cout << endl << uintToCommaString(line_count) << " lines total." << endl;
  if(m_report_life_events)
    cout << uintToString(life_events) << " life events." << endl;

  if(file_ptr)
    fclose(file_ptr);

  return(true);
}

#endif
