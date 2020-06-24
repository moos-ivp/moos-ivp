/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleManager.h                                    */
/*    DATE: Aug 27th 2014                                        */
/*****************************************************************/

#ifndef P_OBSTACLE_MANAGER_HEADER
#define P_OBSTACLE_MANAGER_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPolygon.h"
#include "Obstacle.h"
#include <set>

class ObstacleManager : public AppCastingMOOSApp
{
public:
  ObstacleManager();
  ~ObstacleManager() {};
  
protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  
protected:
  void registerVariables();

  bool handleConfigPostDistToPolys(std::string);

  bool handleMailNewPoint(std::string);
  bool handleMailAlertRequest(std::string);

  bool handleGivenObstacle(std::string);
  
  void postConvexHullUpdates();
  void postConvexHullUpdate(std::string obstacle_key);
  
  XYPolygon genPseudoHull(const std::vector<XYPoint>& pts, double radius);
  
  XYPolygon placeholderConvexHull(std::string obstacle_key);
  
  XYPoint customStringToPoint(std::string point_str);

  bool updatePointHulls();
  void updatePolyRanges();
  void manageMemory();
  
  
private: // Configuration variables
  std::string  m_point_var;            // incoming points

  std::string  m_alert_var;
  std::string  m_alert_name;
  double       m_alert_range;

  // Managing incoming points
  double       m_ignore_range;
  unsigned int m_max_pts_per_cluster;
  double       m_max_age_per_point;

  // Configuring Lasso option
  bool         m_lasso;
  unsigned int m_lasso_points;
  double       m_lasso_radius;

  std::string  m_post_dist_to_polys;
  bool         m_post_view_polys;
  
  std::string  m_obstacles_color;
  
private: // State variables
  double m_nav_x;
  double m_nav_y;
  
  double m_min_dist_ever;
  
  unsigned int  m_points_total;
  unsigned int  m_points_invalid;
  unsigned int  m_points_ignored;
  unsigned int  m_obstacles_released;

  unsigned int  m_alerts_posted;
  unsigned int  m_alerts_resolved;

  std::map<std::string, Obstacle> m_map_obstacles;
};

#endif 
