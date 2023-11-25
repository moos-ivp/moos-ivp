/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PathCheck_MOOSApp.h                                  */
/*    DATE: Nov 22nd 2011                                        */
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

#ifndef PATH_CHECK_MOOS_APP_HEADER
#define PATH_CHECK_MOOS_APP_HEADER

#include <map>
#include <list>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeRecord.h"

class PathCheck_MOOSApp : public AppCastingMOOSApp
{
 public:
  PathCheck_MOOSApp();
  virtual ~PathCheck_MOOSApp() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

 protected:
  bool addNodeRecord(const NodeRecord&);
  
  void registerVariables();
  void computeAndPostSpeeds();
  void detectAndPostOdometry();
  bool handleTripReset(std::string);

  // All below maps are VehicleName-->DATA
  
  std::map<std::string, std::list<NodeRecord> > m_map_records;

  // Odometry: Overall total distance travelled - never reset
  std::map<std::string, double>   m_map_total_dist;
  // Odometry: Recent tripometer distance travelled - may be reset
  std::map<std::string, double>   m_map_trip_dist;
  // Odometry: true if distance updated since last publication
  std::map<std::string, bool>     m_map_changed_dist;
  // Timestamp of last nodereport received
  std::map<std::string, double>   m_map_noderep_tstamp;

  
  unsigned int m_history_size;

  unsigned int m_node_reports_recd_good;
  unsigned int m_node_reports_recd_bad;
};

#endif 









