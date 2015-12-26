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
// Procedure: addMarkVar

bool PareEngine::addMarkVar(string mark_var)
{
  if(!vectorContains(m_markvars, mark_var))
    m_markvars.push_back(mark_var);
  return(true);
}

//--------------------------------------------------------
// Procedure: pareFile

bool PareEngine::pareFile()
{
  bool ok_timestamps = findTimeStamps();
  if(!ok_timestamps)
    return(false);
  return(true);
}

//--------------------------------------------------------
// Procedure: findTimeStamps()

bool PareEngine::findTimeStamps()
{
  FILE *file_ptr = fopen(m_alog_file_in.c_str(), "r");
  if(!file_ptr)
    return(false);

  cout << "Gathering timestamps from file : " << m_alog_file_in << endl;

  m_timestamps.clear();
  
  unsigned int line_count  = 0;
  while(1) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    if((line_count % 10000) == 0)
      cout << "+" << flush;
    if((line_count % 100000) == 0)
      cout << " (" << uintToCommaString(line_count) << ") lines" << endl;

    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    if(line_raw == "eof") 
      break;
    
    string var = getVarName(line_raw);
    if(vectorContains(m_markvars, var)) {
      string time_str = getTimeStamp(line_raw);
      double time_dbl = atof(time_str.c_str());
      m_timestamps.push_back(time_dbl);
    }
  }
  cout << endl << uintToCommaString(line_count) << " lines total." << endl;
  
  if(file_ptr)
    fclose(file_ptr);

  return(true);
}

//--------------------------------------------------------
// Procedure: printReport
//     Notes: 

void PareEngine::printReport()
{
  cout << "Total Timestamps: " << m_timestamps.size() << endl;
  for(unsigned int i=0; i<m_timestamps.size(); i++) {
    cout << "[" << i << "]: " << m_timestamps[i] << endl;
  }
}



