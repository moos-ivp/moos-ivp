/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogCatHandler.cpp                                   */
/*    DATE: Aug 13th, 2018                                       */
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
#include "LogUtils.h"
#include "ALogCatHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: Constructor

ALogCatHandler::ALogCatHandler()
{
  m_force_overwrite = false;
  m_verbose = false;
}

//--------------------------------------------------------
// Procedure: addALogFile()

bool ALogCatHandler::addALogFile(string alog_file)
{
  if(!strEnds(alog_file, ".alog"))
    return(false);
  
  if(vectorContains(m_alog_files, alog_file))
    return(false);

  if(strContainsWhite(alog_file))
    return(false);
  
  m_alog_files.push_back(alog_file);
  return(true);
}

//-------------------------------------------------------------------------
// Procedure: process()

bool ALogCatHandler::process()
{
  preCheck();
  return(true);
}

//------------------------------------------------------------------------
// Procedure: processFile()

bool ALogCatHandler::processFile(string infile)
{
  return(true);
}


//------------------------------------------------------------------------
// Procedure: preCheck()

bool ALogCatHandler::preCheck()
{
  // Part 1: Check the validity of alog files
  if(m_alog_files.size() <= 1) {
    cout << "Two or more input alog files must be provided. Exiting." << endl;
    return(false);
  }

  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    if(!okFileToRead(m_alog_files[i])) {
      cout << "Cannot read " << m_alog_files[i] << endl;
      return(false);
    }
  }
  
  // Part 2: Check the validity of output alog file
  if(m_outfile == "") {
    cout << "Must provide an output file, e.g., --new=file.alog " << endl;
    return(false);
  }

  if(!okFileToWrite(m_outfile)) {
    cout << "Will not be able to write to: " << m_outfile << endl;
    return(false);
  }

  // Part 3: Get the UTC start and end times for each alog file
  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    cout << "alog file: " << m_alog_files[i] << endl;

    double utc_log_start_time = getLogStartFromFile(m_alog_files[i]);
    cout << " " << doubleToString(utc_log_start_time,2) << endl;
    m_utc_log_start_times.push_back(utc_log_start_time);

    double local_data_start_time = getDataStartTimeFromFile(m_alog_files[i]);
    cout << " " << doubleToString(local_data_start_time, 2) << endl;
    double utc_data_start_time = utc_log_start_time + local_data_start_time;
    m_utc_data_start_times.push_back(utc_data_start_time);
    
    double utc_data_end_time = getDataEndTimeFromFile(m_alog_files[i]);
    cout << " " << doubleToString(utc_data_end_time, 2) << endl;
    m_utc_data_end_times.push_back(utc_data_end_time);
  }

  // Part 4: Ensure no two alog files overlap
  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    for(unsigned int j=0; j<m_alog_files.size(); j++) {
      if(i != j) {
	double i_start = m_utc_data_start_times[i];
	double i_end   = m_utc_data_end_times[i];

	double j_start = m_utc_data_start_times[j];
	double j_end   = m_utc_data_end_times[j];

	bool overlap = false;
	if((i_start > j_start) && (i_start < j_end))
	  overlap = true;
	if((i_end > j_start) && (i_end < j_end))
	  overlap = true;

	if(overlap) {
	  cout << "Overlapping alog files: " << endl;
	  cout << "  " << m_alog_files[i] << endl;
	  cout << "  " << m_alog_files[j] << endl;
	  return(false);
	}
      }
    }
  }

  return(true);
}


