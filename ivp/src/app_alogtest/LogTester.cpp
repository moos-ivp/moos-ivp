/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogTester.cpp                                        */
/*    DATE: April 19, 2019                                       */
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
#include <sstream>
#include <cstdio>
#include "LogTester.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

// --------------------------------------------------------
// Constructor 

LogTester::LogTester() 
{
  m_verbose = false;
  m_overwrite = false;
} 

// --------------------------------------------------------
// Destructor 

LogTester::~LogTester() 
{
  if(m_fptr)
    fclose(m_fptr);
} 

// --------------------------------------------------------
// Procedure: addTestFile()

bool LogTester::addTestFile(string filename)
{
  if(!okFileToRead(filename))
    return(false);

  if(vectorContains(m_test_files, filename))
    return(false);

  m_test_files.push_back(filename);
  return(true);
}
      
// --------------------------------------------------------
// Procedure: setALogFile()

bool LogTester::setALogFile(string filename)
{
  // If it cant be read, return false
  if(!okFileToRead(filename))
    return(false);

  // If alog file is already set, return false.
  if(m_alog_file != "")
    return(false);

  m_alog_file = filename;
  return(true);
}
      
// --------------------------------------------------------
// Procedure: setMarkFile()

bool LogTester::setMarkFile(string filename)
{
  // Check if it already exists
  if(okFileToRead(filename)) {
    if(!m_overwrite)
      return(false);
  }

  // Check if it can be written to
  if(!okFileToWrite(filename))
    return(false);

  // If mark file is already set, return false.
  if(m_alog_file != "")
    return(false);

  m_mark_file = filename;
  return(true);
}
      

// --------------------------------------------------------
// Procedure: parseTestFiles()

bool LogTester::parseTestFiles()
{
  bool ok = true;
  for(unsigned int i=0; i<m_test_files.size(); i++) {
    ok = ok && parseTestFile(m_test_files[i]);
  }

  return(ok);
}
  

// --------------------------------------------------------
// Procedure: print()

void LogTester::print() const
{
  cout << "Number of LogTests: " << m_tests.size() << endl;
  for(unsigned int i=0; i<m_tests.size(); i++) {
    m_tests[i].print();
  }
}

// --------------------------------------------------------
// Procedure: parseTestFile()
//   Purpose: Parses an input file for conditions.
//   Returns: true if at least one flag has been read from specified file
//            false otherwise.

bool LogTester::parseTestFile(string test_file)
{
  cout << "Beginning to parse test file: " << test_file << endl;

  vector<string> lines = fileBuffer(test_file);
  if(lines.size() == 0) {
    cout << "Invalid or empty file: " << test_file << ". Exiting" << endl;
    return(false);
  }

  LogTest ctest; // current test
  ctest.setTestName("test");
  
  bool all_lines_ok = true;
  string prev_line_part;
  for(unsigned int i=0; i<lines.size(); i++) {
    string orig = stripBlankEnds(lines[i]);
    string line = stripBlankEnds(orig);
    if(strBegins(line, "//"))
      continue;
    if(line.size() == 0)
      continue;

    if(strEnds(line, "\\")) {
      rbiteString(line, '\\');
      prev_line_part = line;
      continue;
    }
    else {
      line = prev_line_part + line;
      prev_line_part = "";
    }

    string param = biteStringX(line, '=');
    string value = line;
    
    bool ok_line = false;
    if(param == "test_name") {
      if(ctest.isEmpty())
	ctest.setTestName(value);
      else {
	m_tests.push_back(ctest);
	ctest = LogTest();
	ctest.setTestName(value);
      }
      ok_line = true;
    }
    else if((param == "start_condition") || (param == "sc"))
      ok_line = ctest.addStartCondition(value);
    else if((param == "end_condition") || (param == "ec"))
      ok_line = ctest.addEndCondition(value);
    else if((param == "pass_condition") || (param == "pc"))
      ok_line = ctest.addPassCondition(value);
    else if((param == "fail_condition") || (param == "fc"))
      ok_line = ctest.addFailCondition(value);
    else if(param == "start_time")
      ok_line = ctest.setStartTime(value);
    else if(param == "end_time")
      ok_line = ctest.setEndTime(value);

    if(!ok_line) {
      cout << "Problem with line " << i << ": " << orig << endl;
      all_lines_ok = false;
    }
  }

  if(!ctest.isEmpty())
    m_tests.push_back(ctest);
  
  return(all_lines_ok);
}
  

// --------------------------------------------------------
// Procedure: test()
//   Purpose: Checks the specified alogfile for the various conditions. 
//   Returns: true if the specified condition was able to be added to 
//               the specified vector of flags 
//            false otherwise.

bool LogTester::test()
{
  bool ok = parseTestFiles();
  if(!ok)
    return(false);
  if(m_verbose) {
    cout << "All Test files properly parsed" << endl;
    print();
  }
    
  m_fptr = fopen(m_alog_file.c_str(), "r");
  if(!m_fptr )
    return(false);

  bool done = false;
  while(!done) {
    ALogEntry curr_entry = getNextRawALogEntry(m_fptr);
    if(curr_entry.getStatus() == "eof" )
      done = true;
    for(unsigned int i=0; i<m_tests.size(); i++) {
      m_tests[i].updateState(curr_entry);
    }
  }

  bool all_passed = finish();
  return(all_passed);
}


// --------------------------------------------------------
// Procedure: finish()
//   Purpose: Close the input file pointer if needed, and
//            determine if all the tests have passed.

bool LogTester::finish()
{
  // Part 1: Summarize all states
  bool all_passed = true;
  for(unsigned int i=0; i<m_tests.size(); i++) 
    all_passed = all_passed && (m_tests[i].getState() == "passed");

  // Part 2: Build results file
  stringstream ss;
  ss << "final_result = " << boolToString(all_passed) << endl;
  
  for(unsigned int i=0; i<m_tests.size(); i++) {
    bool test_passed = (m_tests[i].getState() == "passed");
    string reason;
    if(!test_passed) 
      reason = "(" + m_tests[i].getFailReason() + ")";

    string foo = "=" + m_tests[i].getState() + "=";
    ss << "result for test " << m_tests[i].getName();
    ss << ":" << boolToString(test_passed) << foo << reason << endl;
    vector<string> events = m_tests[i].getEvents();
    for(unsigned int j=0; j<events.size(); j++)
      ss << events[j] << endl;
    ss << endl;
  }
  
  cout << ss.str() << endl;
  
  return(all_passed);
}


