/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: BHV_SearchArtifact.cpp                               */
/*    DATE: NOV 28, 2007                                         */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
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
  this->setParam("descriptor", "(d)bhv_search_grid");
  this->setParam("unifbox", "course=6,  speed=4");
  this->setParam("gridbox", "course=12, speed=8");

  domain = subDomain(domain, "course,speed");

  has_fired = false;
  osX   = 0;
  osY   = 0;
  osSPD = 0;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_SearchArtifact::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "searchgrid") {
    XYGrid new_search_grid;
    bool ok = new_search_grid.initialize(val);
    if(!ok) 
      return(false);
    if(!param_lock)
      search_grid = new_search_grid;
    return(true);
  }
  
  if(param == "timutif") {
    Timutif new_timutif(val);
    bool ok = new_timutif.isConfigured();
    if(!ok) 
      return(false);
    if(!param_lock)
      timutif = new_timutif;
    return(true);
  }
  
  if(param == "time_horizon") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    if(!param_lock)
      time_horizon = dval;
    return(true);
  }
  
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF
//

IvPFunction *BHV_SearchArtifact::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
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

  AOF_SearchArtifact aof(domain, &search_grid);

  aof.setParam("os_lat", osY);
  aof.setParam("os_lon", osX);
  aof.setParam("time_horizon", time_horizon);
  aof.setParam("timutif", timutif.toString());
  aof.initialize();
  aof.fillCache();

  OF_Reflector reflector(&aof, 1);
  reflector.createUniform(unif_box, grid_box);
  ipf = reflector.extractOF();

  if(ipf)
    ipf->setPWT(priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: updateFromInfoBuffer
//

bool BHV_SearchArtifact::updateFromInfoBuffer()
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
  osX   = info_buffer->dQuery("NAV_X",     ok1);
  osY   = info_buffer->dQuery("NAV_Y",     ok2);
  osSPD = info_buffer->dQuery("NAV_SPEED", ok3);
  osCRS = info_buffer->dQuery("NAV_HEADING", ok4);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2)
    postEMessage("No ownship X/Y info in info_buffer.");
  if(!ok3)
    postWMessage("No ownship speed info in info_buffer.");
  if(!ok4)
    postWMessage("No ownship heading info in info_buffer.");
  if( !(ok1 && ok2 && ok3 && ok4) )
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: updateSearchGrid
//
//  GRID_MSG @ type=config @ gname=area23 @ vname=nyak200 @
//      trans=index,tis_old,tis_new,util_old,util_new


bool BHV_SearchArtifact::updateSearchGrid(double x1, double y1, 
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
      string gvar1 = "GRID_DELTA";
      string gvar2 = "GRID_DELTA_" + glabel;
      string gvar3 = "GRID_MSG";
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
      string msg = "type=delta @ gname=" + glabel;
      msg += " @ vname=" + us_name;
      msg += " @ trans=" + intToString(i) + ",";
	dstringCompact(doubleToString(cur_tis))  + "," +
	dstringCompact(doubleToString(new_tis))  + "," +
	dstringCompact(doubleToString(cur_util)) + "," +
	dstringCompact(doubleToString(new_util));
      postMessage(gvar1, gdelta);
      postMessage(gvar2, gdelta);
      postMessage(gvar3, msg);
    }
  }

  search_grid.resetFromMin();
  return(os_in_grid);
}








