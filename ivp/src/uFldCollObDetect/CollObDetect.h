/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CollObDetect.h                                       */
/*    DATE: September 2nd, 2019                                  */
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

#ifndef COLL_OB_DETECT_HEADER
#define COLL_OB_DETECT_HEADER

#include <string>
#include <vector>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPolygon.h"
#include "NodeRecord.h"
#include "VarDataPair.h"


class CollObDetect : public AppCastingMOOSApp
{
 public:
  CollObDetect();
  virtual ~CollObDetect() {};
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  
 protected:
  void registerVariables();

  bool handleMailKnownObstacle(std::string);
  void handleMailKnownObstacleClear(std::string);
  bool handleMailNodeReport(std::string);

  void updateVehiDists();
  void updateVehiMinDists();
  void updateVehiEncounters();
  
  void postFlags(const std::vector<VarDataPair>&,
		 double min_dist,
		 std::string vname,
		 std::string label);

  void setCurrDist(std::string vname, std::string lab, double dist);
  void setPrevDist(std::string vname, std::string lab, double dist);
  void setMinDist(std::string vname, std::string lab, double dist);

  double getCurrDist(std::string vname, std::string label);
  double getPrevDist(std::string vname, std::string label);
  double getMinDist(std::string vname, std::string label);

  void postEncounterBin(double dist);
  void binEncounter(double dist);
  
  
 private: // Configuration variables

  // Core map of obtacles and time received
  std::map<std::string, XYPolygon> m_map_obstacles;
  std::map<std::string, double>    m_map_ob_tstamp;

  // Params defining test/success
  double m_near_miss_dist;
  double m_collision_dist;
  double m_encounter_dist;

  std::vector<VarDataPair> m_collision_flags;
  std::vector<VarDataPair> m_near_miss_flags;
  std::vector<VarDataPair> m_encounter_flags;
  
 private: // State variables

  std::map<std::string, NodeRecord> m_map_vrecords;

  // For each vehicle and each obstacle
  std::map<std::string, std::map<std::string, double> > m_map_vdist;
  std::map<std::string, std::map<std::string, double> > m_map_vdist_prev;
  std::map<std::string, std::map<std::string, double> > m_map_vdist_min;

  unsigned int m_total_encounters;
  unsigned int m_total_near_misses;
  unsigned int m_total_collisions;

  double m_bin_minval;
  double m_bin_delta;
  std::map<double, unsigned int> m_map_bins;  // totals in bin
  std::map<double, unsigned int> m_map_tbins; // totals in bin and below
  
  double m_global_min_dist;
};

#endif 

