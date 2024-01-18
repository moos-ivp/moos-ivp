/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ObstacleManager.cpp                                  */
/*    DATE: Aug 27th 2014 For RobotX                             */
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

#include <iterator>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "MacroUtils.h"
#include "ACTable.h"
#include "ObstacleManager.h"
#include "ConvexHullGenerator.h"
#include "XYFormatUtilsPoint.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

ObstacleManager::ObstacleManager()
{
  // Init Config Variables
  m_alert_range  = 20;  // meters
  m_ignore_range = -1;  // meters (neg value means off)
  m_gen_alert_range = -1;
  
  m_max_pts_per_cluster = 20;
  m_max_age_per_point   = 20;

  m_poly_label_thresh = 25;
  m_poly_shade_thresh = 100;
  m_poly_vertex_thresh = 150;
  m_poly_label_thresh_over = false;
  m_poly_shade_thresh_over = false;
  m_poly_vertex_thresh_over = false;
  
  m_lasso = false;
  m_lasso_points = 6;
  m_lasso_radius = 5;  // meters

  // Init info output variables
  m_post_dist_to_polys = "close";
  m_post_view_polys = true;

  m_obstacles_color = "blue";

  m_given_max_duration = 60; // seconds
  
  // Init State Variables
  m_nav_x = 0;
  m_nav_y = 0;

  // Init state variables
  m_points_total   = 0;
  m_points_ignored = 0;
  m_points_invalid = 0;
  m_obstacles_released = 0;
  m_obstacles_ever = 0;

  m_given_mail_ever = 0;
  m_given_mail_good = 0;
  m_given_config_ever = 0;
  
  m_min_dist_ever = -1;
  
  m_alerts_posted = 0;
  m_alerts_resolved = 0;

  m_alert_var = "";    // Initially no alerts will be posted
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool ObstacleManager::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 
    double dval   = msg.GetDouble();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    // Consider mail handled by default if handled by mail_flag_set
    bool handled = m_mfset.handleMail(key, m_curr_time);
    
    if(key == m_point_var)
      handled = handleMailNewPoint(sval);
    if(key == "NAV_X") {
      m_nav_x = dval;
      handled = true;
    }
    else if(key == "NAV_Y") {
      m_nav_y = dval;
      handled = true;
    }
    else if(key == "GIVEN_OBSTACLE") 
      handled = handleGivenObstacle(sval);
    else if(key == "OBM_ALERT_REQUEST") 
      handled = handleMailAlertRequest(sval);
    else if(key == "APPCAST_REQ") // handle by AppCastingMOOSApp
      handled = true;

    if(!handled)
      reportRunWarning("Unhandled Mail: " + key);
    
  }

  // After MailFlagSet has handled all mail from this iteration,
  // post any flags that result. Flags will be cleared in m_mfset.
  postFlags(m_mfset.getNewFlags());
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool ObstacleManager::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool ObstacleManager::Iterate()
{
  AppCastingMOOSApp::Iterate();

  manageMemory();
  updatePointHulls();
  updatePolyRanges();
  postConvexHullUpdates();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ObstacleManager::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "point_var")
      handled = setNonWhiteVarOnString(m_point_var, value);
    else if((param == "given_obstable") || (param == "given_obstacle"))
      handled = handleGivenObstacle(value, "mission");
    else if(param == "alert_range")
      handled = setPosDoubleOnString(m_alert_range, value);
    else if(param == "ignore_range")
      handled = setPosDoubleOnString(m_ignore_range, value);
    else if(param == "max_pts_per_cluster")
      handled = setUIntOnString(m_max_pts_per_cluster, value);
    else if(param == "max_age_per_point")
      handled = setPosDoubleOnString(m_max_age_per_point, value);
    else if(param == "post_dist_to_polys")
      handled = handleConfigPostDistToPolys(value);
    else if(param == "post_view_polys")
      handled = setBooleanOnString(m_post_view_polys, value);
    else if(param == "obstacles_color")
      handled = setColorOnString(m_obstacles_color, value);
    else if(param == "poly_label_thresh")
      handled = setUIntOnString(m_poly_label_thresh, value);
    else if(param == "poly_shade_thresh")
      handled = setUIntOnString(m_poly_shade_thresh, value);
    else if(param == "poly_vertex_thresh")
      handled = setUIntOnString(m_poly_vertex_thresh, value);
    else if(param == "given_max_duration")
      handled = handleConfigGivenMaxDuration(value);
    else if(param == "general_alert")
      handled = handleConfigGeneralAlert(value);

    else if(param == "new_obs_flag") 
      handled = handleConfigFlag("new_obs", value);

    else if(param == "mailflag") 
      handled = m_mfset.addFlag(value);

    else if(param == "lasso")
      handled = setBooleanOnString(m_lasso, value);

    else if(param == "lasso_points") {
      handled = setUIntOnString(m_lasso_points, value);
      if(m_lasso_points < 3) {
	reportConfigWarning("lasso_points must be at least 3 points");
	m_lasso_points = 3;
      }
    }
    else if(param == "lasso_radius") {
      handled = setDoubleOnString(m_lasso_radius, value); 
      if(m_lasso_radius <= 0) {
	reportConfigWarning("lasso_radius must be at positive number");
	m_lasso_radius = 1;
      }
    }      
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  // We left the m_point_var unset in the constructor because we don't
  // want to unnecessarily register for a variable that we don't
  // actually need
  if(m_point_var == "")
    m_point_var = "TRACKED_FEATURE";

  Notify("OBM_CONNECT", "true");
  reportEvent("OBM_CONNECT=true");
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void ObstacleManager::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  if(m_point_var != "")
    Register(m_point_var, 0);

  Register("NAV_X", 0);
  Register("NAV_Y", 0);

  Register("GIVEN_OBSTACLE",0);
  Register("OBM_ALERT_REQUEST",0);

  // Register for any variables involved in the MailFlagSet
  vector<string> mflag_vars = m_mfset.getMailFlagKeys();
  for(unsigned int i=0; i<mflag_vars.size(); i++)
    Register(mflag_vars[i], 0);

}


//------------------------------------------------------------
// Procedure: customStringToPoint()
//   Purpose: Parses strings representing tracked features. The format
//            currently is consistent with an XYPoint, but we custom parse
//            here to decouple from the geometry string parsing library.
//   Example: TRACKED_FEATURE = "x=23,y=99,key=b"

XYPoint ObstacleManager::customStringToPoint(string point_str)
{
  XYPoint null_pt;
  
  // Mandatory fields
  string x_str;
  string y_str;
  string obstacle_key_str;
  
  vector<string> svector = parseString(point_str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if(param == "x")
      x_str = value;
    else if(param == "y")
      y_str = value;
    else if((param == "key") || (param == "label"))
      obstacle_key_str = value;
  }

  if((x_str == "") || (y_str == "") || (obstacle_key_str == ""))
    return(null_pt);

  double x = atof(x_str.c_str());
  double y = atof(y_str.c_str());
  
  XYPoint new_pt(x,y);
  new_pt.set_msg(obstacle_key_str);
  
  return(new_pt);
}


//------------------------------------------------------------
// Procedure: handleGivenObstacle()
//   Example: pts={90.2,-80.4:...:82,-88:82.1,-83.7:85.4,-80.4},
//            label=ob_0,duration=60
//      Note: The duration parameter is optional

bool ObstacleManager::handleGivenObstacle(string poly, string source)
{
  // Regardless of result or return value, increment the right counter
  if(source == "mail")
    m_given_mail_ever++;
  else if(source == "mission")
    m_given_config_ever++;


  XYPolygon new_poly = string2Poly(poly);
  if(!new_poly.is_convex())
    return(false);

  // By default set the duration to be off (-1)
  double duration = -1;
  string dur_str = tokStringParse(poly, "duration", ',', '=');
  if(isNumber(dur_str))
    duration = atof(dur_str.c_str());
  
  string key = new_poly.get_label();

  if(source == "mail") {
    if(m_given_max_duration > 0) {
      if(dur_str == "") {
	string msg = "Incoming GIVEN_OBSTACLE has missing duration"; 
	reportRunWarning(msg);
	reportRunWarning(poly);
	return(false);
      }
      if(duration > m_given_max_duration) {
	string msg = "Incoming GIVEN_OBSTACLE has duration exceeding max duration";
	reportRunWarning(msg);
	return(false);
      }
    }
    m_given_mail_good++;
  }
  
  // Sanity check: If an obstacle with given label/key is already
  // known, and associated with an obstacle that is NOT a given
  // obstacle (rather it is associated with a data stream of points),
  // then reject.
  if(m_map_obstacles.count(key) && !m_map_obstacles[key].isGiven()) {
    string msg = "Reject given obst: already known as data obstacle"; 
    reportRunWarning(msg);
    return(false);
  }

  m_map_obstacles[key].setPoly(new_poly);
  m_map_obstacles[key].setDuration(duration);
  m_map_obstacles[key].setTStamp(m_curr_time);
  onNewObstacle("given");  
  
  reportEvent("new obstacle: " + m_map_obstacles[key].getInfo(m_curr_time)); 
  
  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailNewPoint()

bool ObstacleManager::handleMailNewPoint(string value)
{
  // Part 1: Build the new point and check its validity
  XYPoint newpt = customStringToPoint(value);
  if(!newpt.valid()) {
    m_points_invalid++;
    reportRunWarning("Invalid point:" + value);
    return(false);
  }
    
  // Part 2: Check the range of point to ownship, perhaps ignore it
  if(m_ignore_range > 0) {
    double ptx = newpt.get_vx();
    double pty = newpt.get_vy();
    double range = hypot(m_nav_x - ptx, m_nav_y - pty);
    if(range > m_ignore_range) {
      m_points_ignored++;
      return(true);
    }
  }
    
  m_points_total++;
  newpt.set_time(m_curr_time);
  
  // Part 3: Get the obstacle key. Contained in the msg=key parameter, or
  //         if no msg=key parameter then in the label=key parameter. 
  string key = newpt.get_msg();
  if(key == "")
    key = newpt.get_label();
  if(key == "") 
    key = "generic";

  // Part 4: Add the new point to the points associated with that key
  m_map_obstacles[key].addPoint(newpt);
  m_map_obstacles[key].setChanged(true);
  m_map_obstacles[key].setMaxPts(m_max_pts_per_cluster);
  onNewObstacle("points");  

  return(true);
}

//------------------------------------------------------------
// Procedure: updatePointHulls()
//   Purpose: Go through each obstacle and if the points of the
//            obstacle have changed, either new one arrived or
//            an older one has dropped, update the hull.

bool ObstacleManager::updatePointHulls()
{
  unsigned int pcount = m_map_obstacles.size();
  
  bool poly_label_thresh_over  = (pcount > m_poly_label_thresh);
  bool poly_shade_thresh_over  = (pcount > m_poly_shade_thresh);
  bool poly_vertex_thresh_over = (pcount > m_poly_vertex_thresh);

  bool thresh_crossed = false;
  if(poly_label_thresh_over != m_poly_label_thresh_over)
    thresh_crossed = true; 
  if(poly_shade_thresh_over != m_poly_shade_thresh_over)
    thresh_crossed = true; 
  if(poly_vertex_thresh_over != m_poly_vertex_thresh_over)
    thresh_crossed = true; 
  m_poly_label_thresh_over  = poly_label_thresh_over;
  m_poly_shade_thresh_over  = poly_shade_thresh_over;
  m_poly_vertex_thresh_over = poly_vertex_thresh_over;
  
  map<string,Obstacle>::iterator p;
  for(p=m_map_obstacles.begin(); p!=m_map_obstacles.end(); p++) {
    if(!p->second.hasChanged() && !thresh_crossed)
      continue;
    string key = p->first;
    vector<XYPoint> points = p->second.getPoints();
    if(points.size() == 0)
      continue;
    
    XYPolygon poly;
    if(m_lasso) {
      reportEvent("gen_lasso");
      poly = genPseudoHull(points, m_lasso_radius);
    }
    else {
      ConvexHullGenerator chgen;
      for(unsigned int i=0; i<points.size(); i++) 
	chgen.addPoint(points[i].x(), points[i].y(), points[i].get_label());
      
      poly = chgen.generateConvexHull();
    }

    // First check if the polygon is convex. Certain edge cases may result
    // in a non convex polygon even with N>2 points, e.g., 3 colinear pts.
    if(!poly.is_convex()) {
      reportRunWarning("hull failure - Placeholder needed " + uintToString(poly.size()));
      poly = placeholderConvexHull(key);
    }
    
    poly.set_label("obmgr_" + key);
    p->second.setPoly(poly);
    
    if(m_post_view_polys) {
      if(poly_label_thresh_over)
	poly.set_label_color("invisible");

      if(poly_shade_thresh_over)
	poly.set_color("fill", "invisible");
      else {
	poly.set_color("fill", m_obstacles_color);
	poly.set_transparency(0.15);
      }

      if(poly_vertex_thresh_over) {
	poly.set_vertex_color("invisible");
	poly.set_vertex_size(0);
      }
      else {
	poly.set_vertex_color(m_obstacles_color);
	poly.set_vertex_size(3);
      }
      
      poly.set_edge_color(m_obstacles_color);
      poly.set_edge_size(1);
      string poly_str = poly.get_spec(5);
      Notify("VIEW_POLYGON", poly_str);
    }
    //p->second.setChanged(false);
  }
  
  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailAlertRequest
//   Example: OBM_ALERT_REQUEST = "name=avd_ostacle,
//                                 update_var=OBSTACLE_ALERT,
//                                 alert_range=20,

bool ObstacleManager::handleMailAlertRequest(string request)
{
  string name, update_var, alert_range;

  vector<string> svector = parseString(request, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "name")
      name = value;
    else if(param == "update_var")
      update_var = value;
    else if(param == "alert_range")
      alert_range = value;
    else
      return(false);
  }

  if((name == "") || (update_var == "") || (alert_range == ""))
    return(false);

  if(!isNumber(alert_range))
    return(false);
  
  double d_alert_range = atof(alert_range.c_str());
  if(d_alert_range <= 0)
    return(false);

  // Alert request is valid, go ahead and set 
  m_alert_var = update_var;
  m_alert_name = name;
  m_alert_range = d_alert_range;

  // Upon new alert request, mark all obstacles as changed, to ensure
  // the latest will be posted, even if changed some time ago
  map<string,Obstacle>::iterator p;
  for(p=m_map_obstacles.begin(); p!=m_map_obstacles.end(); p++)
    p->second.setChanged();
  
  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigGeneralAlert
//   Example: general_alert = "name=gen_alert,
//                             update_var=GEN_OBSTACLE_ALERT,
//                             alert_range=2000,

bool ObstacleManager::handleConfigGeneralAlert(string request)
{
  string name = "gen_alert";
  string update_var, alert_range;

  vector<string> svector = parseString(request, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "name")
      name = value;
    else if(param == "update_var")
      update_var = value;
    else if(param == "alert_range")
      alert_range = value;
    else
      return(false);
  }

  if((name == "") || (update_var == "") || (alert_range == ""))
    return(false);

  if(!isNumber(alert_range))
    return(false);

  if(!strEnds(name, "_"))
     name += "_";
  
  double d_alert_range = atof(alert_range.c_str());
  if(d_alert_range <= 0)
    return(false);

  // Alert request is valid, go ahead and set 
  m_gen_alert_var   = update_var;
  m_gen_alert_name  = name;
  m_gen_alert_range = d_alert_range;

  return(true);
}

//------------------------------------------------------------
// Procedure: postConvexHullUpdates

void ObstacleManager::postConvexHullUpdates()
{
  // If no alert request has been made, no updates to be made
  if(m_alert_var == "")
    return;

  map<string, Obstacle>::iterator p;
  // For all obstacles that have a convex hull
  for(p=m_map_obstacles.begin(); p!=m_map_obstacles.end(); p++) {
    string   key = p->first;
    Obstacle obs = p->second;
    XYPolygon poly = obs.getPoly();

    // Double check it is convex
    if(poly.is_convex()) {
      double dist = poly.dist_to_poly(m_nav_x, m_nav_y);
      bool close_range = (dist <= m_alert_range);

      bool post_this_dist_to_poly = false;
      if(m_post_dist_to_polys == "true")
	post_this_dist_to_poly = true;
      else if((m_post_dist_to_polys == "close") && close_range) 
	post_this_dist_to_poly = true;

      // Only post dist if ownship w/in alert range or always on
      if(post_this_dist_to_poly) {
	string msg = toupper(key) + "," + doubleToString(dist,1);
	Notify("OBM_DIST_TO_OBJ", msg);
	//reportEvent("OBM_DIST_TO_OBJ="+msg);
      }

      // Only post obstacle hull if it has changed.
      if(m_map_obstacles[key].hasChanged()) {

	// At this point we're committed to posting an update so go ahead 
	// and mark this obstacle key as NOT changed.
	m_map_obstacles[key].setChanged(false);
	m_map_obstacles[key].incUpdatesTotal();

	// Only post hull if ownship is w/in alert range
	if(close_range)
	  postConvexHullUpdate(key, m_alert_var, m_alert_name);
	if((m_alert_var != "") && (dist <= m_gen_alert_range))
	  postConvexHullUpdate(key, m_gen_alert_var, m_gen_alert_name);
      }      
    }
  }
}


//------------------------------------------------------------
// Procedure: postConvexHullUpdate()

void ObstacleManager::postConvexHullUpdate(string key, string alert_var,
					   string alert_name)
{
  XYPolygon poly = m_map_obstacles[key].getPoly();

  string update_str = "name=" + alert_name + key + "#";
  update_str += "poly=" + poly.get_spec_pts(5) + ",label=" + key;

  m_alerts_posted++;  
  Notify(alert_var, update_str);
  reportEvent(alert_var + "=" + update_str);
}

//------------------------------------------------------------
// Procedure: placeholderConvexHull

XYPolygon ObstacleManager::placeholderConvexHull(string key)
{
  // Part 1: Sanity check: Can't build any kind of hull if no pts
  XYPolygon null_poly;
  null_poly.set_label("foo");
  if(m_map_obstacles[key].size() == 0)
    return(null_poly);

  // Part 2: Find the Center Point

  vector<XYPoint> points = m_map_obstacles[key].getPoints();

  double ctr_x = 0;
  double ctr_y = 0;
  unsigned int psize = points.size();
  for(unsigned int i=0; i<psize; i++) {
    ctr_x += points[i].x();
    ctr_y += points[i].y();
  }
  ctr_x = ctr_x / ((double)(psize));
  ctr_y = ctr_y / ((double)(psize));

  // Part 3: Find max distance from the center to all points to set the radius
  double max_dist = 0;
  for(unsigned int i=0; i<psize; i++) {
    double this_dist = distPointToPoint(ctr_x, ctr_y, points[i].x(), points[i].y());
    if(this_dist > max_dist)
      max_dist = this_dist;
  }
  if(max_dist < 0.5)
    max_dist = 0.5;

  // Part 4: Build a octagonal polygon
  stringstream ss;
  ss << "format=radial, x=" << ctr_x << ",y=" << ctr_y << ",radius="
     << max_dist << ",pts=8";
  XYPolygon poly = string2Poly(ss.str());

  return(poly);
}

//------------------------------------------------------------
// Procedure: getPseudoHul()

XYPolygon ObstacleManager::genPseudoHull(const vector<XYPoint>& pts, 
					 double radius)
{
  if(pts.size() == 0) {
    XYPolygon null_poly;
    null_poly.set_label("foo2");
    return(null_poly);
  }

  double avg_x = 0;
  double avg_y = 0;

  for(unsigned int i=0; i<pts.size(); i++) {
    avg_x += pts[i].x();
    avg_y += pts[i].y();
  }

  avg_x = avg_x / ((double)(pts.size()));
  avg_y = avg_y / ((double)(pts.size()));
  
  string spec = "x=" + doubleToString(avg_x,2)  + ",";
  spec += "y=" + doubleToString(avg_y,2)        + ",";
  spec += "radius=" + doubleToString(radius,2)  + ",";
  spec += "pts=" + uintToString(m_lasso_points) + ",";
  spec += "snap=0.01";

  XYPolygon octogon = stringRadial2Poly(spec);
  return(octogon);
}


//------------------------------------------------------------
// Procedure: updatePolyRanges()
//   Purpose: (1) Update the ranges of each obstacle poly to ownship
//            (2) If range changed from just OUT of range to now IN 
//                the alert_range, mark it as changed.

void ObstacleManager::updatePolyRanges()
{
  map<string,Obstacle>::iterator p;
  for(p=m_map_obstacles.begin(); p!=m_map_obstacles.end(); p++) {
    string    key   = p->first;
    XYPolygon poly  = p->second.getPoly();
    double    range = poly.dist_to_poly(m_nav_x, m_nav_y);

    // Also keep track of closest range ever to any obstacle
    if((m_min_dist_ever < 0) || (range < m_min_dist_ever)) {
      m_min_dist_ever = range;
      Notify("OBM_MIN_DIST_EVER", m_min_dist_ever);
    }
     
    // Compare previous range to newly calculated range
    if((m_map_obstacles[key].getRange() > m_alert_range) &&
       (range <= m_alert_range))	
      m_map_obstacles[key].setChanged();

    // Update with the newly calculated range
    m_map_obstacles[key].setRange(range);
  }
}

//------------------------------------------------------------
// Procedure: manageMemory()

void ObstacleManager::manageMemory()
{
  set<string> keys_to_forget;

  // Part 1: Identify all obstacles to be pruned by age
  map<string, Obstacle>::iterator p;
  for(p=m_map_obstacles.begin(); p!=m_map_obstacles.end(); p++) {
    string key  = p->first;
    bool remove = p->second.pruneByAge(m_max_age_per_point, m_curr_time);
    if(p->second.getPoly().active() == false)
      remove = true;

    if(remove)
      keys_to_forget.insert(key);
  }
    
  // Part 2: Free memory for obstacles flagged above
  set<string>::iterator q;
  for(q=keys_to_forget.begin(); q!=keys_to_forget.end(); q++) {
    string key = *q;

    // Post inactive view poly to erase this poly
    if(m_post_view_polys) {
      XYPolygon poly = m_map_obstacles[key].getPoly();
      string spec = poly.get_spec_inactive();
      Notify("VIEW_POLYGON", spec);
    }

    // Post to alert variabe that this obstacle is resolved
    Notify("OBM_RESOLVED", key);
    m_alerts_resolved++;
    reportEvent("OBM_RESOLVED=" + key);

    
    // Update key obstacle manager state
    m_map_obstacles.erase(key);
    m_obstacles_released++;
  }
}


//------------------------------------------------------------
// Procedure: handleConfigPostDistToPolys()

bool ObstacleManager::handleConfigPostDistToPolys(string val) 
{
  val = tolower(stripBlankEnds(val));
  if((val != "true") && (val != "false") && (val != "close"))
    return(false);
  
  m_post_dist_to_polys = val;
  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigGivenMaxDuration()

bool ObstacleManager::handleConfigGivenMaxDuration(string val) 
{
  if(tolower(val) == "off") {
    m_given_max_duration = -1;
    return(true);
  }

  return(setPosDoubleOnString(m_given_max_duration, val));
}

//------------------------------------------------------------
// Procedure: handleConfigFlag()

bool ObstacleManager::handleConfigFlag(string flag_type, string str)
{
  string moosvar = biteStringX(str, '=');
  string moosval = str;

  if((moosvar == "") || (moosval == ""))
    return(false);
  
  VarDataPair pair(moosvar, moosval, "auto");
  if(flag_type == "new_obs")
    m_new_obs_flags.push_back(pair);
  else
    return(false);
  
  return(true);
}


//------------------------------------------------------------
// Procedure: onNewObstacle()

void ObstacleManager::onNewObstacle(string obs_type)
{
  if((obs_type != "points") && (obs_type != "given"))
    return;
  
  m_obstacles_ever++;


  postFlags(m_new_obs_flags);  
}

//------------------------------------------------------------
// Procedure: postFlags()

void ObstacleManager::postFlags(const vector<VarDataPair>& flags)
{
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string moosvar = pair.get_var();

    // If posting is a double, just post. No macro expansion
    if(!pair.is_string()) {
      double dval = pair.get_ddata();
      Notify(moosvar, dval);
    }
    // Otherwise if string posting, handle macro expansion
    else {
      string sval = pair.get_sdata();

      unsigned int obs_now = m_map_obstacles.size();
      
      sval = macroExpand(sval, "OBS_NOW", obs_now);
      sval = macroExpand(sval, "OBS_EVER", m_obstacles_ever);
      
      Notify(moosvar, sval);
    }
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool ObstacleManager::buildReport() 
{
  string str_lasso     = boolToString(m_lasso);
  string str_lasso_pts = uintToString(m_lasso_points);
  string str_lasso_rad = doubleToStringX(m_lasso_radius);

  string str_alert_rng  = doubleToStringX(m_alert_range,1);
  string str_gen_alert_rng  = doubleToStringX(m_gen_alert_range,1);
  string str_ignore_rng = doubleToStringX(m_ignore_range,1);

  string str_max_pts_per = uintToString(m_max_pts_per_cluster);
  string str_max_age_per = doubleToStringX(m_max_age_per_point);

  string str_navx = doubleToStringX(m_nav_x,1);
  string str_navy = doubleToStringX(m_nav_y,1);
  string str_nav = "(" + str_navx + "," + str_navy + ")";

  string str_post_view_polys = boolToString(m_post_view_polys);

  string str_min_dist_ever = doubleToStringX(m_min_dist_ever,1);
  
  m_msgs << "Configuration (point handling):             " << endl;
  m_msgs << "  point_var:    " << m_point_var              << endl;
  m_msgs << "  max_pts_per_cluster: " << str_max_pts_per   << endl;
  m_msgs << "  max_age_per_point:   " << str_max_age_per   << endl;
  m_msgs << "  ignore_range:        " << str_ignore_rng    << endl;
  m_msgs << "Configuration (given_obstacles):            " << endl;
  m_msgs << "  given_max_duration: " << m_given_max_duration << endl;
  m_msgs << "Configuration (viewing):                    " << endl;
  m_msgs << "  post_dist_to_polys: " << m_post_dist_to_polys << endl;
  m_msgs << "  post_view_polys:    " << str_post_view_polys  << endl;
  m_msgs << "  obstacle color:     " << m_obstacles_color  << endl;
  m_msgs << "Configuration (alerts):                     " << endl;
  m_msgs << "  alert_var:   " << m_alert_var               << endl;
  m_msgs << "  alert_name:  " << m_alert_name              << endl;
  m_msgs << "  alert_range: " << str_alert_rng             << endl;
  m_msgs << "Configuration (general alert):              " << endl;
  m_msgs << "  gen_alert_var:   " << m_gen_alert_var       << endl;
  m_msgs << "  gen_alert_name:  " << m_gen_alert_name      << endl;
  m_msgs << "  gen_alert_range: " << str_gen_alert_rng     << endl;
  m_msgs << "Configuration (lasso option):               " << endl;
  m_msgs << "  lasso:        " << str_lasso                << endl;
  m_msgs << "  lasso_points: " << str_lasso_pts            << endl;
  m_msgs << "  lasso_radius: " << str_lasso_rad            << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "State: (Nav)                                " << endl;
  m_msgs << "  Nav Position:      " << str_nav             << endl;
  m_msgs << "State: (points):                            " << endl;
  m_msgs << "  Points Received:   " << m_points_total      << endl;
  m_msgs << "  Points Invalid:    " << m_points_invalid    << endl;
  m_msgs << "  Points Ignored:    " << m_points_ignored    << endl;
  m_msgs << "State: (given_obstacles):                   " << endl;
  m_msgs << "  Given Obstacles (mail) ever: " << m_given_mail_ever << endl;
  m_msgs << "  Given Obstacles (mail) good: " << m_given_mail_good << endl;
  m_msgs << "  Given Obstacles (config):    " << m_given_config_ever << endl;
  m_msgs << "State: (obstacles):                         " << endl;
  m_msgs << "  Obstacles:          " << m_map_obstacles.size() << endl;
  m_msgs << "  Obstacles released: " << m_obstacles_released << endl;
  m_msgs << "  Closest range ever: " << str_min_dist_ever << endl;
  m_msgs << "State: (alerts):                            " << endl;
  m_msgs << "  Alerts Posted:   " << m_alerts_posted   << endl;
  m_msgs << "  Alerts Resolved: " << m_alerts_resolved << endl;

  m_msgs << endl << endl;

  ACTable actab(9);
  actab << "    |     |       | Up    |      | Dur   | Time2 | Curr | Min ";
  actab << "Key | Pts | Verts | Dates | Type | ation | Live  | Dist | Dist";
  actab.addHeaderLines();

  map<string, Obstacle>::iterator p;
  for(p=m_map_obstacles.begin(); p!=m_map_obstacles.end(); p++) {
    string key = p->first;
    Obstacle obstacle = p->second;
    string pts_str = uintToString(obstacle.size());

    string hull_size_str = uintToString(obstacle.getPoly().size());

    string updates_str = uintToString(obstacle.getUpdatesTotal());

    string type_str = "points";
    if(obstacle.isGiven())
      type_str = "given";

    string duration_str = "n/a";
    string time_to_live_str = "n/a";
    if(obstacle.isGiven() && (obstacle.getDuration() > 0)) {
      duration_str = doubleToStringX(obstacle.getDuration(),1);
      double time_to_live = obstacle.getTimeToLive(m_curr_time);
      time_to_live_str = doubleToStringX(time_to_live,1);
    }

    string range_str = doubleToString(obstacle.getRange(),1);
    string min_rng_str = doubleToString(obstacle.getMinRange(),1);
    
    actab << key;
    actab << pts_str;
    actab << hull_size_str;
    actab << updates_str;
    actab << type_str;
    actab << duration_str;
    actab << time_to_live_str;
    actab << range_str;
    actab << min_rng_str;
  }

  m_msgs << actab.getFormattedString();
  m_msgs << endl << endl;

  return(true);
}



