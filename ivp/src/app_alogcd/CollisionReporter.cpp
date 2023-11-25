/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CollisionReporter.cpp                                */
/*    DATE: December 23rd, 2015                                  */
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
#include "CollisionReporter.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Constructor

CollisionReporter::CollisionReporter()
{
  m_encounters  = 0;;
  m_near_misses = 0;;
  m_collisions  = 0;
  
  m_total_encounter_cpa = 0;
  m_total_near_miss_cpa = 0;
  m_total_collision_cpa = 0;

  m_collision_worst = 0;
  m_terse = false;
}


//--------------------------------------------------------
// Procedure: setTimeStampFile()

bool CollisionReporter::setTimeStampFile(string tstamp_file)
{
  if((tstamp_file != "") && !okFileToWrite(tstamp_file)) {
    cout << "TimeStamp file [" << tstamp_file << "] unable to write." << endl;
    return(false);
  }

  m_time_stamp_file = tstamp_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: setALogFile()

bool CollisionReporter::setALogFile(string alog_file)
{
  if(!okFileToRead(alog_file)) {
    cout << "Alog file [" << alog_file << "] unable to read." << endl;
    return(false);
  }
  
  m_alog_file = alog_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: handle

bool CollisionReporter::handle()
{
  FILE *alog_file_ptr = fopen(m_alog_file.c_str(), "r");
  if(!alog_file_ptr )
    return(false);

  FILE *time_file_ptr = 0;
  if(m_time_stamp_file != "")
    time_file_ptr = fopen(m_time_stamp_file.c_str(), "w+");

  if(!m_terse)
    cout << "Analyzing collision encounters in file : " << m_alog_file << endl;

  unsigned int line_count  = 0;
  bool done = false;
  while(!done) {
    line_count++;
    string line_raw = getNextRawLine(alog_file_ptr);

    if(!m_terse) {
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
    if((var!="ENCOUNTER") && (var!="NEAR_MISS") && (var!="COLLISION") &&
       (var!="ENCOUNTER_SUMMARY")) 
     continue;
	
    string val = getDataEntry(line_raw);
    double cpa = atof(val.c_str());
    
    if(time_file_ptr && ((var=="NEAR_MISS") || (var=="COLLISION"))) {
      string time = getTimeStamp(line_raw);
      string cpa_str = doubleToString(cpa,2);
      fprintf(time_file_ptr, "%s,%s,%s\n", time.c_str(),
	      var.c_str(), cpa_str.c_str());
    }
    
    if((var == "ENCOUNTER") || (var == "ENCOUNTER_SUMMARY")) {
      m_encounters++;
      m_total_encounter_cpa += cpa;
    }
    else if(var == "NEAR_MISS") { 
      m_near_misses++;
      m_total_near_miss_cpa += cpa;
    }
    else if(var == "COLLISION") { 
      if((m_collisions == 0) || (cpa < m_collision_worst))
	m_collision_worst = cpa;
      m_collisions++;
      m_total_collision_cpa += cpa;
    }
  }
  if(!m_terse) {
    string line_count_str = uintToCommaString(line_count);
    cout << endl << line_count_str << " total alog file lines." << endl;
    if(m_time_stamp_file != "")
      cout << "tstamp_file: " << m_time_stamp_file << endl;
  }
  
  if(alog_file_ptr)
    fclose(alog_file_ptr);
  if(time_file_ptr)
    fclose(time_file_ptr);

  return(true);
}

//--------------------------------------------------------
// Procedure: printReport
//     Notes: 

void CollisionReporter::printReport()
{
  double encounter_avg = 0;
  double near_miss_avg = 0;
  double collision_avg = 0;
  
  if(m_encounters > 0)
    encounter_avg = m_total_encounter_cpa / (double)(m_encounters);
  if(m_near_misses > 0)
    near_miss_avg = m_total_near_miss_cpa / (double)(m_near_misses);
  if(m_collisions > 0)
    collision_avg = m_total_collision_cpa / (double)(m_collisions);

  string eavg = doubleToString(encounter_avg, 2);
  string navg = doubleToString(near_miss_avg, 2);
  string cavg = doubleToString(collision_avg, 2);

  if(m_terse) {
    cout << m_encounters << "/" << m_near_misses << "/" << m_collisions << endl;
    return;
  }
  
  cout << endl;
  cout << "=========================================" << endl;
  cout << "Collision Report:                        " << endl;
  cout << "=========================================" << endl;
  cout << "Encounters:  " << m_encounters  << "  (avg " << eavg << " m)" << endl;
  cout << "Near Misses: " << m_near_misses << "  (avg " << navg << " m)" << endl;
  cout << "Collisions:  " << m_collisions  << "  (avg " << cavg << " m)" << endl;

  if(m_collisions > 0)
    cout << "Collision Worst: " << doubleToString(m_collision_worst,2) << endl;
}








