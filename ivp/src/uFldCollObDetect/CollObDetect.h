/*****************************************************************/
/*    NAME: Michael R. Benjamin                                  */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CollObDetect.h                                       */
/*    DATE: Septtember 2nd, 2019                                 */
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

  bool handleConfigFlag(std::string, std::string);
  bool handleMailKnownObstacle(std::string);
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

  
 private: // Configuration variables

  // Core list of obtacles
  std::map<std::string, XYPolygon> m_map_obstacles;

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

  double m_global_min_dist;
};

#endif 
