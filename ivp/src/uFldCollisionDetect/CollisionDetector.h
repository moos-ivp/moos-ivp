/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CollisionDetector.h                                  */
/*    DATE: 21 May 2013                                          */
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

#ifndef COLLISION_DETECTOR_HEADER
#define COLLISION_DETECTOR_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

#include <string>
#include <map>
#include "CPAMonitor.h"
#include "CPAEvent.h"
#include "VarDataPair.h"
#include "LogicCondition.h"
#include "InfoBuffer.h"

class CollisionDetector : public AppCastingMOOSApp
{
 public:
  CollisionDetector();
  ~CollisionDetector() {}
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected: 
  void handleMailNodeReport(std::string);
  bool buildReport();
  void registerVariables();
  void handleCPAEvent(CPAEvent);
  bool handleConfigFlag(std::string, std::string);
  void postFlags(const std::vector<VarDataPair>&, const CPAEvent&);

  std::string expandMacroCD(std::string, std::string v1, std::string v2);

  void postRings();
  
 protected: // Functions in support of logic conditions
  bool updateInfoBuffer(CMOOSMsg&);
  bool checkConditions();
  
 private: // Configuration variables
  double m_near_miss_dist;
  double m_collision_dist;
  double m_encounter_dist;
  
  bool   m_pulse_render;
  double m_pulse_duration;
  double m_pulse_range;

  bool   m_post_closest_range;
  bool   m_post_closest_range_ever;

  bool   m_encounter_rings;
  
  std::vector<VarDataPair> m_collision_flags;
  std::vector<VarDataPair> m_near_miss_flags;
  std::vector<VarDataPair> m_encounter_flags;

  // Set of logic conditions determining when detector is active
  std::vector<LogicCondition> m_logic_conditions;
  
 private: // State variables
  CPAMonitor m_cpa_monitor;
  
  bool m_conditions_ok;

  bool m_report_all_encounters;

  // Each map uses the vehicle name as the key
  std::map<std::string, unsigned int> m_map_vname_collisions;
  std::map<std::string, unsigned int> m_map_vname_near_misses;
  std::map<std::string, unsigned int> m_map_vname_encounters;
  
  unsigned int m_total_collisions;
  unsigned int m_total_near_misses;
  unsigned int m_total_encounters;

  std::string m_param_summary;
  std::vector<std::string> m_notified_vehicles;

  InfoBuffer *m_info_buffer;
};

#endif






