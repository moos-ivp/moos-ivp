/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: BHV_Lawnmower.cpp                                    */
/*    DATE: MAY 6, 2008                                          */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <stdlib.h>
#include "BHV_Lawnmower.h"
#include "AOF_Waypoint.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Lawnmower::BHV_Lawnmower(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_lawnmower");
  m_domain = subDomain(m_domain, "course,speed");

  m_cruise_speed    = 0;  // Meters/second
  m_lead_distance   = -1;

  // The completed and perpetual vars are initialized in superclass
  // but we initialize here just to be safe and clear.
  m_completed     = false; 
  m_perpetual     = false;

  m_osx   = -1;
  m_osy   = -1;
  m_osv   = -1;
  m_ptx   = -1;
  m_pty   = -1;

  m_trackpt_x = -1;
  m_trackpt_y = -1;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED");

//  this->setParam("descriptor", "(d)bhv_lawnmower");
//  this->setParam("build_info", "uniform_piece=course:6,speed:4");
//  this->setParam("build_info", "uniform_grid=course:12,speed:8");

//  m_domain = subDomain(m_domain, "course,speed");

//  has_fired = false;
//  osX   = 0;
//  osY   = 0;
//  osSPD = 0;

  //addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Lawnmower::setParam(string param, string val) 
{
	if(IvPBehavior::setParamCommon(param, val))
		return(true);

	if((param == "polygon") || (param == "points")) {
		XYSegList new_seglist;
		bool ok = new_seglist.initialize(val);
		if(!ok)
		    return(false);
		m_waypoint_engine.setSegList(new_seglist);
		return(true);
	}
	else if(param == "speed") {
		double dval = atof(val.c_str());
		if((dval <= 0) || (!isNumber(val)))
		    return(false);
		m_cruise_speed = dval;
		return(true);
	}
	else if(param == "lead") {
		double dval = atof(val.c_str());
		if((dval < 0) || (!isNumber(val)))
		    return(false);
		m_lead_distance = dval;
		return(true);
	}
	//  else if(param == "order") {
	//    if((val!="reverse") && (val!="reversed") && (val!="normal"))
	//      return(false);
	//    bool reverse = ((val == "reverse") || (val == "reversed"));
	//    m_waypoint_engine.setReverse(reverse);
	//    return(true);
	//  }
	//  else if(param == "repeat") {
	//    int ival = atoi(val.c_str());
	//    if((ival < 0) || (!isNumber(val)))
	//      return(false);
	//    m_waypoint_engine.setRepeat(ival);
	//    return(true);
	//  }
	else if(param == "radius") {
		double dval = atof(val.c_str());
		if(dval <= 0)
		    return(false);
		m_waypoint_engine.setCaptureRadius(dval);
		return(true);
	}
	else if(param == "nm_radius")  {
		double dval = atof(val.c_str());
		if(dval <= 0) 
		    return(false);
		m_waypoint_engine.setNonmonotonicRadius(dval);
		return(true);
	}
	
	return(false);
}

void BHV_Lawnmower::onIdleState() 
{
	XYSegList seglist = m_waypoint_engine.getSegList();
	seglist.set_label(m_us_name + "_" + m_descriptor);
	string segmsg = seglist.get_spec();
	postMessage("VIEW_SEGLIST", segmsg);

}


//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction* BHV_Lawnmower::onRunState() 
{
	IvPFunction *ipf = NULL;

	bool ok_info = updateFromInfoBuffer();
	if(!ok_info){
		return(NULL);
	}

	// Set m_ptx, m_pty, m_trackpt_x, m_trackpt_y;
	if(!setNextWaypoint()) {
		XYSegList seglist = m_waypoint_engine.getSegList();
		seglist.set_label(m_us_name + "_" + m_descriptor);
		string segmsg = seglist.get_spec();
		postMessage("VIEW_SEGLIST", segmsg);
		return(NULL);
	}

	bool ok = true;
    AOF_Waypoint aof_wpt(m_domain);
    ok = ok && aof_wpt.setParam("desired_speed", m_cruise_speed);
    ok = ok && aof_wpt.setParam("osx", m_osx);
    ok = ok && aof_wpt.setParam("osy", m_osy);
    ok = ok && aof_wpt.setParam("ptx", m_trackpt_x);
    ok = ok && aof_wpt.setParam("pty", m_trackpt_y);
    ok = ok && aof_wpt.initialize();
    
    if(ok) {
      OF_Reflector reflector(&aof_wpt);
      reflector.create(600, 500);
      ipf = reflector.extractOF();
    }
    
	if(ipf)
		ipf->setPWT(m_priority_wt);

	XYSegList seglist = m_waypoint_engine.getSegList();
	seglist.set_label(m_us_name + "_" + m_descriptor);
	string segmsg = seglist.get_spec();
	postMessage("VIEW_SEGLIST", segmsg);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateFromInfoBuffer
//

bool BHV_Lawnmower::updateFromInfoBuffer()
{

	bool ok1, ok2, ok3;
//	bool ok4, ok5, ok6, ok7;
	m_osx = getBufferDoubleVal("NAV_X",     ok1);
	m_osy = getBufferDoubleVal("NAV_Y",     ok2);
	m_osv = getBufferDoubleVal("NAV_SPEED", ok3);

	if(!ok1 || !ok2)
	postEMessage("No ownship X/Y info in info_buffer.");
	if(!ok3)
	postWMessage("No ownship speed info in info_buffer.");

  //cout << "config: " << search_grid.getConfigString() << endl;
    
  // If we have ever fired before, set the "previous" values.
//  if(has_fired) {
//    prev_osX = osX;
//    prev_osY = osY;
//  }
//  else {
//    has_fired = true;
//    return(false);
//  }

//  bool ok1, ok2, ok3, ok4, ok5, ok6, ok7;
//  osX   = getBufferDoubleVal("NAV_X",     ok1);
//  osY   = getBufferDoubleVal("NAV_Y",     ok2);
//  osSPD = getBufferDoubleVal("NAV_SPEED", ok3);
//  osCRS = getBufferDoubleVal("NAV_HEADING", ok4);
//  sensor_a = getBufferDoubleVal("SENSOR_A", ok5);
//  sensor_b = getBufferDoubleVal("SENSOR_B", ok6);

//  // Must get ownship position from InfoBuffer
//  if(!ok1 || !ok2)
//    postEMessage("No ownship X/Y info in info_buffer.");
//  if(!ok3)
//    postWMessage("No ownship speed info in info_buffer.");
//  if(!ok4)
//    postWMessage("No ownship heading info in info_buffer.");
//  if(!ok5 || !ok6)
//    postWMessage("No sensor information in info_buffer.");
//  if( !(ok1 && ok2 && ok3 && ok4 && ok5 && ok6) )
//    return(false);
//    
//  vector<string> updates = getBufferStringVector("ARTIFACTGRID_DELTA", ok7);
//  //postWMessage("Updating ArtifactGrid with " + intToString(updates.size()) + " updates");
//  vector<string>::const_iterator p;
//  for(p = updates.begin(); p != updates.end(); p++){
//  	search_grid.processDelta( *p );
//  }

  return(true);
}

//-----------------------------------------------------------
// Procedure: setNextWaypoint

bool BHV_Lawnmower::setNextWaypoint()
{
  m_waypoint_engine.setNextWaypoint(m_osx, m_osy);

  if(m_waypoint_engine.isComplete()) {
//    postMessage("WPT_STAT", "complete");
//    setComplete();
//    if(m_perpetual)
      m_waypoint_engine.reset();
//    return(false);
  }
  
  m_ptx = m_waypoint_engine.getPointX();
  m_pty = m_waypoint_engine.getPointY();
  
  // By default, the steering point is the next waypoint.
  m_trackpt_x = m_ptx;
  m_trackpt_y = m_pty;

  // If m_lead_distance is non-neg, and we've already hit the
  // first waypoint, then steer to an intermediate point that
  // is m_lead_distance away from the perpendicular intersection
  // point between the current position and the trackline.
  
  int current_waypt = m_waypoint_engine.getCurrIndex();
  if(m_lead_distance >= 0) {
    if(current_waypt > 0) {
      double x1 = m_waypoint_engine.getPointX(current_waypt-1);
      double y1 = m_waypoint_engine.getPointY(current_waypt-1);

      double nx, ny;
      perpSegIntPt(x1,y1,m_ptx,m_pty,m_osx,m_osy,nx,ny);

      double angle = relAng(x1, y1, m_ptx, m_pty);
      double dist  = distPointToPoint(nx, ny, m_ptx, m_pty);
      if(dist > m_lead_distance) 
	dist = m_lead_distance; 
      projectPoint(angle, dist, nx, ny, m_trackpt_x, m_trackpt_y);
    }
  }
  return(true);
}
