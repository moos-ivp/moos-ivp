/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SortHandler.cpp                                      */
/*    DATE: June 22nd, 2013                                      */
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
#include "SortHandler.h"
#include "ALogSorter.h"
#include "LogUtils.h"
#include "TermUtils.h"

using namespace std;


//--------------------------------------------------------
// Constructor

SortHandler::SortHandler()
{
  m_file_in  = 0;
  m_file_out = 0;

  m_cache_size  = 1000;
  m_total_lines = 0;
  m_re_sorts    = 0;

  m_file_overwrite = false;
}

//--------------------------------------------------------
// Procedure: handleSort

bool SortHandler::handleSort(const string& alogfile, const string& new_alogfile)
{
  if(alogfile == new_alogfile) {
    cout << "Input and output .alog files cannot be the same. " << endl;
    cout << "Exiting now." << endl;
    return(false);
  }

  m_file_in = fopen(alogfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input not found or unable to open - exiting" << endl;
    return(false);
  }
  
  if(new_alogfile != "") {
    m_file_out = fopen(new_alogfile.c_str(), "r");
    if(m_file_out && !m_file_overwrite) {
      bool done = false;
      while(!done) {
	cout << new_alogfile << " already exists. Replace? (y/n [n])" << endl;
	char answer = getCharNoWait();
	if((answer != 'y') && (answer != 'Y')){
	  cout << "Aborted: The file " << new_alogfile;
	  cout << " will not be overwritten." << endl;
	  return(false);
	}
	if(answer == 'y')
	  done = true;
      }
    }
    m_file_out = fopen(new_alogfile.c_str(), "w");
  }
  
  ALogSorter sorter;

  bool done_reading_raw    = false;
  bool done_reading_sorted = false;
  while(!done_reading_sorted) {

    // Step 1: grab the raw line, if any left,  and add to the sorter
    if(!done_reading_raw) {
      string    line_raw = getNextRawLine(m_file_in);
      
      // Check if line is a comment
      if((line_raw.length() > 0) && (line_raw.at(0) == '%')) {
	if(m_file_out)
	  fprintf(m_file_out, "%s\n", line_raw.c_str());
	else
	  cout << line_raw << endl;
      }

      else if(line_raw == "eof")
	done_reading_raw = true;
      else {
	string    stime = getTimeStamp(line_raw);
	double    dtime = atof(stime.c_str());

	ALogEntry entry; 
	entry.setTimeStamp(dtime);
	entry.setRawLine(line_raw);
      
	bool re_sort_noted = sorter.addEntry(entry);
	if(re_sort_noted)
	  m_re_sorts++;
      }
    }
     
    // Step 2: pull back the sorted line from the sorter, if any left
    if((sorter.size() > m_cache_size) || done_reading_raw) {
      if(sorter.size() == 0) 
	done_reading_sorted = true;
      else {
	ALogEntry entry = sorter.popEntry();
	string line_raw = entry.getRawLine();
	if(m_file_out)
	  fprintf(m_file_out, "%s\n", line_raw.c_str());
	else
	  cout << line_raw << endl;
      }
    }
  }

  if(m_file_out)
    fclose(m_file_out);
  m_file_out = 0;
  if(m_file_in)
    fclose(m_file_in);
  m_file_in = 0;

  return(true);
}

//--------------------------------------------------------
// Procedure: handleCheck

bool SortHandler::handleCheck(const string& alogfile)
{
  m_file_in = fopen(alogfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input not found or unable to open - exiting" << endl;
    return(false);
  }
  
  string prev_line_raw;
  double prev_timestamp = 0;
  bool   first = true;
  bool   done  = false;
  while(!done) {
    string line_raw = getNextRawLine(m_file_in);
    
    bool line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    else if(!line_is_comment) {
      string timestamp = getTimeStamp(line_raw);
      double double_timestamp = atof(timestamp.c_str());
      if(first == true) {
	first = false;
      }
      else {
	if(double_timestamp < prev_timestamp) {
	  cout << "Out-of-order pair detected: " << endl;
	  cout << "  " << prev_line_raw << endl;
	  cout << "  " << line_raw << endl;
	  //	  return(false);
	}
      }
      prev_line_raw = line_raw;
      prev_timestamp = double_timestamp;
    }
  }

  return(true);
}

//--------------------------------------------------------
// Procedure: printReport
//     Notes: 

void SortHandler::printReport()
{
  cout << "  Total lines: " << uintToString(m_total_lines) << endl;
  cout << "  Cache size : " << uintToString(m_cache_size)  << endl;
  cout << "  Re-Sorts :   " << uintToString(m_re_sorts)    << endl;
  cout << endl;
}










