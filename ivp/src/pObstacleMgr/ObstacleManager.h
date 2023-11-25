/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ObstacleManager.h                                    */
/*    DATE: Aug 27th 2014                                        */
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

#ifndef P_OBSTACLE_MANAGER_HEADER
#define P_OBSTACLE_MANAGER_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPolygon.h"
#include "Obstacle.h"
#include "VarDataPair.h"
#include "MailFlagSet.h"
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
  bool handleConfigGivenMaxDuration(std::string);
  bool handleConfigFlag(std::string, std::string);
  bool handleConfigGeneralAlert(std::string);

  bool handleMailNewPoint(std::string);
  bool handleMailAlertRequest(std::string);

  bool handleGivenObstacle(std::string, std::string src="mail");
  
  void postConvexHullUpdates();
  void postConvexHullUpdate(std::string obstacle_key, std::string alert_var,
			    std::string alert_name);
  
  XYPolygon genPseudoHull(const std::vector<XYPoint>& pts, double radius);
  
  XYPolygon placeholderConvexHull(std::string obstacle_key);
  
  XYPoint customStringToPoint(std::string point_str);

  bool updatePointHulls();
  void updatePolyRanges();
  void manageMemory();

  void postFlags(const std::vector<VarDataPair>& flags);
  
  void onNewObstacle(std::string obs_type);
  
private: // Configuration variables
  std::string  m_point_var;            // incoming points

  std::string  m_alert_var;
  std::string  m_alert_name;
  double       m_alert_range;

  // General alert var
  std::string  m_gen_alert_var;
  std::string  m_gen_alert_name;
  double       m_gen_alert_range;

  // When view_polys get numerous, skimp on extras
  unsigned int m_poly_label_thresh;
  unsigned int m_poly_shade_thresh;
  unsigned int m_poly_vertex_thresh;
  bool m_poly_label_thresh_over;
  bool m_poly_shade_thresh_over;
  bool m_poly_vertex_thresh_over;
  
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

  double       m_given_max_duration;

  std::vector<VarDataPair> m_new_obs_flags;

  MailFlagSet m_mfset;
  
  
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

  unsigned int m_given_mail_ever;
  unsigned int m_given_mail_good;
  unsigned int m_given_config_ever;

  unsigned int m_obstacles_ever;
  
  std::map<std::string, Obstacle> m_map_obstacles;
};

#endif 

