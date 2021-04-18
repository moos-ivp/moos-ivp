/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogEvaluator.cpp                                    */
/*    DATE: October 6th, 2020                                    */
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
#include "ALogEvaluator.h"
#include "LogUtils.h"
#include "FileBuffer.h"

using namespace std;

//--------------------------------------------------------
// Constructor

ALogEvaluator::ALogEvaluator()
{
  m_verbose = false;
  
  m_info_buffer = new InfoBuffer;

  m_lcheck_set.setInfoBuffer(m_info_buffer);
}

//--------------------------------------------------------
// Procedure: setALogFile()

bool ALogEvaluator::setALogFile(string alog_file)
{
  if(!okFileToRead(alog_file))
    return(false);

  m_alog_file = alog_file;
  
  if(m_verbose)
    cout << "Ok ALog file: " << alog_file << endl;
  return(true);
}

//--------------------------------------------------------
// Procedure: setTestFile()

bool ALogEvaluator::setTestFile(string test_file)
{
  if(!okFileToRead(test_file))
    return(false);

  m_test_file = test_file;
  
  if(m_verbose)
    cout << "Ok Test file: " << test_file << endl;
  return(true);
}


//--------------------------------------------------------
// Procedure: print()

void ALogEvaluator::print() const
{
  list<string>::iterator p;
  // Part 1: LCheck Info
  //if(m_lcheck_set.enabled()) {
  if(1) { 
    list<string> summary_lines1 = m_lcheck_set.getReport();
    for(p=summary_lines1.begin(); p!=summary_lines1.end(); p++) {
      string line = *p;
      cout << line << endl;
    }
    cout << endl;
  }

  // Part 2: VCheck Info                                            
  //if(m_vcheck_set.enabled()) {
  if(1) {
    list<string> summary_lines2 = m_vcheck_set.getReport();
    for(p=summary_lines2.begin(); p!=summary_lines2.end(); p++) {
      string line = *p;
      cout << line << endl;
    }
  }
}


//--------------------------------------------------------
// Procedure: handle()

bool ALogEvaluator::handle()
{
  bool ok = handleTestFile();
  if(ok)
    ok = handleALogFile();

  if(!ok)
    return(false);

  m_lcheck_set.isEvaluated(); 
  m_lcheck_set.isSatisfied();
  m_vcheck_set.isEvaluated(); 
  m_vcheck_set.isSatisfied();

#if 0
  bool lcheck_evaluated = m_lcheck_set.isEvaluated(); 
  bool lcheck_satisfied = m_lcheck_set.isSatisfied();

  bool vcheck_evaluated = m_vcheck_set.isEvaluated(); 
  bool vcheck_satisfied = m_vcheck_set.isSatisfied();
#endif
  
  return(ok);
}


//--------------------------------------------------------
// Procedure: handleTestFile()

bool ALogEvaluator::handleTestFile()
{
  cout << "In ALogEvaluator::handleTestFile()" << endl;
  vector<string> lines = fileBuffer(m_test_file);
  if(lines.size() == 0) {
    cout << "Empty or unfound testfile: " << m_test_file << endl;
    return(false);
  }

  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);
    if(line.length() == 0)
      continue;
    if(line.at(0) == '#')
      continue;

    string param = tolower(biteStringX(line, '='));
    string value = line;
	   
    bool handled = false;
    if(param == "lead_condition")
      handled = m_lcheck_set.addLeadCondition(value);
    else if(param == "pass_condition") 
      handled = m_lcheck_set.addPassCondition(value);
    else if(param == "fail_condition") 
      handled = m_lcheck_set.addFailCondition(value);

    else if(param == "vcheck_start")
      handled = m_vcheck_set.setStartTrigger(value);
    else if(param == "vcheck")
      handled = m_vcheck_set.addVCheck(value);
    else if(param == "fail_on_first")
      handled = m_vcheck_set.setFailOnFirst(value);
    else if(param == "vcheck_max_report")
      handled = m_vcheck_set.setMaxReportSize(value);

    if(!handled) {
      cout << "Unhandled test file line: " << line << endl;
      return(false);
    }
  }

  string lcheck_config_warning = m_lcheck_set.checkConfig();
  if(lcheck_config_warning != "") {
    cout << "Problem with lcheck_config:" << lcheck_config_warning << endl;
    return(false);
  }

  string vcheck_config_warning = m_vcheck_set.checkConfig();
  if(vcheck_config_warning != "") {
    cout << "Problem with vcheck_config:" << vcheck_config_warning << endl;
    return(false);
  }

  cout << "End ALogEvaluator::handleTestFile()" << endl;
  return(true);
}


//--------------------------------------------------------
// Procedure: handleALogFile()

bool ALogEvaluator::handleALogFile()
{
  if(m_verbose) {
    cout << "Starting handle: " << endl;
  }

  FILE *file_ptr = fopen(m_alog_file.c_str(), "r");
  if(!file_ptr) {
    cout << "ALog not found or unable to open - exiting" << endl;
    return(false);
  }

  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(file_ptr);
    
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

    string tstamp_str = getTimeStamp(line_raw);
    double tstamp_dbl = atof(tstamp_str.c_str());

    if((varname == "DEPLOY") || (varname == "MISSION")) {
      cout << "var:[" << varname << "], val:[" << varval << "]" << endl;
    }
    
    double dval = 0;
    if(isNumber(varval))
      dval = atof(varval.c_str());

    m_vcheck_set.handleMail(varname, varval, dval, tstamp_dbl);

#if 0
    //    if(isNumber(varval))
    // m_lcheck_set.handleMail(varname, dval);
    //else
    m_lcheck_set.handleMail(varname, varval);
#endif 
    
    if(isNumber(varval))
      m_info_buffer->setValue(varname, dval, tstamp_dbl);
    else
      m_info_buffer->setValue(varname, varval, tstamp_dbl);
    
    m_lcheck_set.update();
    m_vcheck_set.update(tstamp_dbl);

  }
  
  if(file_ptr)
    fclose(file_ptr);
  
  return(true);
}

