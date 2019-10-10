/*****************************************************************/
/*    NAME: Michael R. Benjamin                                  */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleSim.cpp                                      */
/*    DATE: October 19th, 2017                                   */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "ACTable.h"
#include "ObstacleSim.h"
#include "FileBuffer.h"
#include "ColorParse.h"
#include "XYFormatUtilsPoly.h"
#include "NodeRecordUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ObstacleSim::ObstacleSim()
{
  // Init Config variables
  m_min_range = 0;      
  m_min_poly_size = 0;
  m_max_poly_size = 0;

  m_poly_fill_color  = "white";
  m_poly_edge_color  = "gray50";
  m_poly_vert_color  = "gray50";
  m_poly_label_color = "invisible";

  m_poly_transparency = 0.15;
  m_poly_edge_size   = 1;
  m_poly_vert_size   = 1;

  m_post_points = false;
  m_rate_points = 5;

  m_near_miss_dist = 10;
  m_collision_dist = 5;
  m_encounter_dist = 30;
  
  // Init State variables
  m_viewables_queried = true;
  m_obstacles_queried = true;

  m_viewables_posted = 0;
  m_obstacles_posted = 0;

  m_total_encounters = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ObstacleSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    
    if(key=="PMV_CONNECT")
      m_viewables_queried = true;
    else if(key=="VEHICLE_CONNECT")
      m_obstacles_queried = true;
    else if(key=="NODE_REPORT") {
      bool ok = handleMailNodeReport(sval);
      if(!ok) 
	reportRunWarning("Unhandled Node Report:" + sval);    
    }
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
	
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ObstacleSim::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool ObstacleSim::Iterate()
{
  AppCastingMOOSApp::Iterate();

  updateVehiDists();
  
  if(m_viewables_queried) {
    postViewableObstacles();
    m_viewables_queried = false;
  }

  if(m_post_points)
    postPoints();
  else if(m_obstacles_queried) {
    postKnownObstacles();
    m_obstacles_queried = false;
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ObstacleSim::OnStartUp()
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
    if(param == "obstacle_file") 
      handled = processObstacleFile(value);
    else if((param == "poly_vert_color") && isColor(value))
      handled = setColorOnString(m_poly_vert_color, value);
    else if((param == "poly_fill_color") && isColor(value))
      handled = setColorOnString(m_poly_fill_color, value);
    else if((param == "poly_edge_color") && isColor(value))
      handled = setColorOnString(m_poly_edge_color, value);
    else if((param == "poly_label_color") && isColor(value))
      handled = setColorOnString(m_poly_label_color, value);
    else if(param == "post_points")
      handled = setBooleanOnString(m_post_points, value);
    else if(param == "rate_points")
      handled = setNonNegDoubleOnString(m_rate_points, value);
    else if(param == "rate_pointsx")
      handled = setNonNegDoubleOnString(m_rate_points, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void ObstacleSim::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("PMV_CONNECT", 0);
  Register("VEHICLE_CONNECT", 0);
  Register("NODE_REPORT", 0);
}


//------------------------------------------------------------
// Procedure: handleMailNodeReport()

bool ObstacleSim::handleMailNodeReport(string node_report)
{
  NodeRecord record = string2NodeRecord(node_report);
  if(!record.valid())
    return(false);
  string vname = record.getName();

  // Part 1: Update the node record list for this vehicle
  m_map_vrecords[vname] = record;
  return(true);
}

//------------------------------------------------------------
// Procedure: updateVehiDists()

void ObstacleSim::updateVehiDists()
{
  // Part 1: Update current distance to obstacles for all vehicles 
  map<string, vector<double> > m_map_vdists_new;

  unsigned int num_obstacles = m_obstacles.size();
  
  map<string, NodeRecord>::iterator p;
  for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
    string vname = p->first;
    NodeRecord record = p->second;
    vector<double> init_vector(num_obstacles, 0);
    m_map_vdists_new[vname] = init_vector;
    for(unsigned int i=0; i<num_obstacles; i++) {
      double vx = record.getX(); 
      double vy = record.getY(); 
      double dist = m_obstacles[i].dist_to_poly(vx, vy);
      m_map_vdists_new[vname][i] = dist;
    }
  }

  // Part 2: Check if any of the vehicles have transitioned from
  // being within the encounter range to outside. In this case we
  // want to note the min distance and handle accordingly.
  for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
    string vname = p->first;
    for(unsigned int i=0; i<num_obstacles; i++) {
      if(m_map_vdists.count(vname) &&
	 m_map_vdists_new.count(vname) &&
	 m_map_vdists_min.count(vname) &&
	 (i < m_map_vdists[vname].size()) &&
	 (i < m_map_vdists_new[vname].size())) {
	// Check if Encounter has completed
	if((m_map_vdists[vname][i] < m_encounter_dist) &&
	   (m_map_vdists_new[vname][i] >= m_encounter_dist)) {
	  double min_dist = m_map_vdists_min[vname][i];
	  if(min_dist < m_collision_dist) 
	    postFlags(m_collision_flags, min_dist, vname, i);
	  else if(min_dist < m_near_miss_dist) 
	    postFlags(m_near_miss_flags, min_dist, vname, i);	 

	  m_total_encounters++;
	  postFlags(m_encounter_flags, min_dist, vname, i);
	  m_map_vdists_min[vname][i] = -1;
	}
      }
    }
  }

  m_map_vdists = m_map_vdists_new;

  // Part 2: If the vdists_min table has not been previously built
  // then make a new table, same size as the vdists table, with all
  // cells initialized to -1;
  if(m_map_vdists_min.size() == 0) {
    for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
      string vname = p->first;
      vector<double> init_vector(num_obstacles, -1);
      m_map_vdists_min[vname] = init_vector;
    }
  }
  
  // Part 3: Update min observed dist for each vehicle and obstacle
  for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
    string vname = p->first;
    // Check if num obstacles is the same
    if(m_map_vdists[vname].size() == m_map_vdists_min[vname].size()) {
      for(unsigned int i=0; i<m_map_vdists[vname].size(); i++) {
	if((m_map_vdists_min[vname][i] == -1) ||
	   (m_map_vdists[vname][i] < m_map_vdists_min[vname][i]))
	  if(m_map_vdists[vname][i] <= m_encounter_dist)
	    m_map_vdists_min[vname][i] = m_map_vdists[vname][i];
      }
    }
  }
    


}

//------------------------------------------------------------
// Procedure: postFlags

void ObstacleSim::postFlags(const vector<VarDataPair>& flags,
			    double dist, string vname, unsigned int ix)
{
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string moosvar = pair.get_var();

    // Handle Variable macro expansion
    moosvar = findReplace(moosvar, "$VNAME", vname);
    moosvar = findReplace(moosvar, "$UP_VNAME", toupper(vname));
    moosvar = findReplace(moosvar, "$IX", uintToString(ix));
    //moosvar = findReplace(moosvar, "$IDX", uintToString(m_total_encounters));

    // If posting is a double, just post. No macro expansion
    if(!pair.is_string()) {
      double dval = pair.get_ddata();
      Notify(moosvar, dval);
    }
    // Otherwise if string posting, handle macro expansion
    else {
      string sval = pair.get_sdata();
      string dist_str = doubleToStringX(dist, 2);

      // If the string is just $DIST interpret as a double posting
      if(sval == "$DIST")
	Notify(moosvar, dist);
      else if(sval == "$IDX") 
	Notify(moosvar, m_total_encounters);
      else {
	sval = findReplace(sval, "$VNAME", vname);
	sval = findReplace(sval, "$UP_VNAME", toupper(vname));
	sval = findReplace(sval, "$DIST", dist_str);
	sval = findReplace(sval, "$IDX", uintToString(m_total_encounters));
	Notify(moosvar, sval);
      }
    }
  }
}


//------------------------------------------------------------
// Procedure: processObstacleFile

bool ObstacleSim::processObstacleFile(string filename)
{
  vector<string> lines = fileBuffer(filename);
  unsigned int i, vsize = lines.size();
  if(vsize == 0) {
    reportConfigWarning("Error reading: " + filename);
    return(false);
  }

  for(i=0; i<vsize; i++) {
    string line = stripBlankEnds(lines[i]);
    if((line == "") || strBegins(line, "//"))
      continue;
    
    string left  = biteStringX(line, '=');
    string right = line;
    if(left == "region") {
      XYPolygon region = string2Poly(right);
      if(!region.is_convex()) {
	reportConfigWarning("Poorly specified region: " + right);
	return(false);
      }
      m_poly_region = region;
    }
    else if(left == "min_range") {
      if(isNumber(right)) 
	m_min_range = atof(right.c_str());
      else
	reportConfigWarning("Poorly specified min_range: " + right);
    }
    else if(left == "min_size") {
      if(isNumber(right)) 
	m_min_poly_size = atof(right.c_str());
      else
	reportConfigWarning("Poorly specified min_size: " + right);
    }
    else if(left == "max_size") {
      if(isNumber(right)) 
	m_max_poly_size = atof(right.c_str());
      else
	reportConfigWarning("Poorly specified min_size: " + right);
    }
    else if(left == "poly") {
      XYPolygon poly = string2Poly(right);
      if(!poly.is_convex()) {
	reportConfigWarning("Poorly specified obstacle: " + right);
	return(false);
      }
      poly.set_color("edge", m_poly_edge_color);
      poly.set_color("vertex", m_poly_vert_color);
      poly.set_color("fill", m_poly_fill_color);
      poly.set_color("label", m_poly_label_color);
      poly.set_vertex_size(m_poly_vert_size);
      poly.set_edge_size(m_poly_edge_size);
      poly.set_transparency(m_poly_transparency);
      m_obstacles.push_back(poly);
    }
  }
  return(true);
}

//------------------------------------------------------------
// Procedure: postViewableObstacles()

void ObstacleSim::postViewableObstacles()
{
  if(!m_post_points) {
    for(unsigned int i=0; i<m_obstacles.size(); i++) {
      string spec = m_obstacles[i].get_spec();
      Notify("VIEW_POLYGON", spec);
    }
  }

  if(m_poly_region.is_convex())
    Notify("VIEW_POLYGON", m_poly_region.get_spec());
  m_viewables_posted++;

}

//------------------------------------------------------------
// Procedure: postKnownObstacles()

void ObstacleSim::postKnownObstacles()
{
  for(unsigned int i=0; i<m_obstacles.size(); i++) {
    string spec = m_obstacles[i].get_spec();
    Notify("GIVEN_OBSTACLE", spec);
  }
  m_obstacles_posted++;
}

//------------------------------------------------------------
// Procedure: postPoints()
//      Note: Points are published as:
//            TRACKED_FEATURE = x=5,y=8,label=key,size=4,color=1

void ObstacleSim::postPoints()
{
  for(unsigned int i=0; i<m_obstacles.size(); i++) {
    for(unsigned int j=0; j<m_rate_points; j++) {
      double x, y;
      bool ok = randPointInPoly(m_obstacles[i], x, y);
      if(ok) {
	string key = m_obstacles[i].get_label();
	string msg = "x=" + doubleToStringX(x,2);
	msg += ",y=" + doubleToStringX(y,2);
	msg += ",label=" + key + ",color=1,size=2";
	Notify("TRACKED_FEATURE", msg);
	reportEvent("TRACKED_FEATURE="+msg);
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: buildReport()

bool ObstacleSim::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "Configuration:                              " << endl;
  m_msgs << "  Obstacles: " << uintToString(m_obstacles.size()) << endl;
  m_msgs << "  MinRange:  " << doubleToStringX(m_min_range)     << endl;
  m_msgs << "  MinSize:   " << doubleToStringX(m_min_poly_size) << endl;
  m_msgs << "  MaxSize:   " << doubleToStringX(m_max_poly_size) << endl << endl;

  m_msgs << "  Post Points:   " << boolToString(m_post_points) << endl;
  m_msgs << "  Rate Points:   " << doubleToStringX(m_rate_points) << endl;

  m_msgs << "State:                                      " << endl;
  m_msgs << "  Viewables Posted: " << uintToString(m_viewables_posted) << endl;
  m_msgs << "  Obstacles Posted: " << uintToString(m_obstacles_posted) << endl;
  m_msgs << "  Total Encounters: " << uintToString(m_total_encounters) << endl;

  // ==============================================================
  // Part 2: Build a table of current distances to obstacles
  // ==============================================================  
  unsigned int vcount = m_map_vrecords.size();
  ACTable actab(vcount);

  // Build header string, e.g., "abe | ben | cal | deb"
  string header_str;
  map<string, NodeRecord>::iterator p;
  for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
    string vname = p->first;
    if(p==m_map_vrecords.begin())
      header_str = vname;
    else
      header_str += " | " + vname;
  }
  actab << header_str;
  actab.addHeaderLines();

  for(unsigned int i=0; i<m_obstacles.size(); i++) {
    for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
      string vname = p->first;
      string sdist = "-";
      if(m_map_vdists.count(vname)) {
	if(i < m_map_vdists[vname].size()) {
	  double dist = m_map_vdists[vname][i];
	  sdist = doubleToStringX(dist,1);
	}
      }
      actab << sdist;
    }
  }
  m_msgs << actab.getFormattedString() << endl << endl;

  // ==============================================================
  // Part 3: Build a table of min distances to obstacles
  // ==============================================================
  ACTable actab2(vcount);

  // Build header string, e.g., "abe | ben | cal | deb"
  for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
    string vname = p->first;
    if(p==m_map_vrecords.begin())
      header_str = vname;
    else
      header_str += " | " + vname;
  }
  actab2 << header_str;
  actab2.addHeaderLines();

  for(unsigned int i=0; i<m_obstacles.size(); i++) {
    for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
      string vname = p->first;
      string sdist = "-";
      if(m_map_vdists_min.count(vname)) {
	if(i < m_map_vdists_min[vname].size()) {
	  double dist = m_map_vdists_min[vname][i];
	  sdist = doubleToStringX(dist,1);
	}
      }
      actab2 << sdist;
    }
  }
  m_msgs << actab2.getFormattedString();

  return(true);
}

