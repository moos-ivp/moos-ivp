/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleManager.cpp                                  */
/*    DATE: Aug 27th 2014                                        */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "ObstacleManager.h"
#include "ConvexHullGenerator.h"
#include "XYFormatUtilsPoint.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ObstacleManager::ObstacleManager()
{
  m_points_total = 0;
  m_obstacle_alert_var  = "OBSTACLE_ALERT";
  m_updates_request_var = "OBSTACLE_UPDATE_REQUEST";
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
//            happens before connection is open

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
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == m_point_var) {
      m_point_var = value;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  if(m_point_var == "")
    m_point_var = "VIEW_POINT";

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
// Procedure: handleMailNewPoint()

bool ObstacleManager::handleMailNewPoint(string value)
{
  m_points_total++;
  XYPoint newpt = string2Point(value);
  if(!newpt.valid())
    return(false);
  if(newpt.get_type() != "obstacle")
    return(true);

  string obstacle_key = newpt.get_msg();
  if(obstacle_key == "")
    obstacle_key = newpt.get_label();

  m_map_points[obstacle_key].push_back(newpt);
  m_map_points_total[obstacle_key]++;

  bool hull_update_needed = false;
  if(m_map_convex_hull.count(obstacle_key) == 0)
    hull_update_needed = true;
  else if(!m_map_convex_hull[obstacle_key].contains(newpt.x(), newpt.y()))
    hull_update_needed = true;

  if(!hull_update_needed)
    return(true);

  // A new convex hull calculation is needed because of this new point
  const vector<XYPoint>& points = m_map_points[obstacle_key];

  ConvexHullGenerator chgen;
  for(unsigned int i=0; i<points.size(); i++) 
    chgen.addPoint(points[i].x(), points[i].y(), points[i].get_label());
  
  XYPolygon poly = chgen.generateConvexHull();
  poly.set_label(obstacle_key);

  m_map_convex_hull[obstacle_key]  = poly;
  m_map_hull_changed[obstacle_key] = true;

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
  
  m_map_updates[key] = var;
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
  if(m_map_alerted[obstacle_key])
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
  string poly_str = poly.get_spec();
  Notify("VIEW_POLYGON", poly_str);

  // Part 2: Construct the posting to be made
  string obstacle_alert = "name=" + obstacle_key + "#poly=";
  obstacle_alert += poly.get_spec_pts(2) + ",label=" + obstacle_key;
  obstacle_alert += "#obstacle_key=" + obstacle_key;
  
  // Part 3: Make the posting
  Notify(m_obstacle_alert_var, obstacle_alert);

  // Part 4: Note that an alert has been generated for this obstacle key
  m_map_alerted[obstacle_key] = true;
}


//------------------------------------------------------------
// Procedure: postConvexHullUpdates

void ObstacleManager::postConvexHullUpdates()
{
  map<string, bool>::iterator p;
  for(p=m_map_hull_changed.begin(); p!=m_map_hull_changed.end(); p++) {
    postConvexHullUpdate(p->first);
  }
}


//------------------------------------------------------------
// Procedure: postConvexHullUpdate

void ObstacleManager::postConvexHullUpdate(string obstacle_key)
{
  if(!m_map_hull_changed[obstacle_key])
    return;
  
  XYPolygon poly = m_map_convex_hull[obstacle_key];
  string poly_str = poly.get_spec();
  Notify("VIEW_POLYGON", poly_str);

  string update_var = m_map_updates[obstacle_key];
  string update_str = poly.get_spec_pts(2) + ",label=" + obstacle_key;
  
  Notify(update_var, update_str);
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
  m_msgs << "  Most Recent Turn: " << m_most_recent_turn   << endl;
  m_msgs << "  Clusters:         " << m_map_points.size()  << endl;

  return(true);
}



