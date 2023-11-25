/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: EncounterPlot.cpp                                    */
/*    DATE: Jan 11th, 2016                                       */
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "EncounterPlot.h"
#include "LogUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

EncounterPlot::EncounterPlot()
{
  m_min_cpa  = 0; 
  m_max_cpa  = 0; 
  m_min_eff  = 0; 
  m_max_eff  = 0; 
  m_average_cpa = 0;
  m_average_eff = 0;
  m_average_set = false;

  m_collision_range = 5.33;
  m_near_miss_range = 10.33;
  m_encounter_range = 50.33;
}

//---------------------------------------------------------------
// Procedure: addEncounter()

bool EncounterPlot::addEncounter(double time, CPAEvent event)
{
  // Sanity Check: Time must be in ascending order. If new pair
  // doesn't obey, no action is taken, and false is returned.
  unsigned int tsize = m_time.size();
  if((tsize != 0) && (m_time[tsize-1] > time))
    return(false);

  if(m_events.size() == 0)
    m_ownship = event.getVName1();
  else {
    if(m_ownship != event.getVName1())
      m_ownship = "mixed";
  }
  
  m_time.push_back(time);
  m_events.push_back(event);
  
  double cpa = event.getCPA();
  double eff = event.getEFF();

  if((tsize == 0) || (cpa > m_max_cpa))
    m_max_cpa = cpa;
  if((tsize == 0) || (cpa < m_min_cpa))
    m_min_cpa = cpa;
  if((tsize == 0) || (eff > m_max_eff))
    m_max_eff = eff;
  if((tsize == 0) || (eff < m_min_eff))
    m_min_eff = eff;
  
  m_average_set = false;
  return(true);
}

//---------------------------------------------------------------
// Procedure: setCollisionRange()

void EncounterPlot::setCollisionRange(double collision_range)
{
  if(collision_range < 0)
    return;
  m_collision_range = collision_range;

  // Adjust near_miss range if need be. Should be >= near_miss range.
  if(m_near_miss_range < m_collision_range)
    m_near_miss_range = m_collision_range;

  // Adjust encounter range if need be. Should be >= encounter range.
  if(m_encounter_range < m_collision_range)
    m_encounter_range = m_collision_range;
}

//---------------------------------------------------------------
// Procedure: setNearMissRange()

void EncounterPlot::setNearMissRange(double near_miss_range)
{
  if(near_miss_range < 0)
    return;
  m_near_miss_range = near_miss_range;

  // Adjust collision range if need be. Should be <= near_miss range.
  if(m_collision_range > m_near_miss_range)
    m_collision_range = m_near_miss_range;

  // Adjust encounter range if need be. Should be >= near_miss range.
  if(m_encounter_range < m_near_miss_range)
    m_encounter_range = m_near_miss_range;
}

//---------------------------------------------------------------
// Procedure: setEncounterRange()

void EncounterPlot::setEncounterRange(double encounter_range)
{
  if(encounter_range < 0)
    return;
  m_encounter_range = encounter_range;

  // Adjust collision range if need be. Should be <= encounter range.
  if(m_collision_range > m_encounter_range)
    m_collision_range = m_encounter_range;

  // Adjust near_miss range if need be. Should be <= encounter range.
  if(m_near_miss_range > m_encounter_range)
    m_near_miss_range = m_encounter_range;
}

//---------------------------------------------------------------
// Procedure: applySkew

void EncounterPlot::applySkew(double skew)
{
  for(unsigned int i=0; i<m_time.size(); i++)
    m_time[i] += skew;
}
     
//---------------------------------------------------------------
// Procedure: getValueCPAByIndex

double EncounterPlot::getValueCPAByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_events[index].getCPA());
  return(0);
}
     
//---------------------------------------------------------------
// Procedure: getValueEffByIndex

double EncounterPlot::getValueEffByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_events[index].getEFF());
  return(0);
}
     
//---------------------------------------------------------------
// Procedure: getValueIDByIndex

int EncounterPlot::getValueIDByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_events[index].getID());
  return(0);
}
     
//---------------------------------------------------------------
// Procedure: getValueContactByIndex

string EncounterPlot::getValueContactByIndex(unsigned int index) const
{
  if(index < m_time.size()) {
    string vname2 = m_events[index].getVName2();
    if(m_ownship != vname2)
      return(vname2);
    else
      return(m_events[index].getVName1());
  }
  return("unknown");
}
     
//---------------------------------------------------------------
// Procedure: getTimeByIndex

double EncounterPlot::getTimeByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index]);
  return(0);
}
     
//---------------------------------------------------------------
// Procedure: getValueCPAByTime

double EncounterPlot::getValueCPAByTime(double gtime) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(0);

  if(gtime >= m_time[vsize-1])
    return(m_events[vsize-1].getCPA());

  if(gtime <= m_time[0])
    return(m_events[0].getCPA());

  unsigned int index = getIndexByTime(m_time, gtime);
  return(m_events[index].getCPA());
}
     
//---------------------------------------------------------------
// Procedure: getValueEffByTime

double EncounterPlot::getValueEffByTime(double gtime) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(0);

  if(gtime >= m_time[vsize-1])
    return(m_events[vsize-1].getEFF());

  if(gtime <= m_time[0])
    return(m_events[0].getEFF());

  unsigned int index = getIndexByTime(m_time, gtime);
  return(m_events[index].getEFF());
}
     
//---------------------------------------------------------------
// Procedure: getMeanCPA()

double EncounterPlot::getMeanCPA()
{
  if(!m_average_set)
    calcAverages();
  return(m_average_cpa);
}

//---------------------------------------------------------------
// Procedure: getMeanEFF()

double EncounterPlot::getMeanEFF()
{
  if(!m_average_set)
    calcAverages();
  return(m_average_eff);
}

//---------------------------------------------------------------
// Procedure: calcAvgerages()

void EncounterPlot::calcAverages()
{
  // The avg values are stored locally - only calculated it if it
  // has not been calculated already. Subsequent additions to the 
  // log will clear the average values;
  if(m_average_set)
    return;

  double total_cpa = 0;
  double total_eff = 0;
  for(unsigned int i=0; i<m_events.size(); i++) {
    total_cpa += m_events[i].getCPA();
    total_eff += m_events[i].getEFF();
  }

  m_average_cpa = total_cpa / (double)(m_events.size());
  m_average_eff = total_eff / (double)(m_events.size());

  m_average_set = true;
}
     
//---------------------------------------------------------------
// Procedure: getMinTime

double EncounterPlot::getMinTime() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  return(0);
}

//---------------------------------------------------------------
// Procedure: getMaxTime

double EncounterPlot::getMaxTime() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  return(0);
}

//---------------------------------------------------------------
// Procedure: print

void EncounterPlot::print() const
{
  cout << "EncounterPlot::print()" << endl;
  cout << " Ownship: " << m_ownship << endl;

  for(unsigned int i=0; i<m_time.size(); i++) {
    cout << "time:" << m_time[i] << " event:" << m_events[i].getSpec() << endl;
  }

}





