/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
}


//--------------------------------------------------------
// Procedure: handle

bool CollisionReporter::handle(const string& alogfile)
{
  if(alogfile == "") {
    cout << "Alog file was not specified. Exiting now." << endl;
    return(false);
  }

  FILE *file_ptr = fopen(alogfile.c_str(), "r");
  if(!file_ptr) {
    cout << "Alog file not found or unable to open - exiting" << endl;
    return(false);
  }
  
  cout << "Analysing collision encounters in file : " << alogfile << endl;

  unsigned int line_count  = 0;
  bool done = false;
  while(!done) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    if((line_count % 10000) == 0)
      cout << "+" << flush;
    if((line_count % 100000) == 0)
      cout << " (" << uintToCommaString(line_count) << ") lines" << endl;

    bool line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    
    if(!done && !line_is_comment) {
      string varname = getVarName(line_raw);
      string data = getDataEntry(line_raw);
      if(varname == "ENCOUNTER") { 
	double cpa = atof(data.c_str());
	m_encounters++;
	m_total_encounter_cpa += cpa;
      }
      else if(varname == "NEAR_MISS") { 
	double cpa = atof(data.c_str());
	m_near_misses++;
	m_total_near_miss_cpa += cpa;
      }
      else if(varname == "COLLISION") { 
	double cpa = atof(data.c_str());
	if((m_collisions == 0) || (cpa < m_collision_worst))
	  m_collision_worst = cpa;
	m_collisions++;
	m_total_collision_cpa += cpa;
      }
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

  cout << endl << endl;
  cout << "=========================================" << endl;
  cout << "Collision Report:                        " << endl;
  cout << "=========================================" << endl;
  cout << "Encounters:  " << m_encounters  << "  (avg " << eavg << " m)" << endl;
  cout << "Near Misses: " << m_near_misses << "  (avg " << navg << " m)" << endl;
  cout << "Collisions:  " << m_collisions  << "  (avg " << cavg << " m)" << endl;

  if(m_collisions > 0)
    cout << "Collision Worst: " << m_collision_worst << "m" << endl;

}



