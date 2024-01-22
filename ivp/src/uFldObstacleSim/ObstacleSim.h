/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ObstacleSim.h                                        */
/*    DATE: October 19th, 2017                                   */
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

  bool handleConfigMinDuration(std::string);
  bool handleConfigMaxDuration(std::string);
  bool handleConfigObstacleFile(std::string filename);
  void handleConfigObstacleDurations();

  bool handleMailNodeReport(std::string);
  bool handleMailPointSize(std::string);

  void postObstaclesRefresh();
  void postObstaclesErase();
  void postPoints();

  void updateVRanges();
  void updateObstaclesField();
  void updateObstaclesRefresh();
  
  
 private: // Configuration variables

  // Parameters if we're creating obstacles as we go
  XYPolygon m_poly_region;
  double    m_min_range;
  double    m_min_poly_size;
  double    m_max_poly_size;
  bool      m_reuse_ids;
  
  // Visual params for rendering obstacles
  std::string m_poly_fill_color;
  std::string m_poly_edge_color;
  std::string m_poly_vert_color;
  std::string m_poly_label_color;
  double      m_poly_edge_size;
  double      m_poly_vert_size;
  double      m_poly_transparency;
  
  // Visual params for rendering region
  bool        m_draw_region;
  std::string m_region_edge_color;
  std::string m_region_vert_color;

  // Pseudo LIDAR generation mode
  bool    m_post_points;
  double  m_rate_points;
  double  m_point_size;
  
  // Params for random durations
  double  m_min_duration;
  double  m_max_duration;
  double  m_obs_refresh_interval;

  // Params for resetting the obs field
  double  m_reset_interval;
  double  m_reset_range;

  //range of the sensor for generating point updates
  double m_sensor_range;

  bool   m_post_visuals;

  
private: // State variables

  // Core list of obtacles
  std::vector<XYPolygon> m_obstacles;
  std::vector<double>    m_durations;

  // Maps keyed on vnames. 
  std::map<std::string, NodeRecord>  m_map_vrecords;
  std::map<std::string, double>      m_map_vrange;

  // Maps keyed on obstacle key/label 
  std::map<std::string, unsigned int> m_map_pts_published;
  std::map<std::string, unsigned int> m_map_giv_published;
  
  double       m_reset_tstamp;
  bool         m_reset_request;
  bool         m_reset_pending;
  bool         m_newly_exited;
  bool         m_region_entered;
  unsigned int m_reset_total;
  
  double  m_min_vrange_to_region;
  
  bool    m_obs_refresh_needed;
  double  m_obs_refresh_tstamp;

  unsigned int m_obstacles_made;  
  unsigned int m_obstacles_posted;
};

#endif 

