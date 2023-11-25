/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "BundleOut.h"
#include "ALogEvaluator.h"
#include "LogUtils.h"
#include "FileBuffer.h"

using namespace std;

//--------------------------------------------------------
// Constructor

ALogEvaluator::ALogEvaluator()
{
  m_verbose = false;
  m_show_sequence = false;
  
  m_info_buffer = new InfoBuffer;

  m_passed = false;
  
  m_lcheck_set.setInfoBuffer(m_info_buffer);
  m_logic_tests.setInfoBuffer(m_info_buffer);
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
// Procedure: handle()

bool ALogEvaluator::handle()
{
  bool ok = handleTestFile();
  if(!ok)
    return(false);

  outputTestSequence();
  ok = handleALogFile();
  if(!ok)
    return(false);
  outputTestSequence();

  if(m_logic_tests.isEvaluated()) {
    if(m_logic_tests.isSatisfied())
      m_passed = true;
  }

  if(m_verbose)
    cout << "Result: pass=" << boolToString(m_passed) << endl;

  return(true);
}


//--------------------------------------------------------
// Procedure: handleTestFile()

bool ALogEvaluator::handleTestFile()
{
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
      handled = m_logic_tests.addLeadCondition(value);
    else if(param == "pass_condition") 
      handled = m_logic_tests.addPassCondition(value);
    else if(param == "fail_condition") 
      handled = m_logic_tests.addFailCondition(value);

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

  string lcheck_config_warning = m_logic_tests.checkConfig();
  if(lcheck_config_warning != "") {
    cout << "Problem with lcheck_config:" << lcheck_config_warning << endl;
    return(false);
  }

  string vcheck_config_warning = m_vcheck_set.checkConfig();
  if(vcheck_config_warning != "") {
    cout << "Problem with vcheck_config:" << vcheck_config_warning << endl;
    return(false);
  }

  return(true);
}


//--------------------------------------------------------
// Procedure: handleALogFile()

bool ALogEvaluator::handleALogFile()
{
  set<string> logic_vars = m_logic_tests.getLogicVars();
  string logic_vars_str = stringSetToString(logic_vars);

  if(m_verbose) 
    cout << "BEGIN ALogEvaluator: Handling ALog File:" << endl;

  FILE *file_ptr = fopen(m_alog_file.c_str(), "r");
  if(!file_ptr) {
    cout << "ALog not found or unable to open - exiting" << endl;
    return(false);
  }

  bool done = false;
  string prev_entry;
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

    bool relevant_var = false;
    if(logic_vars.count(varname))
      relevant_var = true;

    double dval = 0;
    if(isNumber(varval))
      dval = atof(varval.c_str());

    m_vcheck_set.handleMail(varname, varval, dval, tstamp_dbl);

    if(isNumber(varval))
      m_info_buffer->setValue(varname, dval, tstamp_dbl);
    else
      m_info_buffer->setValue(varname, varval, tstamp_dbl);
    
    if(relevant_var) {
      m_logic_tests.update();
      bool evaluated = m_logic_tests.isEvaluated();

      if(m_verbose) {
	tstamp_str = padString(tstamp_str, 11, true);
	varname = padString(varname, 9, true);
	if(isNumber(varval))
	   varval = doubleToStringX(dval, 5);
	unsigned int logic_curr_ix = m_logic_tests.currIndex();
	bool satisfied = m_logic_tests.isSatisfied();
	string result = "[" + boolToString(evaluated);
	result += "," + boolToString(satisfied) + "]";
	result = padString(result, 13, false);
	result += " (" + uintToString(logic_curr_ix) + ")";

	string entry = " " + tstamp_str + "  " + varname + "  ";
	entry += varval + "   " + result;

	if(entry != prev_entry) {
	  cout << entry << endl;
	  prev_entry = entry;
	}
      }	

      if(evaluated) {
	done = true;
	if(m_verbose)
	  cout << "EVALUATION COMPLETE" << endl;
      }
      
    }
  }
  if(file_ptr)
    fclose(file_ptr);
  
  if(m_verbose) 
    cout << "END  ALogEvaluator: Handling ALog File" << endl;

  return(true);
}


//--------------------------------------------------------
// Procedure: outputTestSequence()

void ALogEvaluator::outputTestSequence()
{
  if(!m_show_sequence || !m_verbose)
    return;

  cout << "BEGIN ALogEvaluator LogicTestSequence ========= " << endl;
  vector<string> spec = m_logic_tests.getSpec();
  for(unsigned int i=0; i<spec.size(); i++)
    cout << spec[i] << endl;
  cout << "END   ALogEvaluator LogicTestSequence ========= " << endl;
}



