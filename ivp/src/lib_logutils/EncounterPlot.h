/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: EncounterPlot.h                                      */
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

#ifndef ENCOUNTER_PLOT_HEADER
#define ENCOUNTER_PLOT_HEADER

#include <string>
#include <vector>
#include "CPAEvent.h"

class EncounterPlot
{
public:
  EncounterPlot();
  ~EncounterPlot() {}
  
 public: // Setting
  bool   addEncounter(double time, CPAEvent event);
  void   setCollisionRange(double);
  void   setNearMissRange(double);
  void   setEncounterRange(double);
  
 public: // Modification
  void   applySkew(double skew);

 public: // Querying
  double getTimeByIndex(unsigned int index) const;

  double getValueCPAByIndex(unsigned int index) const;
  double getValueEffByIndex(unsigned int index) const;
  int    getValueIDByIndex(unsigned int index) const;
  std::string getValueContactByIndex(unsigned int index) const;
  
  double getValueCPAByTime(double gtime) const;
  double getValueEffByTime(double gtime) const;

  double getMinTime() const;
  double getMaxTime() const;

  double getMeanCPA();
  double getMeanEFF();

  double getMinCPA() const         {return(m_min_cpa);}
  double getMinEFF() const         {return(m_min_eff);}
  double getMaxCPA() const         {return(m_max_cpa);}
  double getMaxEFF() const         {return(m_max_eff);}

  double getCollisionRange() const {return(m_collision_range);}
  double getNearMissRange() const  {return(m_near_miss_range);}
  double getEncounterRange() const {return(m_encounter_range);}
  
  std::string getOwnship() const   {return(m_ownship);}
  unsigned int  size() const       {return(m_time.size());}

  bool   empty() const             {return(m_time.size()==0);}
  void   print() const;

 protected:
  void   calcAverages();
  
protected:
  std::string m_ownship;

  std::vector<double>   m_time;
  std::vector<CPAEvent> m_events;

  double m_min_cpa;
  double m_min_eff;
  double m_max_cpa;
  double m_max_eff;

  double m_collision_range;
  double m_near_miss_range;
  double m_encounter_range;
  
  bool   m_average_set;
  double m_average_cpa;
  double m_average_eff;
};
#endif 


















