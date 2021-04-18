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

  m_file_out = 0;
}

//--------------------------------------------------------
// Procedure: setVerbose()

void ALogCatHandler::setVerbose()
{
  if(!m_verbose) {
    m_verbose = true;
    return;
  }

  m_debug = true;
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
  if(!preCheck())
    return(false);

  if(!processFirstFile())
    return(false);

  if(!processOtherFiles())
    return(false);

  if(m_verbose)
    cout << "Created new alog file: " << m_outfile << endl;
  
  return(true);
}

//------------------------------------------------------------------------
// Procedure: processFirstFile()

bool ALogCatHandler::processFirstFile()
{
  // Part 1: Sanity checks
  if(m_alog_files.size() <= 1)
    return(false);
  string first_file = m_alog_files[0];
  if(m_verbose) 
    cout << "Processing file: " << first_file << endl;

  if(m_file_out != 0) {
    cout << "Expected null output file pointer at the start" << endl;
    return(false);
  }
  if(m_outfile == "") {
    cout << "No output file provided." << endl; 
    return(false);
  }


  // Part 2: Open the first alog file
  FILE *file_in = fopen(first_file.c_str(), "r");
  if(!file_in) {
    cout << "Failed to open file [" << first_file << "] for reading." << endl;
    return(false);
  }

  // Part 3: Ensure we can write to the output file, and create file ptr
  m_file_out = fopen(m_outfile.c_str(), "w");
  if(m_file_out == 0) {
    cout << "Failed to open file [" << m_outfile << "] for writing." << endl;
    fclose(file_in);
    return(false);
  }

  bool done = false;
  while(!done) {
    string line = getNextRawLine(file_in);
    if(line == "eof") 
      done = true;
    else
      fprintf(m_file_out, "%s\n", line.c_str());
  }
  
  fclose(file_in);
  return(true);
}

//------------------------------------------------------------------------
// Procedure: processOtherFiles()

bool ALogCatHandler::processOtherFiles()
{
  // Part 1: Sanity checks
  if(!m_file_out) {
    cout << "Expected valid output file pointer." << endl;
    return(false);
  }
  if(m_alog_files.size() <= 1)
    return(false);


  double first_utc_log_start_time = m_utc_log_start_times[0];
  
  for(unsigned int i=1; i<m_alog_files.size(); i++) {
    
    string file = m_alog_files[i];
    if(m_verbose) 
      cout << "Processing file: " << file << endl;

    FILE *file_in = fopen(file.c_str(), "r");
    if(!file_in) {
      cout << "Failed to open file [" << file << "] for reading." << endl;
      return(false);
    }
      
    double utc_log_start_time = m_utc_log_start_times[i];

    double delta = utc_log_start_time - first_utc_log_start_time;

    bool done = false;
    unsigned long int linenum = 0;
    while(!done) {
      linenum++;
      string line_raw = getNextRawLine(file_in);
      // Part 1: Check for end of file
      if(line_raw == "eof") 
	break;

      // Part 2: Check if the line is a comment and handle or ignore
      if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
	continue;

      // Part 3: Get the timestamp of the line, ensure its valid num
      string timestamp = getTimeStamp(line_raw);
      if(!isNumber(timestamp))
	continue;
      
      // Part 4: Apply the delta. Show warning if negative tstamp
      double dtime = atof(timestamp.c_str());
      dtime += delta;
      if(dtime < 0) {
	cout << "[" << doubleToString(delta,2) << "]";
	cout << "[" << doubleToString(dtime,2) << "]";
	cout << "Negative tstamp in file: " << file;
	cout << ", on line " << linenum << endl;
      }
          
      // Part 5: Write the new line back to the output file
      string stime = doubleToString(dtime, 3);
      string line = findReplace(line_raw, timestamp, stime);
      fprintf(m_file_out, "%s\n", line.c_str());
    }
    fclose(file_in);
  }

  return(true);
}


//------------------------------------------------------------------------
// Procedure: preCheck()

bool ALogCatHandler::preCheck()
{
  if(m_verbose)
    cout << "Performing a pre-check on the list of alog files..." << endl;
  
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
  
  // Part 2: Check if output file exists
  if(okFileToRead(m_outfile) && !m_force_overwrite) {
    cout << "Output file [" << m_outfile << "] already exists. " << endl;
    cout << "Use the --force option to force an overwite." << endl;
    return(false);
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

    double utc_log_start_time = getLogStartFromFile(m_alog_files[i]);
    m_utc_log_start_times.push_back(utc_log_start_time);

    double local_data_start_time = getDataStartTimeFromFile(m_alog_files[i]);
    double utc_data_start_time = utc_log_start_time + local_data_start_time;
    
    double local_data_end_time = getDataEndTimeFromFile(m_alog_files[i]);
    double utc_data_end_time = utc_log_start_time + local_data_end_time;

    if(m_debug) {
      cout << "alog file: " << m_alog_files[i] << endl;
      cout << " " << doubleToString(utc_log_start_time,2) << endl;
      cout << " " << doubleToString(local_data_start_time, 2);
      cout << "  (" << doubleToString(utc_data_start_time, 2) << ")" << endl;
      cout << " " << doubleToString(local_data_end_time, 2);
      cout << "  (" << doubleToString(utc_data_end_time, 2) << ")" << endl;
    }
    
    m_utc_data_start_times.push_back(utc_data_start_time);
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
  

  // Part 5: Sort by End Time
  vector<string> new_alog_files;
  vector<double> new_utc_log_start_times;
  vector<double> new_utc_data_start_times;
  vector<double> new_utc_data_end_times;

  unsigned int amt = m_alog_files.size();
  for(unsigned int i=0; i<amt; i++) {
    unsigned int earliest_end_ix = amt;
    double earliest_utc = 0;

    for(unsigned int j=0; j<amt; j++) {
      if(m_alog_files[j] != "") {
	if((earliest_end_ix == amt) ||
	   (m_utc_data_end_times[j] < earliest_utc)) {
	  earliest_end_ix = j;
	  earliest_utc = m_utc_data_end_times[j];
	}
      }
    }
    new_alog_files.push_back(m_alog_files[earliest_end_ix]);
    new_utc_log_start_times.push_back(m_utc_log_start_times[earliest_end_ix]);
    new_utc_data_start_times.push_back(m_utc_data_start_times[earliest_end_ix]);
    new_utc_data_end_times.push_back(m_utc_data_end_times[earliest_end_ix]);
    m_alog_files[earliest_end_ix] = "";
  }

  m_alog_files = new_alog_files;
  m_utc_log_start_times = new_utc_log_start_times;
  m_utc_data_start_times = new_utc_data_start_times;
  m_utc_data_end_times = new_utc_data_end_times;


  // Part 6
  if(m_debug) {
    for(unsigned int i=0; i<m_alog_files.size(); i++) {
      cout << "new alog file: [" << m_alog_files[i] << "]" << endl;
      cout << "   " << doubleToString(m_utc_log_start_times[i],2) << endl;
      cout << "   " << doubleToString(m_utc_data_start_times[i],2) << endl;
      cout << "   " << doubleToString(m_utc_data_end_times[i],2) << endl;
    }
  }


  return(true);
}


