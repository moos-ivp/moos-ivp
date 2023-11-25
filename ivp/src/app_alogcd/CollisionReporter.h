/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CollisionReporter.h                                  */
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

#ifndef COLLISION_REPORTER_HEADER
#define COLLISION_REPORTER_HEADER

#include <string>

class CollisionReporter
{
 public:
  CollisionReporter();
  ~CollisionReporter() {}

  bool setTimeStampFile(std::string);
  bool setALogFile(std::string);
  void setTerse() {m_terse=true;}
  
  bool handle();
  void printReport();

  bool hadCollisions() {return(m_collisions > 0);}
  bool hadEncounters() {return(m_encounters > 0);}
  
 protected:
  unsigned int m_encounters;
  unsigned int m_near_misses;
  unsigned int m_collisions;
  
  double m_total_encounter_cpa;
  double m_total_near_miss_cpa;
  double m_total_collision_cpa;
  
  double m_collision_worst;

  std::string m_time_stamp_file;
  std::string m_alog_file;

  bool   m_terse;
};

#endif










