/*****************************************************************/
/*    NAME: Andrew Shafer, Mike Benjamin                         */
/*    ORGN: MIT                                                  */
/*    FILE: BHV_SearchArtifact.cpp                               */
/*    DATE: NOV 28, 2007                                         */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include "BHV_SearchArtifact.h"
#include "AOF_SearchArtifact.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_SearchArtifact::BHV_SearchArtifact(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_search_artifact_grid");
  this->setParam("build_info", "uniform_piece=course:6,speed:4");
  this->setParam("build_info", "uniform_grid=course:12,speed:8");

  m_domain = subDomain(m_domain, "course,speed");

  has_fired = false;
  osX   = 0;
  osY   = 0;
  osSPD = 0;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING, ARTIFACTGRID_DELTA, SENSOR_A, SENSOR_B");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_SearchArtifact::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "searchgrid") {
    XYArtifactGrid new_search_grid;
    bool ok = new_search_grid.initialize(val);
    if(!ok) 
      return(false);
    search_grid = new_search_grid;
    return(true);
  }
  
  if(param == "time_horizon") {
    double dval = atof(val.c_str());
    if( (!isNumber(val)) || (dval < 0) )
      return(false);
    time_horizon = dval;
    return(true);
  }
  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction *BHV_SearchArtifact::onRunState() 
{
	IvPFunction *ipf = NULL;

	bool ok_info = updateFromInfoBuffer();
	if(!ok_info){
		return(NULL);
	}

	string grid_label = search_grid.getLabel();
	bool   in_grid    = search_grid.containsPoint(osX, osY);

	if(!in_grid || osSPD == 0){
		postMessage("TIME_TO_EXIT_GRID_"+grid_label, 0);
	}
	else {
		XYPolygon poly = search_grid.getPBound();
		double dist_to_exit = poly.dist_to_poly(osX, osY, osCRS);
		double time_to_exit = dist_to_exit / osSPD;
		postMessage("TIME_TO_EXIT_GRID_"+grid_label, time_to_exit);
		postMessage("osSPD", osSPD);
		postMessage("dtg", dist_to_exit);
	}

	AOF_SearchArtifact aof(m_domain, &search_grid);

	aof.setParam("os_lat", osY);
	aof.setParam("os_lon", osX);
	aof.setParam("time_horizon", time_horizon);
	aof.setParam("sensor_a", sensor_a);
	aof.setParam("sensor_b", sensor_b);
	bool ready = aof.initialize();
  
  	if(ready){
  		aof.fillCache();
		
		// 1 for linear pieces
		OF_Reflector reflector(&aof, 1);
		
		reflector.create(m_build_info);
		
		ipf = reflector.extractOF();

	  if(ipf)
	    ipf->setPWT(m_priority_wt);
	}

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateFromInfoBuffer
//

bool BHV_SearchArtifact::updateFromInfoBuffer()
{
  //cout << "config: " << search_grid.getConfigString() << endl;
    
  // If we have ever fired before, set the "previous" values.
  if(has_fired) {
    prev_osX = osX;
    prev_osY = osY;
  }
  else {
    has_fired = true;
    return(false);
  }

  bool ok1, ok2, ok3, ok4, ok5, ok6, ok7;
  osX   = getBufferDoubleVal("NAV_X",     ok1);
  osY   = getBufferDoubleVal("NAV_Y",     ok2);
  osSPD = getBufferDoubleVal("NAV_SPEED", ok3);
  osCRS = getBufferDoubleVal("NAV_HEADING", ok4);
  sensor_a = getBufferDoubleVal("SENSOR_A", ok5);
  sensor_b = getBufferDoubleVal("SENSOR_B", ok6);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2)
    postEMessage("No ownship X/Y info in info_buffer.");
  if(!ok3)
    postWMessage("No ownship speed info in info_buffer.");
  if(!ok4)
    postWMessage("No ownship heading info in info_buffer.");
  if(!ok5 || !ok6)
    postWMessage("No sensor information in info_buffer.");
  if( !(ok1 && ok2 && ok3 && ok4 && ok5 && ok6) )
    return(false);
    
  vector<string> updates = getBufferStringVector("ARTIFACTGRID_DELTA", ok7);
  //postWMessage("Updating ArtifactGrid with " + intToString(updates.size()) + " updates");
  vector<string>::const_iterator p;
  for(p = updates.begin(); p != updates.end(); p++){
  	search_grid.processDelta( *p );
  }

  return(true);
}
