/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PathCheck_MOOSApp.h                                   */
/*   DATE: Nov 22nd 2011                                        */
/****************************************************************/

#ifndef PATH_CHECK_MOOS_APP_HEADER
#define PATH_CHECK_MOOS_APP_HEADER

#include <map>
#include <list>
#include "MOOSLib.h"
#include "NodeRecord.h"

class PathCheck_MOOSApp : public CMOOSApp
{
 public:
  PathCheck_MOOSApp();
  virtual ~PathCheck_MOOSApp();

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

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
  std::map<std::string, bool>   m_map_changed_dist;
  
  unsigned int m_history_size;

};

#endif 
