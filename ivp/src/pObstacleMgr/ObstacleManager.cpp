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
  m_obstacle_alert_var = "OBSTACLE_ALERT";
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
  postConvexHulls();
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

  string key = newpt.get_msg();
  if(key == "")
    key = newpt.get_label();

  m_map_points[key].push_back(newpt);
  m_map_points_changed[key] = true;
  m_map_points_total[key]++;

  m_points.push_back(newpt);

  unsigned int vsize = m_points.size();
  if(vsize >= 3) {
    double x0 = m_points[vsize-3].x();
    double y0 = m_points[vsize-3].y();
    double x1 = m_points[vsize-2].x();
    double y1 = m_points[vsize-2].y();
    double x2 = m_points[vsize-1].x();
    double y2 = m_points[vsize-1].y();
    bool turn_left = threePointTurnLeft(x0,y0,x1,y1,x2,y2);
    if(turn_left)
      m_most_recent_turn = "left!";
    else
      m_most_recent_turn = "non-left!";
  }
  else
    m_most_recent_turn = "too short";

  return(true);
}

//------------------------------------------------------------
// Procedure: postConvexHulls

void ObstacleManager::postConvexHulls()
{
  map<string, vector<XYPoint> >::iterator p;
  for(p=m_map_points.begin(); p!=m_map_points.end(); p++) {
    postConvexHull(p->first, p->second);
  }
}


//------------------------------------------------------------
// Procedure: postConvexHull

void ObstacleManager::postConvexHull(const string& cluster_label, 
				     const vector<XYPoint>& points)
{
  unsigned int vsize = points.size();
  if(vsize < 3)
    return;

  // If no new points for the give cluster label have been received 
  // since the last time a posting was made, don't make a posting now.
  if(m_map_points_changed[cluster_label] == false)
    return;
  m_map_points_changed[cluster_label] = false;

  ConvexHullGenerator chgen;
  for(unsigned int i=0; i<vsize; i++) 
    chgen.addPoint(points[i].x(), points[i].y(), points[i].get_label());
  
  XYPolygon poly = chgen.generateConvexHull();
  poly.set_label(cluster_label);
  string poly_str = poly.get_spec();
  Notify("VIEW_POLYGON", poly_str);

  string obstacle_alert = "name=" + cluster_label + "#poly=";
  obstacle_alert += poly.get_spec_pts(2) + ",label=" + cluster_label;
  
  Notify(m_obstacle_alert_var, obstacle_alert);

  //Notify("VIEW_POINT", pt_str);
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



