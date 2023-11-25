/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MHashReporter.cpp                                    */
/*    DATE: May 12th, 2023                                       */
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
#include "MHashReporter.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Constructor()

MHashReporter::MHashReporter()
{
  // Init config vars
  m_terse  = false;
  m_report = "full";

  // Init state vars
  m_osx = 0;
  m_osy = 0;
  m_osx_virgin = true;
  m_osy_virgin = true;
  m_osx_prev = 0;
  m_osy_prev = 0;

  m_utc_start_time = 0;
  
  m_prev_time = 0;
  m_curr_time = 0;
  m_odometry  = 0;
}


//--------------------------------------------------------
// Procedure: setALogFile()

bool MHashReporter::setALogFile(string alog_file)
{
  if(!okFileToRead(alog_file)) {
    cout << "Alog file [" << alog_file << "] unable to read." << endl;
    return(false);
  }
  
  m_alog_file = alog_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: handle()

bool MHashReporter::handle()
{
  if(m_mhash_in != "") {
    reportXHash();
    return(true);
  }

  // ========================================================
  // Part 1: Alog file sanity check
  // ========================================================
  if(m_alog_file == "") {
    cout << "No alog file provided. Exiting. " << endl;
    return(false);
  }

  m_utc_start_time = getLogStartFromFile(m_alog_file);
  
  FILE *alog_file_ptr = fopen(m_alog_file.c_str(), "r");
  if(!alog_file_ptr )
    return(false);

  if(m_report == "full")
    cout << "Analyzing odometry in file : " << m_alog_file << endl;

  // ========================================================
  // Part 2: Parse the Alog file
  // ========================================================
  unsigned int line_count  = 0;
  bool done = false;
  while(!done) {
    line_count++;
    string line_raw = getNextRawLine(alog_file_ptr);

    if(m_report == "full") {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 250000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }
    
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    if(line_raw == "eof") 
      break;
    
    string var = getVarName(line_raw);
    if((var!="NAV_X") && (var!="DB_UPTIME") &&
       (var!="NAV_Y") && (var!="MISSION_HASH"))
     continue;
	
    string sval  = getDataEntry(line_raw);
    string stime = getTimeStamp(line_raw);
    if((m_vname == "") && (var == "DB_UPTIME")) {
      string src = getSourceName(line_raw);
#if 0
      m_vname = rbiteString(src, '_');
#endif
#if 1
      biteString(src, '_');
      m_vname = src;
#endif
      if(m_vname == "shoreside")
	m_vname = "shore";
    }
    
    if(var == "NAV_X") {
      m_osx = atof(sval.c_str());
      if(m_osx_virgin) {
	m_osx_prev = m_osx;
	m_osx_virgin = false;
      }
    }
    else if(var == "NAV_Y") {
      m_osy = atof(sval.c_str());
      if(m_osy_virgin) {
	m_osy_prev = m_osy;
	m_osy_virgin = false;
      }
    }
    else if(var == "MISSION_HASH") 
      m_curr_mhash = sval;

    m_curr_time = atof(stime.c_str());
    double elapsed = m_curr_time - m_prev_time;
    
    if((elapsed > 0.5) && !m_osx_virgin && !m_osy_virgin) {
      double delta_x = m_osx - m_osx_prev;
      double delta_y = m_osy - m_osy_prev;
      double dist = hypot(delta_x, delta_y);
      m_odometry += dist;
      m_osx_prev  = m_osx;
      m_osy_prev  = m_osy;
      m_prev_time = m_curr_time;
      if(m_curr_mhash != "")
	m_hash_odo[m_curr_mhash] += dist;
    }
  }

  // ========================================================
  // Part 3: Reporting results
  // ========================================================
  if(m_report == "full") {
    string line_count_str = uintToCommaString(line_count);
    cout << endl << line_count_str << " total alog file lines." << endl;
  }
  
  if(alog_file_ptr)
    fclose(alog_file_ptr);

  if(m_report == "odist")
    reportODist();
  else if(m_report == "mhash")
    reportMHash();
  else if(m_report == "duration")
    reportDuration();
  else if(m_report == "name")
    reportName();
  else if(m_report == "utc")
    reportUTC();
  else if(m_report == "xhash")
    reportXHash();
  else if(m_report == "all")
    reportAll();
  else
    reportVerbose();
  
  return(true);
}

//--------------------------------------------------------
// Procedure: reportVerbose()
//      Note: Normally not run as part of a script, but has
//            user friendly summary output.

void MHashReporter::reportVerbose()
{
  string full_mhash = getMaxOdoMHash();
  string mhash = tokStringParse(full_mhash, "mhash");
  string utc   = tokStringParse(full_mhash, "utc");

  cout << "-------------------------------------" << endl;
  cout << "MHash:     " << mhash << endl;
  cout << "Odometry:  " << doubleToStringX(m_odometry,1) << endl;
  cout << "Duration:  " << doubleToStringX(m_curr_time,1) << endl;
  cout << "Node Name: " << m_vname << endl;
  cout << "UTC:       " << utc << endl;
  cout << "Full List of MHashes noted: " << endl;
  map<string,double>::iterator p;
  for(p=m_hash_odo.begin(); p!=m_hash_odo.end(); p++) {
    string mhash = p->first;
    double odist = p->second;
    string hash=tokStringParse(mhash, "mhash");

    string odostr = doubleToString(odist,2);
    cout << " [" << hash << "]: " << odostr << " meters" << endl;
  }
}

//--------------------------------------------------------
// Procedure: reportMHash()
//      Note: If multiple MHASH vals detected, report the one
//            MHASH that was associated with the longest
//            duration in terms of odometry distance.

void MHashReporter::reportMHash()
{
  string full_mhash = getMaxOdoMHash();
  string mhash = tokStringParse(full_mhash, "mhash");

  cout << mhash << flush;
  if(!m_terse)
    cout << endl;
}

//--------------------------------------------------------
// Procedure: reportODist()

void MHashReporter::reportODist()
{
  cout << doubleToStringX(m_odometry,1) << flush;
  if(!m_terse)
    cout << endl;
}
  
//--------------------------------------------------------
// Procedure: reportDuration()
//      Note: The value of the final timestamp of the last
//            entry in the alog file. Used as a way of gauging
//            whether this alog file represents a valid run,
//            or perhaps was a false start short alog file.

void MHashReporter::reportDuration()
{
  cout << doubleToStringX(m_curr_time,1) << flush;
  if(!m_terse)
    cout << endl;
}
  
//--------------------------------------------------------
// Procedure: reportName()
//      Note: Typically a vehicle name or "shoreside". Is
//            identical to the name of the MOOS community.

void MHashReporter::reportName()
{
  cout << m_vname << flush;
  if(!m_terse)
    cout << endl;
}
  
//--------------------------------------------------------
// Procedure: reportUTC()
//      Note: Represents that starting time for an alog file
//            in UTC time, given by the LOGSTART header in
//            the top of the alog file.

void MHashReporter::reportUTC()
{
  cout << doubleToStringX(m_utc_start_time,3) << flush;
  if(!m_terse)
    cout << endl;
}
  
//--------------------------------------------------------
// Procedure: reportXHash()

void MHashReporter::reportXHash() const
{
  cout << getXHash() << flush;
  if(!m_terse)
    cout << endl;
}
  
//--------------------------------------------------------
// Procedure: reportAll()
//      Note: Important to preserve ordering since .mhash
//            files will be created with the output of this
//            function verbatim.

void MHashReporter::reportAll()
{
  string full_mhash = getMaxOdoMHash();
  string mhash = tokStringParse(full_mhash, "mhash");
  string utc   = tokStringParse(full_mhash, "utc");
  string xhash = getXHash();

  // Strip the decimal part of the utc if it has it
  utc = biteString(utc, '.');
  
  string report = "mhash=" + mhash;
  report += ",odo=" + doubleToString(m_odometry,1);
  report += ",duration=" + doubleToString(m_curr_time,1);
  report += ",name=" + m_vname;
  report += ",utc=" + utc;
  report += ",xhash=" + xhash;
  
  cout << report << flush;
  if(!m_terse)
    cout << endl;
}
  
//--------------------------------------------------------
// Procedure: getMaxOdoMHash()
//   Purpose: In the case where an alog file has multiple
//            MHASH values, return the MHASH that was
//            prevailing for the longest odometry dist.
//      Note: Shoreside alog files won't have odometry, but
//            they also are very unlikely to have multiple
//            MHASH values either since an MHASH is typically
//            created once upon the launch of shoreside.

string MHashReporter::getMaxOdoMHash() const
{
  // Longest mhash will be the most recent if there are not mhashes
  // tied to odometry distance, e.g., in shoreside missions.
  
  double longest_odist = -1;
  string longest_mhash = m_curr_mhash;

  map<string,double>::const_iterator p;
  for(p=m_hash_odo.begin(); p!=m_hash_odo.end(); p++) {
    string mhash = p->first;
    double odist = p->second;

    if(odist > longest_odist) {
      longest_mhash = mhash;
      longest_odist = odist;
    }
  }  

  return(longest_mhash);  
}
 
//--------------------------------------------------------
// Procedure: getXHash()
//   Example: odo=1507.5
//            duration=451.3,
//            name=abe
//   Returns: ABE-451S-999M

string MHashReporter::getXHash() const
{
  // Handle Duration
  double duration = m_curr_time;
  if(duration > 999)
    duration = 999;
  
  string dur_str = doubleToString(m_curr_time,0);
  if(dur_str.length() == 1)
    dur_str = "00" + dur_str;
  else if(dur_str.length() == 2)
    dur_str = "0" + dur_str;

  // Handle Odometry
  double odometry = m_odometry;
  if(odometry > 999)
    odometry = 999;
  string odo_str = doubleToString(odometry,0);
  if(odo_str.length() == 1)
    odo_str = "00" + odo_str;
  else if(odo_str.length() == 2)
    odo_str = "0" + odo_str;

  string xhash = toupper(m_vname) + "-" + dur_str + "S-" + odo_str + "M";
  return(xhash);
}

