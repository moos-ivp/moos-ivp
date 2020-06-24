/*****************************************************************/
/*    NAME: Michael R. Benjamin                                  */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CollObDetect.cpp                                     */
/*    DATE: September 2nd, 2019                                  */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "ACTable.h"
#include "CollObDetect.h"
#include "FileBuffer.h"
#include "ColorParse.h"
#include "XYFormatUtilsPoly.h"
#include "NodeRecordUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CollObDetect::CollObDetect()
{
  // Init Config variables
  m_near_miss_dist = 10;
  m_collision_dist = 5;
  m_encounter_dist = 30;
  
  // Init State variables
  m_total_encounters = 0;
  m_total_near_misses = 0;
  m_total_collisions = 0;

  m_global_min_dist = -1;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CollObDetect::OnNewMail(MOOSMSG_LIST &NewMail)
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

    if(key == "KNOWN_OBSTACLE")
      handleMailKnownObstacle(sval);
    
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

bool CollObDetect::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool CollObDetect::Iterate()
{
  AppCastingMOOSApp::Iterate();

  updateVehiDists();
  updateVehiMinDists();
  updateVehiEncounters();
  
   AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool CollObDetect::OnStartUp()
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
    if(param == "collision_dist")
      handled = setNonNegDoubleOnString(m_collision_dist, value);
    else if(param == "near_miss_dist")
      handled = setNonNegDoubleOnString(m_near_miss_dist, value);
    else if(param == "encounter_dist")
      handled = setNonNegDoubleOnString(m_encounter_dist, value);
    else if(param == "collision_flag") 
      handled = handleConfigFlag("collision", value);
    else if(param == "near_miss_flag") 
      handled = handleConfigFlag("near_miss", value);
    else if(param == "encounter_flag") 
      handled = handleConfigFlag("encounter", value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  if(m_collision_dist > m_near_miss_dist) {
    m_near_miss_dist = m_collision_dist;
    reportConfigWarning("near_miss_dist cannot be < collision_dist");
  }    
  if(m_near_miss_dist > m_encounter_dist) {
    m_encounter_dist = m_near_miss_dist;
    reportConfigWarning("encounter_dist cannot be < near_miss_dist");
  }    
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void CollObDetect::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("KNOWN_OBSTACLE", 0);
  Register("NODE_REPORT", 0);
}

//------------------------------------------------------------
// Procedure: handleConfigFlag()

bool CollObDetect::handleConfigFlag(string flag_type, string str)
{
  string moosvar = biteStringX(str, '=');
  string moosval = str;

  if((moosvar == "") || (moosval == ""))
    return(false);
  
  VarDataPair pair(moosvar, moosval, "auto");
  if(flag_type == "collision")
    m_collision_flags.push_back(pair);
  else if(flag_type == "near_miss")
    m_near_miss_flags.push_back(pair);
  else if(flag_type == "encounter")
    m_encounter_flags.push_back(pair);
  else
    return(false);

  return(true);
}


//------------------------------------------------------------
// Procedure: handleMailKnownObstacle()
//            Known obstacles may come from (a) simulation,
//            (b) simulated sensors, (c) known locations e.g. of
//            of buoys when operatingin the field, or (d) via
//            actual sensors when operatingin the field.
//
//   Example: pts={90.2,-80.4:...:82,-88:82.1,-83.7:85.4,-80.4},label=ob_0

bool CollObDetect::handleMailKnownObstacle(string poly)
{
  XYPolygon new_poly = string2Poly(poly);
  if(!new_poly.is_convex())
    return(false);

  string label = new_poly.get_label();

  m_map_obstacles[label] = new_poly;
  return(true);
}

//------------------------------------------------------------
// Procedure: setCurrDist()

void CollObDetect::setCurrDist(string vname, string label, double dist)
{
  m_map_vdist[vname][label] = dist;
}

//------------------------------------------------------------
// Procedure: setPrevDist()

void CollObDetect::setPrevDist(string vname, string label, double dist)
{
  m_map_vdist_prev[vname][label] = dist;
}

//------------------------------------------------------------
// Procedure: setMinDist()

void CollObDetect::setMinDist(string vname, string label, double dist)
{
  m_map_vdist_min[vname][label] = dist;
}

//------------------------------------------------------------
// Procedure: getCurrDist()

double CollObDetect::getCurrDist(string vname, string label)
{
  if(m_map_vdist.count(vname)) 
    if(m_map_vdist[vname].count(label))
      return(m_map_vdist[vname][label]);
  return(-1);
}

//------------------------------------------------------------
// Procedure: getPrevDist()

double CollObDetect::getPrevDist(string vname, string label)
{
  if(m_map_vdist_prev.count(vname))
    if(m_map_vdist_prev[vname].count(label))
      return(m_map_vdist_prev[vname][label]);
  return(-1);
}

//------------------------------------------------------------
// Procedure: getMinDist()

double CollObDetect::getMinDist(string vname, string label)
{
  if(m_map_vdist_min.count(vname))
    if(m_map_vdist_min[vname].count(label))
      return(m_map_vdist_min[vname][label]);
  return(-1);
}

  
//------------------------------------------------------------
// Procedure: handleMailNodeReport()

bool CollObDetect::handleMailNodeReport(string node_report)
{
  NodeRecord record = string2NodeRecord(node_report);
  if(!record.valid())
    return(false);
  string vname = record.getName();

  // Update the node record list for this vehicle
  m_map_vrecords[vname] = record;
  return(true);
}

//------------------------------------------------------------
// Procedure: updateVehiDists()

void CollObDetect::updateVehiDists()
{
  // Part 1: Store curr distances as "previouse"
  m_map_vdist_prev = m_map_vdist;

  // Part 2: Update current distance to obstacles for all vehicles 
  map<string, NodeRecord>::iterator p;
  map<string, XYPolygon>::iterator q;

  for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
    string vname = p->first;
    NodeRecord record = p->second;
    double vx = record.getX(); 
    double vy = record.getY(); 
    for(q=m_map_obstacles.begin(); q!=m_map_obstacles.end(); q++) {
      string label = q->first;
      XYPolygon poly = q->second;
      // The dist_to_poly function returns distance to closest edge
      // so a point IN the poly my have a positive distance. So we
      // check explicitly that the point is not within the poly.
      double dist = 0;
      if(!poly.contains(vx, vy))
	dist = poly.dist_to_poly(vx, vy);
      setCurrDist(vname, label, dist);
      if((m_global_min_dist < 0) || (dist < m_global_min_dist))
	m_global_min_dist = dist;
    }
  }
}

//------------------------------------------------------------
// Procedure: updateVehiMinDists()

void CollObDetect::updateVehiMinDists()
{
  // Part 1: Update the min_dist for each vehi/label if the current
  // distance is less than the previously noted min distance.
  // Note: A min_dist of -1 indicates that min_dist is virgin.
  
  map<string, NodeRecord>::iterator p;
  map<string, XYPolygon>::iterator q;

  for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
    string vname = p->first;
    for(q=m_map_obstacles.begin(); q!=m_map_obstacles.end(); q++) {
      string label = q->first;
      double curr_dist = getCurrDist(vname, label);
      double min_dist = getMinDist(vname, label);
      if((min_dist < 0) || (curr_dist < min_dist))
	setMinDist(vname, label, curr_dist);
    }
  }
}

//------------------------------------------------------------
// Procedure: updateVehiEncounters()
//   Purpose: o For each vehicle and obstacle, examine curr_dist
//              vs prev_dist. If curr_dist is greater than
//              encounter_dist, and prev_dist was less than
//              encounter_dist, then we will say that an 
//              encounter has just been completed.

//            o Having completed an encounter, we look at stored
//              val of min_dist (how close did we get to the
//              obstacle?)
//            o Depending on the value, post the proper flags,
//              and reset the min_dist for that vehicle/obstacle
//              pair.

void CollObDetect::updateVehiEncounters()
{
  map<string, NodeRecord>::iterator p;
  map<string, XYPolygon>::iterator q;

  for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
    string vname = p->first;
    for(q=m_map_obstacles.begin(); q!=m_map_obstacles.end(); q++) {
      string label = q->first;
      double prev_dist = getPrevDist(vname, label);
      double curr_dist = getCurrDist(vname, label);
      if((prev_dist > 0) && (prev_dist <= m_encounter_dist)) {	       
	if(curr_dist > m_encounter_dist) {
	  double min_dist = getMinDist(vname, label);
	  if(min_dist < m_collision_dist) {
	    m_total_collisions++;
	    postFlags(m_collision_flags, min_dist, vname, label);
	  }
	  else if(min_dist < m_near_miss_dist) {
	    m_total_near_misses++;
	    postFlags(m_near_miss_flags, min_dist, vname, label);
	  }
	  
	  m_total_encounters++;
	  postFlags(m_encounter_flags, min_dist, vname, label);
	  setMinDist(vname, label, -1);
	}
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: postFlags

void CollObDetect::postFlags(const vector<VarDataPair>& flags,
			    double dist, string vname, string label)
{
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string moosvar = pair.get_var();
    string postval;

    // Handle Variable macro expansion
    moosvar = findReplace(moosvar, "$VNAME", vname);
    moosvar = findReplace(moosvar, "$UP_VNAME", toupper(vname));
    moosvar = findReplace(moosvar, "$ID", label);

    // If posting is a double, just post. No macro expansion
    if(!pair.is_string()) {
      double dval = pair.get_ddata();
      postval = doubleToStringX(dval,4);
      Notify(moosvar, dval);
    }
    // Otherwise if string posting, handle macro expansion
    else {
      string sval = pair.get_sdata();
      string dist_str = doubleToStringX(dist, 2);

      string enc_str  = uintToString(m_total_encounters);
      string miss_str = uintToString(m_total_near_misses);
      string coll_str = uintToString(m_total_collisions);
      
      // If the string is just $DIST interpret as a double posting
      if(sval == "$DIST") {
	postval = doubleToStringX(dist, 4);
	Notify(moosvar, dist);
      }
      else if(sval == "$ENC_CNT") {
	postval = enc_str;
	Notify(moosvar, m_total_encounters);
      }
      else if(sval == "$COLL_CNT") {
	postval = coll_str;
	Notify(moosvar, m_total_collisions);
      }
      else if(sval == "$MISS_CNT") {
	postval = miss_str;
	Notify(moosvar, m_total_near_misses);
      }
      else {
	sval = findReplace(sval, "$VNAME", vname);
	sval = findReplace(sval, "$UP_VNAME", toupper(vname));
	sval = findReplace(sval, "$DIST", dist_str);
	sval = findReplace(sval, "$ID", label);
	sval = findReplace(sval, "$ENC_CNT", enc_str);
	sval = findReplace(sval, "$COLL_CNT", coll_str);
	sval = findReplace(sval, "$MISS_CNT", miss_str);
	postval = sval;
	Notify(moosvar, sval);
      }
    }
    reportEvent(moosvar + "=" + postval);
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool CollObDetect::buildReport() 
{
  
  // ==============================================================
  // Part 1: Overview
  // ==============================================================  
  
  string str_obstacles_amt = uintToString(m_map_obstacles.size()); 
  string str_collision_dist = doubleToStringX(m_collision_dist);
  string str_near_miss_dist = doubleToStringX(m_near_miss_dist);
  string str_encounter_dist = doubleToStringX(m_encounter_dist);

  string str_encounters  = uintToString(m_total_encounters);
  string str_near_misses = uintToString(m_total_near_misses);
  string str_collisions  = uintToString(m_total_collisions);
  string str_global_dist = doubleToStringX(m_global_min_dist,1);

  
  m_msgs << "============================================" << endl;
  m_msgs << "Configuration:                              " << endl;
  m_msgs << "  Obstacles: " << str_obstacles_amt       << endl;
  m_msgs << "  Collision Dist: " << str_collision_dist << endl;
  m_msgs << "  Near Miss Dist: " << str_near_miss_dist << endl;
  m_msgs << "  Encounter Dist: " << str_encounter_dist << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "State:                                  " << endl;
  m_msgs << "  Total Encounters:  " << str_encounters  << endl;
  m_msgs << "  Total Near Misses: " << str_near_misses << endl;
  m_msgs << "  Total Collisions:  " << str_collisions  << endl;
  m_msgs << "  Global Min Dist:   " << str_global_dist << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Vehicle/Obstacle Curr Distances:            " << endl;
  m_msgs << "============================================" << endl;
  
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

  map<string, XYPolygon>::iterator q;
  for(q=m_map_obstacles.begin(); q!=m_map_obstacles.end(); q++) {
    string label = q->first;
    for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
      string vname = p->first;
      string sdist = "-";
      double dist = getCurrDist(vname, label);
      if(dist >= 0) {
	sdist = doubleToStringX(dist,1);
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

  for(q=m_map_obstacles.begin(); q!=m_map_obstacles.end(); q++) {
    string label = q->first;
    for(p=m_map_vrecords.begin(); p!=m_map_vrecords.end(); p++) {
      string vname = p->first;
      string sdist = "-";
      if(m_map_vdist_min.count(vname)) {
	if(m_map_vdist_min[vname].count(label)) {
	  double dist = getMinDist(vname, label);
	  sdist = doubleToStringX(dist,1);
	}
      }
      actab2 << sdist;
    }
  }
  m_msgs << actab2.getFormattedString();
  return(true);
}

