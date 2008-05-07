/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_StandOn17.cpp                                    */
/*    DATE:                                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h>
#include "AngleUtils.h"
#include "BHV_StandOn17.h"
#include "AOF_R16.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Boilerplate for Dynamic Behavior Loading

IvPBehavior *createBehavior(IvPDomain domain) {
  return(new BHV_StandOn17(domain));
}

//-----------------------------------------------------------
// Procedure: Constructor

BHV_StandOn17::BHV_StandOn17(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_giveway17");
  this->setParam("build_info", "uniform_piece=course:2,speed:3");
  this->setParam("build_info", "uniform_grid=course:8,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  range = -1;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_StandOn17::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);

  if((g_param == "them") || (g_param == "contact")) {
    them_name = toupper(g_val);
    addInfoVars(them_name+"_NAV_X");
    addInfoVars(them_name+"_NAV_Y");
    addInfoVars(them_name+"_NAV_SPEED");
    addInfoVars(them_name+"_NAV_HEADING");
    return(true);
  }  
  else if(g_param == "range") {
    range = atof(g_val.c_str());
    return(true);
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_StandOn17::onRunState() 
{
  if(them_name == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok1, ok2;

  double cnCRS = getBufferDoubleVal(them_name+"_NAV_HEADING", ok1);
  double cnSPD = getBufferDoubleVal(them_name+"_NAV_SPEED", ok2);
  if(!ok1 || !ok2)
    return(0);

  double osCRS = getBufferDoubleVal("NAV_HEADING", ok1);
  double osSPD = getBufferDoubleVal("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(0);
  }

  if(cnCRS < 0) cnCRS += 360.0;

  double cnLAT = getBufferDoubleVal(them_name+"_NAV_Y", ok1);
  double cnLON = getBufferDoubleVal(them_name+"_NAV_X", ok2);
  if(!ok1 || !ok2)
    return(0);

  double osLAT = getBufferDoubleVal("NAV_Y", ok1);
  double osLON = getBufferDoubleVal("NAV_X", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(0);
  }

  double relevance = getRelevance(osLON, osLAT, osCRS, osSPD, 
				  cnLON, cnLAT, cnCRS, cnSPD);

  cout << "BHV_StandOn17::produceOF(): relevance: " << relevance << endl;
  if(relevance == 0)
    return(0);

  AOF_R16 aof(m_domain);
  aof.setParam("os_lat", osLAT);
  aof.setParam("os_lon", osLON);
  aof.setParam("cn_lat", cnLAT);
  aof.setParam("cn_lon", cnLON);
  aof.setParam("cn_crs", cnCRS);
  aof.setParam("cn_spd", cnSPD);
  aof.initialize();

  OF_Reflector reflector(&aof, 1);
  reflector.create(m_build_info);
  IvPFunction *ipf = reflector.extractOF();

  ipf->setPWT(relevance * m_priority_wt);

  return(ipf);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_StandOn17::getRelevance(double oslon, double oslat,
				   double oscrs, double osspd,
				   double cnlon, double cnlat, 
				   double cncrs, double cnspd)
{
  if((oslon==cnlon)&&(oslat==cnlat))
    return(0);

  if(range > -1) { // -1 means range wasn't specified
    double dist = hypot((oslon-cnlon), (oslat-cnlat));
    if(dist > range)
      return(0);
  }

  double ang_to_cn = relAng(oslon, oslat, cnlon, cnlat);
  double ang_to_os = ang_to_cn + 180.0;
  if(ang_to_os >= 360.0)
    ang_to_os -= 360.0;

  cout << "ang_to_cn: " << ang_to_cn << endl;
  cout << "oscrs:     " << oscrs << endl;
  cout << "ang_to_os: " << ang_to_os << endl;
  cout << "cncrs:     " << cncrs << endl;


  // Determine the "bearing delta" - the difference between the course
  // of a vehicle and the bearing of that vehicle to the other vehicle.
  //
  //                                x     Bearing to "x" is 35 degrees
  //                              /
  //                             /
  //                            /         "Bearing delta" is 55 degrees
  //                           /
  //                          /
  //                         O------->    Current course is 90 degrees

#if 0
  double os_bearing_delta = oscrs - ang_to_cn;
  if(os_bearing_delta < 0)
    os_bearing_delta += 360.0;
  if(os_bearing_delta > 180)
    os_bearing_delta = 360.0 - os_bearing_delta;
  if(os_bearing_delta > 15.0)
    return(0.0);
#endif

#if 1
  double cn_bearing_delta = cncrs - ang_to_os;
  if(cn_bearing_delta < 0)
    cn_bearing_delta += 360.0;
  if(cn_bearing_delta > 180)
    cn_bearing_delta = 360.0 - cn_bearing_delta;
  if(cn_bearing_delta > 15.0)
    return(0.0);
#endif

  return(1.0);
}






