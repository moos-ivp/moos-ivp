/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_SearchGrid.cpp                                   */
/*    DATE: Sep 3rd, 2005                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <stdlib.h>
#include "BHV_SearchGrid.h"
#include "AOF_SearchGrid.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_SearchGrid::BHV_SearchGrid(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_search_grid");
  this->setParam("build_info", "uniform_piece=course:6,speed:4");
  this->setParam("build_info", "uniform_grid=course:12,speed:8");

  m_domain = subDomain(m_domain, "course,speed");

  has_fired = false;
  osX   = 0;
  osY   = 0;
  osSPD = 0;

  addInfoVars("NAV_X, NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_SearchGrid::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);

  if(param == "searchgrid") {
    XYGrid new_search_grid;
    bool ok = new_search_grid.initialize(val);
    if(!ok) 
      return(false);
    search_grid = new_search_grid;
    return(true);
  }
  
  if(param == "timutif") {
    Timutif new_timutif(val);
    bool ok = new_timutif.isConfigured();
    if(!ok) 
      return(false);
    timutif = new_timutif;
    return(true);
  }
  
  if(param == "time_horizon") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    time_horizon = dval;
    return(true);
  }
  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction *BHV_SearchGrid::onRunState() 
{
  IvPFunction *ipf = 0;

  bool ok_info = updateFromInfoBuffer();
  if(!ok_info)
    return(0);

  string grid_label = search_grid.getLabel();
  bool   in_grid    = search_grid.ptIntersect(osX, osY);
  
  if(!in_grid)
    postMessage("TIME_TO_EXIT_GRID_"+grid_label, 0);
  else {
    XYPolygon poly = search_grid.getPBound();
    double dist_to_exit = poly.dist_to_poly(osX, osY, osCRS);
    double time_to_exit = dist_to_exit / osSPD;
    postMessage("TIME_TO_EXIT_GRID_"+grid_label, time_to_exit);
  }    

  bool os_in_grid = updateSearchGrid(prev_osX, prev_osY, osX, osY);

  AOF_SearchGrid aof(m_domain, &search_grid);

  aof.setParam("os_lat", osY);
  aof.setParam("os_lon", osX);
  aof.setParam("time_horizon", time_horizon);
  aof.setParam("timutif", timutif.toString());
  aof.initialize();
  aof.fillCache();

  OF_Reflector reflector(&aof, 1);
  reflector.create(m_build_info);
  ipf = reflector.extractOF();

  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateFromInfoBuffer
//

bool BHV_SearchGrid::updateFromInfoBuffer()
{
  cout << "config: " << search_grid.getConfigString() << endl;
    
  // If we have ever fired before, set the "previous" values.
  if(has_fired) {
    prev_osX = osX;
    prev_osY = osY;
  }
  else {
    has_fired = true;
    postMessage("GRID_CONFIG", search_grid.getConfigString());
    return(false);
  }

  bool ok1, ok2, ok3, ok4;
  osX   = getBufferDoubleVal("NAV_X",     ok1);
  osY   = getBufferDoubleVal("NAV_Y",     ok2);
  osSPD = getBufferDoubleVal("NAV_SPEED", ok3);
  osCRS = getBufferDoubleVal("NAV_HEADING", ok4);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2)
    postEMessage("No ownship X/Y info in info_buffer.");
  if(!ok3)
    postWMessage("No ownship speed info in info_buffer.");
  if(!ok4)
    postWMessage("No ownship heading info in info_buffer.");
  if(!ok1 || !ok2 || !ok3 || !ok4)
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: updateSearchGrid
//
//  GRID_MSG @ type=config @ gname=area23 @ vname=nyak200 @
//      trans=index,tis_old,tis_new,util_old,util_new


bool BHV_SearchGrid::updateSearchGrid(double x1, double y1, 
				      double x2, double y2) 
{
  bool os_in_grid = false;

  int gsize = search_grid.size();
  for(int i=0; i<gsize; i++) {
    XYSquare square = search_grid.getElement(i);
    os_in_grid = os_in_grid || square.containsPoint(x2,y2);
    double length = square.segIntersectLength(x1,y1,x2,y2);
    if(length > 0) {
      double time_in_square = 0;
      if(osSPD > 0)
	time_in_square = length / osSPD;
      string glabel  = search_grid.getLabel();
      string gvar    = "GRID_DELTA_LOCAL";
      double cur_tis = search_grid.getVal(i);
      double new_tis = cur_tis + time_in_square;
      double cur_util = search_grid.getUtil(i);
      double new_util = timutif.evalUtility(new_tis);

      search_grid.setVal(i, new_tis);
      search_grid.setUtil(i, new_util);
      string gdelta = glabel + "@" + 
	intToString(i) + "," +
	dstringCompact(doubleToString(cur_tis))  + "," +
	dstringCompact(doubleToString(new_tis))  + "," +
	dstringCompact(doubleToString(cur_util)) + "," +
	dstringCompact(doubleToString(new_util));
      postMessage(gvar, gdelta);
    }
  }

  search_grid.resetFromMin();
  return(os_in_grid);
}








