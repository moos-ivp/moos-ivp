/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleManager.cpp                                  */
/*    DATE: Aug 27th 2014                                        */
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
  // Init configuration variables
  m_obstacle_alert_var  = "OBSTACLE_ALERT";
  m_updates_request_var = "OBSTACLE_UPDATE_REQUEST";

  m_key_field = "label";
  m_hull_approx = 0;

  // Init state variables
  m_points_total = 0;
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

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    
    if(key == m_point_var) { 
      bool handled = handleMailNewPoint(sval);
      if(!handled)
       reportRunWarning("Unhandled Mail: " + key);
    }
    else if(key == m_updates_request_var) { 
      bool handled = handleMailUpdatesRequest(sval);
      if(!handled)
       reportRunWarning("Unhandled Mail: " + key);
    }
    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
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
  postConvexHullAlerts();
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
    else if(param == "key_field")
      handled = setNonWhiteVarOnString(m_key_field, value);
    else if(param == "hull_approx")
      handled = setPosDoubleOnString(m_hull_approx, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  // We left the m_point_var unset in the constructor because we don't want to 
  // unnecessarily register for a variable that we don't actually need
  if(m_point_var == "")
    m_point_var = "TRACKED_FEATURE";

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
  Register(m_updates_request_var);
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
// Procedure: handleMailNewPoint()

bool ObstacleManager::handleMailNewPoint(string value)
{
  // Part 1: Build the new point and check if it is an obstacle point
  XYPoint newpt = customStringToPoint(value);

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

  // Part 4: Add the new point to the points associated with that key
#if 0
  m_map_points[obstacle_key].push_back(newpt);
  m_map_points_total[obstacle_key]++;
#endif

#if 1
  m_map_points[obstacle_key].push_back(newpt);
  if(m_map_points[obstacle_key].size() > 10)
    m_map_points[obstacle_key].pop_front();

  m_map_points_total[obstacle_key] = m_map_points[obstacle_key].size();
#endif
  

  // Part 6: If the set of points associated with the obstacle_key is too
  //         small to make a convex polygon, we can just return now.
  //if(m_map_points_total[obstacle_key] < 3)
  //  return(true);

  // Part 5: Determine if the convex hull associated with the obstacle
  //         key needs to be updated. 
  //   Note: If the hull doesn't exist and there are >2 points now then
  //         try to build the convex hull
  //   Note: If the hull does exist, but it contains the new point, then
  //         the hull does not need to be updated.

  bool hull_update_needed = false;
  if(m_map_convex_hull.count(obstacle_key)==0)
    hull_update_needed = true;
  else if(!m_map_convex_hull[obstacle_key].contains(newpt.x(), newpt.y()))
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

  //const vector<XYPoint>& points = m_map_points[obstacle_key];

#if 0
  ConvexHullGenerator chgen;
  for(unsigned int i=0; i<points.size(); i++) 
    chgen.addPoint(points[i].x(), points[i].y(), points[i].get_label());
  
  XYPolygon poly = chgen.generateConvexHull();
#endif

#if 1
  XYPolygon poly = genPseudoHull(points, 1);
#endif


  // First check if the polygon is convex. Certain edge cases may result
  // in a non convex polygon even with N>2 points, e.g., 3 colinear pts.
  if(!poly.is_convex())
    //return(true);
    poly = placeholderConvexHull(obstacle_key);
  
  poly.set_label(obstacle_key);
  m_map_convex_hull[obstacle_key]       = poly;
  m_map_hull_changed_flag[obstacle_key] = true;
  
  
  poly.set_vertex_color("dodger_blue");
  poly.set_edge_color("dodger_blue");
  poly.set_vertex_size(4);
  poly.set_edge_size(1);
  string poly_str = poly.get_spec(3);
  Notify("VIEW_POLYGON", poly_str);

  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailUpdatesRequest
//   Example: OBSTACLE_UPDATE_REQUEST = "obstacle_key=a,bhv_name=avd_obstacle_a"

bool ObstacleManager::handleMailUpdatesRequest(string request)
{
  string key;
  string var;

  vector<string> svector = parseString(request, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if(param == "obstacle_key")
      key = value;
    else if(param == "update_var")
      var = value;
    else
      return(false);
  }

  if((key == "") || (var == ""))
    return(false);
  
  m_map_updates_var[key] = var;
  return(true);
}


//------------------------------------------------------------
// Procedure: postConvexHullAlerts

void ObstacleManager::postConvexHullAlerts()
{
  map<string, XYPolygon>::iterator p;
  for(p=m_map_convex_hull.begin(); p!=m_map_convex_hull.end(); p++) {
    postConvexHullAlert(p->first);
  }
}


//------------------------------------------------------------
// Procedure: postConvexHullAlert

void ObstacleManager::postConvexHullAlert(string obstacle_key)
{
  // Sanity check 1: if an alert has already been generated, dont repeat
  if(m_map_alerted_flag[obstacle_key])
    return;

  // Sanity check 2: if a convex hull has not been created for this key
  // no alert can be made
  if(m_map_convex_hull.count(obstacle_key) == 0)
    return;

  // Sanity check 3: if a hull/poly exists, but its not convex, return
  XYPolygon poly = m_map_convex_hull[obstacle_key];
  if(!poly.is_convex())
    return;

  // Part 1: Get the string version of the polygon
  string poly_str = poly.get_spec(3);
  Notify("VIEW_POLYGON", poly_str);

  // Part 2: Construct the posting to be made
  string obstacle_alert = "name=" + obstacle_key + "#poly=";
  obstacle_alert += poly.get_spec_pts(2) + ",label=" + obstacle_key;
  obstacle_alert += "#obstacle_key=" + obstacle_key;
  
  // Part 3: Make the posting
  Notify(m_obstacle_alert_var, obstacle_alert);

  // Part 4: Note that an alert has been generated for this obstacle key
  m_map_alerted_flag[obstacle_key] = true;
}


//------------------------------------------------------------
// Procedure: postConvexHullUpdates

void ObstacleManager::postConvexHullUpdates()
{
  map<string, bool>::iterator p;
  for(p=m_map_hull_changed_flag.begin(); p!=m_map_hull_changed_flag.end(); p++) {
    postConvexHullUpdate(p->first);
  }
}


//------------------------------------------------------------
// Procedure: postConvexHullUpdate

void ObstacleManager::postConvexHullUpdate(string obstacle_key)
{
  // If there has not been an enitity, e.g., behavior, that has requested
  // updates
  if(m_map_updates_var.count(obstacle_key) == 0)
    return;

  if(!m_map_hull_changed_flag[obstacle_key])
    return;

  // At this point we're committed to posting an update so go ahead 
  // and mark this obstacle key as NOT changed.
  m_map_hull_changed_flag[obstacle_key] = false;
  
  // Increment the total number of updates posted for this obstacle key
  if(m_map_updates_total.count(obstacle_key) == 0)
    m_map_updates_total[obstacle_key] = 0;
  m_map_updates_total[obstacle_key]++;

  
  XYPolygon poly = m_map_convex_hull[obstacle_key];
  string poly_str = poly.get_spec(3);
  Notify("VIEW_POLYGON", poly_str);

  string update_var = m_map_updates_var[obstacle_key];
  string update_str = poly.get_spec_pts(2) + ",label=" + obstacle_key;
  
  Notify(update_var, update_str);
}

//------------------------------------------------------------
// Procedure: placeholderConvexHull

XYPolygon ObstacleManager::placeholderConvexHull(string obstacle_key)
{
  // Part 1: Sanity check: Can't build any kind of hull if no points at all.
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
  ss << "format=radial, x=" << ctr_x << ",y=" << ctr_y << ",radius=" << max_dist << ",pts=8";
  XYPolygon poly = string2Poly(ss.str());

  return(poly);
}



//------------------------------------------------------------
// Procedure: buildReport()

bool ObstacleManager::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "Configuration:                              " << endl;
  m_msgs << "  PointVar: " << m_point_var << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "State:                                      " << endl;
  m_msgs << "  Points Received:  " << m_points_total       << endl;
  m_msgs << "  Clusters:         " << m_map_points.size()  << endl;

  m_msgs << endl << endl;

  ACTable actab(5);
  actab << "ObstacleKey | Points | HullSize | UpdatesVar | Updates ";
  actab.addHeaderLines();

  map<string, unsigned int>::iterator p;
  for(p=m_map_points_total.begin(); p!=m_map_points_total.end(); p++) {
    string obstacle_key = p->first;
    string points_str = "0";
    if(m_map_points.count(obstacle_key) !=0)
      points_str = uintToString(m_map_points[obstacle_key].size());

    string hull_size_str = "0";
    if(m_map_convex_hull.count(obstacle_key) !=0)
      hull_size_str = uintToString(m_map_convex_hull[obstacle_key].size());

    string updates_var_str = "n/a";
    if(m_map_updates_var.count(obstacle_key) !=0)
      updates_var_str = m_map_updates_var[obstacle_key];

    string updates_total_str = "n/a";
    if(m_map_updates_total.count(obstacle_key) !=0)
      updates_total_str = uintToString(m_map_updates_total[obstacle_key]);
    
    actab << obstacle_key;
    actab << points_str;
    actab << hull_size_str;
    actab << updates_var_str;
    actab << updates_total_str;
  }

  m_msgs << actab.getFormattedString();
  m_msgs << endl << endl;

  return(true);
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
  
  string spec = "x=" + doubleToString(avg_x,2) + ",";
  spec += "y=" + doubleToString(avg_y,2) + ",";
  spec += "radius=" + doubleToString(radius,2) + ",";
  spec += "pts=8,snap=0.01";

  XYPolygon octogon = stringRadial2Poly(spec);
  return(octogon);
}
