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
  // Init config vars
  m_verbose = false;

  // Init state vars
  m_breach_count = 0;
}


//--------------------------------------------------------
// Procedure: report()

void LoadReporter::report()
{
  for(unsigned int i=0; i<m_alog_files.size(); i++)
    breachCount(m_alog_files[i]);

  cout << "Final Total Breaches: " << m_breach_count << endl;
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

//--------------------------------------------------------
// Procedure: breachCount()

bool LoadReporter::breachCount(string alogfile)
{
  if(m_verbose)
    cout << "Examining alogfile: " << alogfile << endl;
  
  if(alogfile == "")
    return(false);

  FILE *file_ptr = fopen(alogfile.c_str(), "r");
  if(!file_ptr) {
    if(m_verbose)
      cout << "Unable to open alog file: " << alogfile << endl;
    return(false);
  }

  unsigned int max_breach_count = 0;
  
  unsigned int line_count  = 0;
  bool done = false;
  while(!done) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    bool line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    
    if(!done && !line_is_comment) {
      string varname = getVarName(line_raw);
      string sdata = getDataEntry(line_raw);
      if(varname == "ULW_BREACH_COUNT") {
	if(isNumber(sdata)) {
	  double dcount = atof(sdata.c_str());
	  unsigned int count = (unsigned int)(dcount);
	  if(count > max_breach_count)
	    max_breach_count = count;
	}
      }
    }
  }

  m_breach_count += max_breach_count;

  if(m_verbose)
    cout << "  Total breaches: " << max_breach_count << endl;

  
  return(true);
}
