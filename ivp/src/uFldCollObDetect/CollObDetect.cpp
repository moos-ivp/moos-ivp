/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CollObDetect.cpp                                     */
/*    DATE: September 2nd, 2019                                  */
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
#include "GeomUtils.h"
#include "ACTable.h"
#include "CollObDetect.h"
#include "FileBuffer.h"
#include "ColorParse.h"
#include "XYFormatUtilsPoly.h"
#include "NodeRecordUtils.h"
#include "VarDataPairUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

CollObDetect::CollObDetect()
{
  // Init Config variables
  m_near_miss_dist = 5;
  m_collision_dist = 1;
  m_encounter_dist = 15;

  m_bin_delta = -1;
  m_bin_minval = -1;
  
  // Init State variables
  m_total_encounters = 0;
  m_total_near_misses = 0;
  m_total_collisions = 0;

  m_global_min_dist = -1;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

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

    if(key == "KNOWN_OBSTACLE") {
      bool ok = handleMailKnownObstacle(sval);
      if(!ok) 
	reportRunWarning("Unhandled KNOWN_OBSTACLE:" + sval);    
    }
    
    else if(key == "KNOWN_OBSTACLE_CLEAR")
      handleMailKnownObstacleClear(sval);
    
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
// Procedure: OnConnectToServer()

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
    else if(param == "bin_min_val")
      handled = setNonNegDoubleOnString(m_bin_minval, value);
    else if(param == "bin_delta")
      handled = setPosDoubleOnString(m_bin_delta, value);
    else if(param == "near_miss_dist")
      handled = setNonNegDoubleOnString(m_near_miss_dist, value);
    else if(param == "encounter_dist")
      handled = setNonNegDoubleOnString(m_encounter_dist, value);
    else if(param == "collision_flag") 
      handled = addVarDataPairOnString(m_collision_flags, value);
    else if(param == "near_miss_flag") 
      handled = addVarDataPairOnString(m_near_miss_flags, value);
    else if(param == "encounter_flag") 
      handled = addVarDataPairOnString(m_encounter_flags, value);

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
// Procedure: registerVariables()

void CollObDetect::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("KNOWN_OBSTACLE", 0);
  Register("KNOWN_OBSTACLE_CLEAR", 0);
  Register("NODE_REPORT", 0);
}

//------------------------------------------------------------
// Procedure: handleMailKnownObstacle()
//            Known obstacles may come from (a) simulation,
//            (b) simulated sensors, (c) known locations e.g. of
//            of buoys when operatingin the field, or (d) via
//            actual sensors when operating in the field.
//
//   Example: pts={90.2,-80.4:...:82,-88:82.1,-83.7:85.4,-80.4},label=ob_0

bool CollObDetect::handleMailKnownObstacle(string poly)
{
  XYPolygon new_poly = string2Poly(poly);
  if(!new_poly.is_convex())
    return(false);

  string label = new_poly.get_label();

  m_map_obstacles[label] = new_poly;
  m_map_ob_tstamp[label] = m_curr_time;
  return(true);
}

//------------------------------------------------------------
// Procedure: handleMailKnownObstacleClear()
//      Note: May occur when used with an obstacle simulator that
//            is resetting the obstacle field mid-mission.
//      Note: Timestamps are checked and a threshold is applied before
//            clearing an obstacle. This is to ensure the clear-and-
//            replace event is more robust to the possibility that the
//            clear message arrives after one of the new replacement
//            obstacles.
//   Example: "all", "obs_001"

void CollObDetect::handleMailKnownObstacleClear(string str)
{
  if(str == "")
    return;

  double delete_time_thresh = 4;
  
  set<string> remove_ids;
  
  map<string, double>::iterator p;
  for(p=m_map_ob_tstamp.begin(); p!=m_map_ob_tstamp.end(); p++) {
    string id = p->first;
    double tstamp = p->second;
    if((id == str) || (tolower(str) == "all")) {
      double age = m_curr_time - tstamp;
      if(age > delete_time_thresh)
	remove_ids.insert(id);
    }
  }

  set<string>::iterator q;
  for(q=remove_ids.begin(); q!=remove_ids.end(); q++) {
    string obstacle_id = *q;
    m_map_obstacles.erase(obstacle_id);
    m_map_vdist.erase(obstacle_id);
    m_map_vdist_prev.erase(obstacle_id);
    m_map_vdist_min.erase(obstacle_id);
  }
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
  // Part 1: Store curr distances as "previous"
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
      string obstacle_label = q->first;
      XYPolygon poly = q->second;
      // The dist_to_poly function returns distance to closest edge
      // so a point IN the poly my have a positive distance. So we
      // check explicitly that the point is not within the poly.
      double dist = 0;
      if(!poly.contains(vx, vy))
	dist = poly.dist_to_poly(vx, vy);
      setCurrDist(vname, obstacle_label, dist);
      if((m_global_min_dist < 0) || (dist < m_global_min_dist))
	m_global_min_dist = dist;
    }
  }
}

//------------------------------------------------------------
// Procedure: updateVehiMinDists()

void CollObDetect::updateVehiMinDists()
{
  // Part 1: Update the min_dist for each vname/obstacle_label if the
  // current distance is less than the previously noted min distance.
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
//              vs prev_dist. If prev_dist is less than
//              encounter_dist, and curr_dist is greater than
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

	  binEncounter(min_dist);

	  m_total_encounters++;
	  postFlags(m_encounter_flags, min_dist, vname, label);
	  setMinDist(vname, label, -1);
	}
      }
    }
  }
}

//------------------------------------------------------------
// Procedure: binEncounter()

void CollObDetect::binEncounter(double dist)
{
  if((m_bin_minval < 0) || (m_bin_delta <= 0))
    return;
  
  double bin_val = snapToStep(dist, m_bin_delta);
  m_map_bins[bin_val]++;
  m_map_tbins[bin_val]++;

  map<double, unsigned int>::iterator p;
  for(p=m_map_tbins.begin(); p!=m_map_tbins.end(); p++) {
    double bval = p->first;
    if(bval > bin_val)
      m_map_tbins[bval]++;
  }
}

//------------------------------------------------------------
// Procedure: postEncounterBin()

void CollObDetect::postEncounterBin(double dist)
{
  double bin_val = snapToStep(dist, m_bin_delta);

  string sbin_val = doubleToStringX(bin_val);
  sbin_val = findReplace(sbin_val, ".", "P");

  string moos_var = "NMISS_" + sbin_val;
  Notify(moos_var, m_map_bins[bin_val]);
}

//------------------------------------------------------------
// Procedure: postFlags()

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
  // Part 2: Build table of bin values
  // ==============================================================  
  ACTable actab1(3);

  // Build header string, e.g., "abe | ben | cal | deb"

  string header1_str = "Thresh | Count | Total";
  actab1 << header1_str;
  actab1.addHeaderLines();
  map<double, unsigned int>::iterator pp;
  for(pp=m_map_bins.begin(); pp!=m_map_bins.end(); pp++) {
    double thresh = pp->first;
    unsigned int count = pp->second;
    unsigned int tcount = m_map_tbins[thresh];
    actab1 << doubleToStringX(thresh,2);
    actab1 << count;
    actab1 << tcount;
  }
  m_msgs << actab1.getFormattedString() << endl << endl;

  
  // ==============================================================
  // Part 3: Build a table of min distances to obstacles
  // ==============================================================
  unsigned int vcount = m_map_vrecords.size();
  ACTable actab2(vcount);

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
  actab2 << header_str;
  actab2.addHeaderLines();

  map<string, XYPolygon>::iterator q;
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


