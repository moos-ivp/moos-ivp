/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleManager.cpp                                  */
/*    DATE: Aug 27th 2014 For RobotX                             */
/*    DATE: Sep 17th 2017 Overhaul by mikerb                     */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "ACTable.h"
#include "ObstacleManager.h"
#include "ConvexHullGenerator.h"
#include "XYFormatUtilsPoint.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ObstacleManager::ObstacleManager()
{
  // State Variables
  m_nav_x = 0;
  m_nav_y = 0;

  // Init configuration variables
  m_alert_var  = "";    // Initially no alerts will be posted

  m_alert_range  = 20;  // meters
  m_ignore_range = -1;  // meters (neg value means off)

  m_max_pts_per_cluster = 20;
  m_max_age_per_point   = 20;

  m_lasso = false;
  m_lasso_points = 6;
  m_lasso_radius = 5;  // meters
  
  // Init state variables
  m_points_total   = 0;
  m_points_ignored = 0;
  m_clusters_released = 0;

  // Init info output variables
  m_post_dist_to_polys = true;
  m_post_view_polys = true;
}

//---------------------------------------------------------
// Procedure: OnNewMail

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

    bool handled = false;
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
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

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
  postConvexHullUpdates();
  updatePolyRanges();
  manageMemory();
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
    else if(param == "given_obstable")
      handled = handleGivenObstacle(value);
    else if(param == "alert_range")
      handled = setPosDoubleOnString(m_alert_range, value);
    else if(param == "ignore_range")
      handled = setPosDoubleOnString(m_ignore_range, value);
    else if(param == "max_pts_per_cluster")
      handled = setUIntOnString(m_max_pts_per_cluster, value);
    else if(param == "max_age_per_point")
      handled = setPosDoubleOnString(m_max_age_per_point, value);
    else if(param == "post_dist_to_polys")
      handled = setBooleanOnString(m_post_dist_to_polys, value);
    else if(param == "post_view_polys")
      handled = setBooleanOnString(m_post_view_polys, value);
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

  Notify("VEHICLE_CONNECT", "true");
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void ObstacleManager::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  if(m_point_var != "")
    Register(m_point_var, 0);

  Register("NAV_X", 0);
  Register("NAV_Y", 0);

  Register("GIVEN_OBSTACLE",0);
  Register("OBM_ALERT_REQUEST",0);
}


//------------------------------------------------------------
// Procedure: customStringToPoint
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
// Procedure: handleGivenObstacle
//   Example: pts={90.2,-80.4:...:82,-88:82.1,-83.7:85.4,-80.4},label=ob_0

bool ObstacleManager::handleGivenObstacle(string poly)
{
  XYPolygon new_poly = string2Poly(poly);
  if(!new_poly.is_convex())
    return(false);

  string label = new_poly.get_label();

  // Sanity check: If an obstacle with given label/key is already
  // known, and associated with an obstacle that is NOT a given
  // obstacle (rather it is associated with a data stream of points),
  // then reject.
  if(m_map_poly_given.count(label) && !m_map_poly_given[label]) {
    string msg = "Reject given obst: already known as data obstacle"; 
    reportRunWarning(msg);
  }
  
  if(m_map_poly_given.count(label) != 0) {
    m_map_poly_convex[label] = new_poly;
    m_map_poly_given[label] = true;
    m_map_poly_changed[label] = true;
    return(true);
  }

  m_map_poly_convex[label] = new_poly;
  m_map_poly_changed[label] = true;
  m_map_poly_given[label]   = true;
  
  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailNewPoint()

bool ObstacleManager::handleMailNewPoint(string value)
{
  // Part 1A: Build the new point and perhaps check its validity
  XYPoint newpt = customStringToPoint(value);
  if(!newpt.valid()) {
    m_points_invalid++;
    reportRunWarning("Invalid point:" + value);
    return(false);
  }
    
  // Part 1B: Check the range of the point to ownship
  if(m_ignore_range > 0) {
    double ptx = newpt.get_vx();
    double pty = newpt.get_vy();
    double range = hypot(m_nav_x - ptx, m_nav_y - pty);
    if(range > m_ignore_range) {
      m_points_ignored++;
      return(true);
    }
  }
    
  newpt.set_time(m_curr_time);
  
  // Part 2: Increment the total points received but return if the point
  //         is not valid
  m_points_total++;
  if(!newpt.valid())
    return(false);

  // Part 3: Get the obstacle key. Contained in the msg=key parameter, or
  //         if no msg=key parameter then in the label=key parameter. 
  string obstacle_key = newpt.get_msg();
  if(obstacle_key == "")
    obstacle_key = newpt.get_label();
  if(obstacle_key == "") 
    obstacle_key = "generic";

#if 1
  // Part 4A: Check the distance of the newpoint to previously
  // received points. Perhaps reject if dist is not high
  if(m_map_points[obstacle_key].size() > 0) {
    list<XYPoint>::iterator q;
    double min_dist = -1;
    for(q=m_map_points[obstacle_key].begin(); 
	q!=m_map_points[obstacle_key].end(); q++) {
      XYPoint pt = *q;
      double dist = distPointToPoint(pt, newpt);
      if((min_dist < 0) || (dist < min_dist))
	min_dist = dist;
    }
    if(min_dist < 0.1)
      return(true);
  }
#endif


  // Part 4: Add the new point to the points associated with that key
  m_map_points[obstacle_key].push_back(newpt);
  if(m_map_points[obstacle_key].size() > m_max_pts_per_cluster)
    m_map_points[obstacle_key].pop_front();

  m_map_points_total[obstacle_key] = m_map_points[obstacle_key].size();
  

  // Part 5: If the set of points associated with the obstacle_key is too
  //         small to make a convex polygon, we can just return now.
  if(m_map_points_total[obstacle_key] < 3)
    return(true);

  // Part 6: Determine if the convex hull associated with the obstacle
  //         key needs to be updated. 
  //   Note: If the hull doesn't exist and there are >2 points now then
  //         try to build the convex hull
  //   Note: If the hull does exist, but it contains the new point, then
  //         the hull does not need to be updated.

  bool hull_update_needed = false;
  if(m_map_poly_convex.count(obstacle_key)==0)
    hull_update_needed = true;
  else if(!m_map_poly_convex[obstacle_key].contains(newpt.x(), newpt.y()))
    hull_update_needed = true;

  if(!hull_update_needed)
    return(true);

  // A new convex hull calculation is needed because of this new point
  vector<XYPoint> points;

  list<XYPoint>::iterator p;
  for(p=m_map_points[obstacle_key].begin(); 
      p!=m_map_points[obstacle_key].end(); p++) {
    XYPoint pt = *p;
    points.push_back(pt);
  }

  XYPolygon poly;
  if(m_lasso)
    poly = genPseudoHull(points, m_lasso_radius);
  else {
    ConvexHullGenerator chgen;
    for(unsigned int i=0; i<points.size(); i++) 
      chgen.addPoint(points[i].x(), points[i].y(), points[i].get_label());
    poly = chgen.generateConvexHull();
  }

  // First check if the polygon is convex. Certain edge cases may result
  // in a non convex polygon even with N>2 points, e.g., 3 colinear pts.
  if(!poly.is_convex())
    poly = placeholderConvexHull(obstacle_key);
  
  poly.set_label(obstacle_key);
  m_map_poly_convex[obstacle_key]  = poly;
  m_map_poly_changed[obstacle_key] = true;
  
  if(m_post_view_polys) {
    poly.set_vertex_color("dodger_blue");
    poly.set_edge_color("dodger_blue");
    poly.set_vertex_size(4);
    poly.set_edge_size(1);
    string poly_str = poly.get_spec(3);
    Notify("VIEW_POLYGON", poly_str);
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

  // Upon new alert request, mark all convex hulls as changed, to ensure
  // the latest will be posted, even if changed some time ago
  map<string,bool>::iterator p;
  for(p=m_map_poly_changed.begin(); p!=m_map_poly_changed.end(); p++) {
    string obstacle_key = p->first;
    m_map_poly_changed[obstacle_key] = true;
  }  
  
  return(true);
}

//------------------------------------------------------------
// Procedure: postConvexHullUpdates

void ObstacleManager::postConvexHullUpdates()
{
  // If no alert request has been made, no updates to be made
  if(m_alert_var == "")
    return;

  map<string, XYPolygon>::iterator p;
  // For all obstacles that have a convex hull
  for(p=m_map_poly_convex.begin(); p!=m_map_poly_convex.end(); p++) {
    string obs_key = p->first;
    XYPolygon poly = p->second;

    // Double check it is convex
    if(poly.is_convex()) {
      double dist = poly.dist_to_poly(m_nav_x, m_nav_y);
      Notify("OBM_DIST_TO_OBJ_" + toupper(obs_key), dist);
      // Only post if ownship is within the alert range
      if(dist <= m_alert_range)
        postConvexHullUpdate(obs_key);
    }
  }
}


//------------------------------------------------------------
// Procedure: postConvexHullUpdate

void ObstacleManager::postConvexHullUpdate(string obstacle_key)
{
  // Part 1: If the polygon hasn't changed, don't post an update
  //if(!m_map_poly_changed[obstacle_key])
  //  return;

  // At this point we're committed to posting an update so go ahead 
  // and mark this obstacle key as NOT changed.
  m_map_poly_changed[obstacle_key] = false;
  
  // Increment the total number of updates posted for this obstacle key
  if(m_map_updates_total.count(obstacle_key) == 0)
    m_map_updates_total[obstacle_key] = 0;
  m_map_updates_total[obstacle_key]++;

  
  XYPolygon poly = m_map_poly_convex[obstacle_key];

  //string update_str = "name=" + m_alert_name + obstacle_key + "#";
  string update_str = "name=" + obstacle_key + "#";
  update_str += "poly=" + poly.get_spec_pts(5) + ",label=" + obstacle_key;
  
  Notify(m_alert_var, update_str);
}

//------------------------------------------------------------
// Procedure: placeholderConvexHull

XYPolygon ObstacleManager::placeholderConvexHull(string obstacle_key)
{
  // Part 1: Sanity check: Can't build any kind of hull if no points
  // at all.
  XYPolygon null_poly;
  if(m_map_points.count(obstacle_key) == 0)
    return(null_poly);
  if(m_map_points[obstacle_key].size() == 0)
    return(null_poly);

  // Part 2: Find the Center Point
  //  const vector<XYPoint>& points = m_map_points[obstacle_key];

  vector<XYPoint> points;
  list<XYPoint>::iterator p;
  for(p=m_map_points[obstacle_key].begin(); 
      p!=m_map_points[obstacle_key].end(); p++) {
    XYPoint pt = *p;
    points.push_back(pt);
  }

  double ctr_x = 0;
  double ctr_y = 0;
  unsigned int i, psize = points.size();
  for(i=0; i<psize; i++) {
    ctr_x += points[i].x();
    ctr_y += points[i].y();
  }
  ctr_x = ctr_x / ((double)(psize));
  ctr_y = ctr_y / ((double)(psize));

  // Part 3: Find the max distance to the center of all points to set the radius
  double max_dist = 0;
  for(i=0; i<psize; i++) {
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

void ObstacleManager::updatePolyRanges()
{
  map<string,XYPolygon>::iterator p;
  for(p=m_map_poly_convex.begin(); p!=m_map_poly_convex.end(); p++) {
    string key = p->first;
    XYPolygon poly = p->second;
    double range = poly.dist_to_poly(m_nav_x, m_nav_y);

    bool update_needed = false;
    if(m_map_poly_range.count(key) == 0)
      update_needed = true;
    else {
      if((m_map_poly_range[key] > m_alert_range) &&
	 (range <= m_alert_range))
	update_needed = true;
    }
    if(update_needed)
      m_map_poly_changed[key] = true;
    m_map_poly_range[key] = range;
  }
}

//------------------------------------------------------------
// Procedure: manageMemory()

void ObstacleManager::manageMemory()
{
  set<string> keys_to_forget;

  // Part 1: Go through all sensor points and prune based on age
  // Note which obstacle keys have no 
  map<string, list<XYPoint> >::iterator p;
  for(p=m_map_points.begin(); p!=m_map_points.end(); p++) {
    string key = p->first;
    list<XYPoint>::iterator q;
    for(q=m_map_points[key].begin(); q!=m_map_points[key].end(); ) {
      XYPoint pt = *q;
      double age = m_curr_time - pt.get_time();
      if(age > m_max_age_per_point)
	q = m_map_points[key].erase(q);
      else
	++q;
    }
    // Flag the obstacle for removal if the points list is empty and
    // is it not a given obstacle.
    bool forget_this_obstacle = true;
    if(m_map_points[key].size() != 0)
      forget_this_obstacle = false;
    if(m_map_poly_given[key] == true)
      forget_this_obstacle = false;
    if(forget_this_obstacle)
      keys_to_forget.insert(key);
  }

  // Part 2: Just to be sure, check all polys and also flag it for
  // removal if there are no points associated, and it is not a given
  // obstacle.
  map<string, XYPolygon>::iterator q;
  for(q=m_map_poly_convex.begin(); q!=m_map_poly_convex.end(); q++) {
    string key = q->first;

    bool forget_this_obstacle = true;
    if((m_map_points.count(key) > 0) && (m_map_points[key].size() > 0))
      forget_this_obstacle = false;
    if(m_map_poly_given[key] == true)
      forget_this_obstacle = false;
    if(forget_this_obstacle)
      keys_to_forget.insert(key);
  }
    
  // Part 3: Free memory for obstacles flagged above
  set<string>::iterator p1;
  for(p1=keys_to_forget.begin(); p1!=keys_to_forget.end(); p1++) {
    string key = *p1;
    m_map_points.erase(key);
    m_map_points_total.erase(key);
    m_map_poly_given.erase(key);
    m_map_poly_changed.erase(key);
    m_map_updates_total.erase(key);
    m_clusters_released++;

    string done_str = "name=" + key + "#resolved=true";
    Notify(m_alert_var, done_str);
    
    // If a poly is being freed, first post a viewable poly with
    // active set to false, so a renderer knows to erase.
    if(m_post_view_polys && m_map_poly_convex.count(key)) {
      m_map_poly_convex[key].set_active(false);
      string spec = m_map_poly_convex[key].get_spec();
      Notify("VIEW_POLYGON", spec);
    }

    m_map_poly_convex.erase(key);
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool ObstacleManager::buildReport() 
{
  string str_lasso = boolToString(m_lasso);
  string str_lasso_pts = uintToString(m_lasso_points);
  string str_lasso_rad = doubleToStringX(m_lasso_radius);

  string str_alert_rng = doubleToStringX(m_alert_range,1);
  string str_ignore_rng = doubleToStringX(m_ignore_range,1);

  string str_max_pts_per = uintToString(m_max_pts_per_cluster);
  string str_max_age_per = doubleToStringX(m_max_age_per_point);

  string str_navx = doubleToStringX(m_nav_x,1);
  string str_navy = doubleToStringX(m_nav_y,1);
  string str_nav = "(" + str_navx + "," + str_navy + ")";

  
  m_msgs << "Configuration (point handling):             " << endl;
  m_msgs << "  point_var:    " << m_point_var              << endl;
  m_msgs << "  max_pts_per_cluster: " << str_max_pts_per   << endl;
  m_msgs << "  max_age_per_point:   " << str_max_age_per   << endl;
  m_msgs << "  ignore_range:        " << str_ignore_rng    << endl;
  m_msgs << "Configuration (alerts):                     " << endl;
  m_msgs << "  alert_var:   " << m_alert_var               << endl;
  m_msgs << "  alert_name:  " << m_alert_name              << endl;
  m_msgs << "  alert_range: " << str_alert_rng             << endl;
  m_msgs << "Configuration (lasso option):               " << endl;
  m_msgs << "  lasso:        " << str_lasso                << endl;
  m_msgs << "  lasso_points: " << str_lasso_pts            << endl;
  m_msgs << "  lasso_radius: " << str_lasso_rad            << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "State:                                      " << endl;
  m_msgs << "  Nav Position:      " << str_nav             << endl;
  m_msgs << "  Points Received:   " << m_points_total      << endl;
  m_msgs << "  Points Invalid:    " << m_points_invalid    << endl;
  m_msgs << "  Points Ignored:    " << m_points_ignored    << endl;
  m_msgs << "  Polygon obstacles: " << m_map_poly_convex.size() << endl;
  m_msgs << "  Clusters:          " << m_map_points.size() << endl;
  m_msgs << "  Clusters released: " << m_clusters_released << endl;

  m_msgs << endl << endl;

  ACTable actab(4);
  actab << "ObstacleKey | Points | HullSize | Updates ";
  actab.addHeaderLines();

  map<string, unsigned int>::iterator p;
  for(p=m_map_points_total.begin(); p!=m_map_points_total.end(); p++) {
    string obstacle_key = p->first;
    string points_str = "0";
    if(m_map_points.count(obstacle_key) !=0)
      points_str = uintToString(m_map_points[obstacle_key].size());

    string hull_size_str = "0";
    if(m_map_poly_convex.count(obstacle_key) !=0)
      hull_size_str = uintToString(m_map_poly_convex[obstacle_key].size());

    string updates_total_str = "n/a";
    if(m_map_updates_total.count(obstacle_key) !=0)
      updates_total_str = uintToString(m_map_updates_total[obstacle_key]);
    
    actab << obstacle_key;
    actab << points_str;
    actab << hull_size_str;
    actab << updates_total_str;
  }

  m_msgs << actab.getFormattedString();
  m_msgs << endl << endl;

  return(true);
}


