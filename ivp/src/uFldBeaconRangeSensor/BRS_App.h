/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BRS_App.h                                            */
/*    DATE: Feb 1st, 2011                                        */
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

#ifndef UFLD_BEACON_RANGE_SENSOR_APP_HEADER
#define UFLD_BEACON_RANGE_SENSOR_APP_HEADER

#include <vector>
#include <string>
#include <list>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "BeaconBuoy.h"
#include "NodeRecord.h"

class BRS_App : public AppCastingMOOSApp
{
 public:
  BRS_App();
  virtual ~BRS_App() {}

 public: // Standard MOOSApp functions to overload
  bool   OnNewMail(MOOSMSG_LIST &NewMail);
  bool   OnStartUp();
  bool   Iterate();
  bool   OnConnectToServer();

 protected: // Standard AppCastingMOOSApp function to overload
  bool   buildReport();

 protected: 
  void   registerVariables();
  void   postVisuals();

 protected: // Configuration utility
  bool   addBeaconBuoy(const std::string&);

 protected: // Incoming mail utility
  bool   handleNodeReport(const std::string&);
  bool   handleRangeRequest(const std::string&);

 protected: // Outgoing mail utility
  void   handleBeaconReport(unsigned int bix);
  void   postBeaconRangeReport(unsigned int bix, double rng, 
				std::string rec_node);
  void   postBeaconRangePulse(unsigned int bix, std::string color="");

  void   postRangePulse(double x, double y, std::string color,
			 std::string label, double dur, double radius);
 protected: // Utilities
  double getTrueBeaconNodeRange(unsigned int bix, std::string);
  double getNoisyBeaconNodeRange(double) const;
  void   updateNodeRecords(NodeRecord);
  bool   setPingWait(std::string);
  bool   setPingPayments(std::string);
  bool   setReportVars(std::string);
  bool   setRandomNoiseAlgorithm(std::string);
  bool   setNodePushDistance(std::string);
  bool   setNodePullDistance(std::string);

  bool   pingTrans(double push, double pull, double actual);

 protected: // State variables
  // Vector of N beacons 
  std::vector<BeaconBuoy>  m_beacons;

  // Map is keyed on the name of the vehicle
  std::map<std::string, NodeRecord>   m_map_node_records;
  std::map<std::string, unsigned int> m_map_node_reps_recd;
  std::map<std::string, unsigned int> m_map_node_pings_usol;
  std::map<std::string, unsigned int> m_map_node_pings_gend;
  std::map<std::string, unsigned int> m_map_node_pings_repl;
  std::map<std::string, unsigned int> m_map_node_xping_freq;
  std::map<std::string, unsigned int> m_map_node_xping_dist;
  std::map<std::string, double>       m_map_node_last_ping;

  std::list<std::string> m_event_messages;

 protected: // Configuration variables
  double      m_default_node_push_dist;  
  double      m_default_node_pull_dist;  
  double      m_default_node_ping_wait;

  // Map is keyed on the name of the vehicle
  std::map<std::string, double> m_map_node_push_dist;
  std::map<std::string, double> m_map_node_pull_dist;
  std::map<std::string, double> m_map_node_ping_wait;
  
  BeaconBuoy  m_default_beacon_buoy;

  std::string m_ping_payments;

  std::string m_report_vars;
  bool        m_ground_truth;

  std::string m_rn_algorithm;   // Empty string = no random noise
  double      m_rn_uniform_pct;

  bool        m_verbose;
};

#endif 










