/*****************************************************************/
/*    NAME: Michael R. Benjamin                                  */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleSim.h                                        */
/*    DATE: October 19th, 2017                                   */
/*****************************************************************/

#ifndef OBSTACLESIM_HEADER
#define OBSTACLESIM_HEADER

#include <string>
#include <vector>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPolygon.h"
#include "NodeRecord.h"
#include "VarDataPair.h"


class ObstacleSim : public AppCastingMOOSApp
{
 public:
  ObstacleSim();
  virtual ~ObstacleSim() {};
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  
 protected:
  void registerVariables();
  bool processObstacleFile(std::string filename);
  void postViewableObstacles();
  void postKnownObstacles();

  void postPoints();
  
 private: // Configuration variables

  // Core list of obtacles
  std::vector<XYPolygon> m_obstacles;

  // Parameters if we're creating obstacles as we go
  XYPolygon m_poly_region;
  double    m_min_range;
  double    m_min_poly_size;
  double    m_max_poly_size;

  // Visual params for rendering obstacles
  std::string m_poly_fill_color;
  std::string m_poly_edge_color;
  std::string m_poly_vert_color;
  std::string m_poly_label_color;

  double    m_poly_edge_size;
  double    m_poly_vert_size;
  double    m_poly_transparency;
  
  // Visual params for rendering region
  bool      m_draw_region;

  // Pseudo LIDAR generation mode
  bool      m_post_points;
  double    m_rate_points;
  
 private: // State variables

  bool m_viewables_queried;
  bool m_obstacles_queried;

  unsigned int m_viewables_posted;
  unsigned int m_obstacles_posted;
};

#endif 
