/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogBinHandler.cpp                                    */
/*    DATE: August 26th, 2020                                    */
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
#include "LogBinHandler.h"
#include "LogUtils.h"

using namespace std;


//--------------------------------------------------------
// Constructor

LogBinHandler::LogBinHandler()
{
  m_file_in  = 0;
  m_delta    = 1;
  m_minval   = 0;
  m_verbose  = false;
}

//--------------------------------------------------------
// Procedure: setLogFile()

bool LogBinHandler::setLogFile(string logfile)
{
  m_file_in = fopen(logfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input file not found or unable to open - exiting" << endl;
    return(false);
  }
  
  return(true);
}

//--------------------------------------------------------
// Procedure: setDelta()

bool LogBinHandler::setDelta(string str)
{
  return(setPosDoubleOnString(m_delta, str));
}

//--------------------------------------------------------
// Procedure: setMinVal()

bool LogBinHandler::setMinVal(string str)
{
  return(setDoubleOnString(m_minval, str));
}



//--------------------------------------------------------
// Procedure: handle

bool LogBinHandler::handle()
{
  if(!m_file_in)
    return(false);

  // ==============================================
  // Part 1: Sort the bins
  // ==============================================
  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(m_file_in);

    // Check for end of file
    if(line_raw == "eof") 
      break;
    if(line_raw.at(0) == '%')
      continue;
    if(!isNumber(line_raw)) {
      cout << "Bad line: [" << line_raw << "]" << endl;
      continue;
    }

    double dbl_val = atof(line_raw.c_str());
    double bin_val = (trunc((dbl_val - m_minval) / m_delta) + 0.5) * m_delta;
    string bin_str = doubleToStringX(bin_val,6);

    if(m_verbose)
      cout << dbl_val << " --> " << bin_str << endl;
    
    m_bins[bin_val]++;
  }

  if(m_file_in)
    fclose(m_file_in);

  // ==============================================
  // Part 2: Make the report
  // ==============================================

  map<double, unsigned int>::iterator p;
  for(p=m_bins.begin(); p!=m_bins.end(); p++) {
    //string bin_str = p->first;
    double bin_val = p->first;
    string bin_str = doubleToStringX(bin_val, 8);
    
    cout << bin_str << "  " << p->second << endl;
  }
  
  
  return(true);
}






