/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AvgHandler.cpp                                       */
/*    DATE: December 15th, 2021                                  */
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
#include "AvgHandler.h"
#include "LogUtils.h"
#include "ACTable.h"

using namespace std;


//--------------------------------------------------------
// Constructor

AvgHandler::AvgHandler()
{
  m_file_in  = 0;
  m_verbose  = false;

  m_format_aligned = true;
  m_format_negpos  = false;
}

//--------------------------------------------------------
// Procedure: setLogFile()

bool AvgHandler::setLogFile(string logfile)
{
  if(m_file_in !=0) {
    if(m_verbose)
      cout << "Only one log file may be provided." << endl;
    return(false);
  }
  
  m_file_in = fopen(logfile.c_str(), "r");
  if(!m_file_in) {
    if(m_verbose)
      cout << "Input file not found or unable to open: " + logfile << endl;
    return(false);
  }
  
  return(true);
}

//--------------------------------------------------------
// Procedure: handle

bool AvgHandler::handle()
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

    string line = stripBlankEnds(line_raw);
    string col1 = biteStringX(line, ' ');
    string col2 = line;

    if(!isNumber(col1) || !isNumber(col2))
      if(m_verbose)
	cout << "bad line: [" << line_raw << "]" << endl;

    double xval = atof(col1.c_str());
    double yval = atof(col2.c_str());

    m_entries[xval].setXVal(xval);
    m_entries[xval].addYVal(yval);
  }

  if(m_file_in)
    fclose(m_file_in);

  // ==============================================
  // Part 2: Make the report
  // ==============================================
  if(m_format_aligned) {
    ACTable actab(5,2);
    
    map<double, ExpEntry>::iterator p;
    for(p=m_entries.begin(); p!=m_entries.end(); p++) {
      double xval = p->first;
      ExpEntry entry = p->second;
      
      double yavg = entry.getYAvg();
      double ymin = entry.getYMin();
      double ymax = entry.getYMax();
      double yneg = entry.getYNeg();
      double ypos = entry.getYPos();
      double ystd = entry.getYStd();

      if(m_format_negpos)
	actab << xval << yavg << yneg << ypos << ystd;
      else
	actab << xval << yavg << ymin << ymax << ystd;
    }
    vector<string> lines = actab.getTableOutput();
    for(unsigned int i=0; i<lines.size(); i++)
      cout << lines[i] << endl;
    
  }
  else {
    map<double, ExpEntry>::iterator p;
    for(p=m_entries.begin(); p!=m_entries.end(); p++) {
      double xval = p->first;
      ExpEntry entry = p->second;
      
      double yavg = entry.getYAvg();
      double ymin = entry.getYMin();
      double ymax = entry.getYMax();
      double yneg = entry.getYNeg();
      double ypos = entry.getYPos();
      double ystd = entry.getYStd();
      
      if(m_format_negpos) {
	cout << xval << " " << yavg << " " << yneg <<
	  " " << ypos << " " << ystd << endl;
      }
      else 
	cout << xval << " " << yavg << " " << ymin <<
	  " " << ymax << " " << ystd << endl;
    }
  }

  return(true);
}






