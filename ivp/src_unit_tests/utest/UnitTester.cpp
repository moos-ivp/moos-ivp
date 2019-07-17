/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UnitTester.cpp                                       */
/*    DATE: Nov 5th, 2018                                        */
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

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <map>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "UnitTester.h"

using namespace std;

//--------------------------------------------------------
// Constructor

UnitTester::UnitTester()
{
  m_verbosity = 0;
}

//--------------------------------------------------------
// Procedure: addTestFile()

bool UnitTester::addTestFile(string filename)
{
  if(!okFileToRead(filename)) {
    if(m_verbosity > 0)
      cout << "File " << filename << " is not readable." << endl;
    return(false);
  }
  if(vectorContains(m_test_files, filename)) {
    if(m_verbosity > 0)
      cout << "File " << filename << " has already been added." << endl;
    return(false);
  }
  
  m_test_files.push_back(filename);
  return(true);
    
}

//--------------------------------------------------------
// Procedure: runUnitTests()

bool UnitTester::runUnitTests()
{
  for(unsigned int i=0; i<m_test_files.size(); i++) {
    if(m_verbosity > 0)
      cout << "Testing file: " << m_test_files[i] << endl;
    bool ok = handleUnitTestFile(m_test_files[i]);
    if(!ok) {
      if(m_verbosity > 0)
	cout << "Failed on test file: " << m_test_files[i] << endl;
      return(false);
    }
  }
  return(true);
}
  
//--------------------------------------------------------
// Procedure: handleUnitTestFile()

bool UnitTester::handleUnitTestFile(string filename)
{
  vector<string> lines = fileBuffer(filename);
  if(lines.size() == 0)
    return(false);

  lines = joinLines(lines, true);
  
  string test_cmd;
 
  bool all_lines_ok = true;
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);
    if(strBegins(line, "//") || (line.length() == 0))
      continue;

    if(strBegins(line, "cmd="))
      test_cmd = line.substr(4);
    else if(test_cmd != "") {
      string cmd_args = biteStringX(line, '#');
      string expected = line;
      
      string sys_cmd = test_cmd + " " + cmd_args;
      string actual  = removeWhiteNL(executeOneTest(sys_cmd));

      bool line_ok = true;
      line_ok = line_ok && (actual != "");
      line_ok = line_ok && (!strContains(tolower(actual), "error"));
      line_ok = line_ok && (!strContains(tolower(actual), "exiting"));
      line_ok = line_ok && actualSatExpected(actual, expected);
      
      if(m_verbosity > 0) {
	if(line_ok)
	  cout << " Line " << i+1 << " OK " << endl;
	else
	  cout << " Line " << i+1 << " FAIL " << endl;

	if(!line_ok || (m_verbosity > 1)) {
	  cout << "sys_cmd: [" << sys_cmd  << "]" << endl;
	  cout << "results: [" << actual   << "]" << endl;
	  cout << "expected:[" << expected << "]" << endl;
	}	
      }
      all_lines_ok = all_lines_ok && line_ok;
    }
  }
  return(all_lines_ok);
}




//--------------------------------------------------------
// Procedure: executeOneTest()

string UnitTester::executeOneTest(string cmd)
{
  char buffer[128];
  string result;
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
    while (!feof(pipe)) {
      if (fgets(buffer, 128, pipe) != NULL)
	result += buffer;
    }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  return(result);
}


//--------------------------------------------------------
// Procedure: actualSatExpected()

bool UnitTester::actualSatExpected(string actual, string expected)
{
  map<string,string> map_expect;
  vector<string> svector1 = parseString(expected, ' ');
  for(unsigned int j=0; j<svector1.size(); j++) {
    svector1[j] = stripBlankEnds(svector1[j]);
    string param = biteStringX(svector1[j], '=');
    string value = svector1[j];
    map_expect[param] = value;
  }
      
  map<string,string> map_actual;
  vector<string> svector2 = parseString(actual, ',');
  for(unsigned int k=0; k<svector2.size(); k++) {
    svector2[k] = stripBlankEnds(svector2[k]);
    string param = biteStringX(svector2[k],'=');
    string value = svector2[k];
    map_actual[param] = value;
  }

  bool all_ok = true;
  map<string,string>::iterator p;
  for(p=map_actual.begin(); p!=map_actual.end(); p++) {
    string param = p->first;
    string value = p->second;
    if(map_expect.count(param)) {
      if(map_expect[param] != value) {
	all_ok = false;
	if(m_verbosity > 1) {
	  cout << "Param[" << param << "], [" << value;
	  cout << " != " << map_expect[param] << "]" << endl;
	}
      }
    }
  }
  return(all_ok);
}


