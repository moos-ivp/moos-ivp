/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_CutRangeFCPA.cpp                                 */
/*    DATE: Nov 4th 2006                                         */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h>
#include "AOF_CutRangeFCPA.h"
#include "BHV_CutRangeFCPA.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_CutRangeFCPA::BHV_CutRangeFCPA(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_cutrange_fcpa");
  this->setParam("build_info", "uniform_piece=course:3,speed:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  range_max = 0;
  range_min = 50;
  tol       = 60;

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_CutRangeFCPA::setParam(string g_param, string g_val) 
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
  else if(g_param == "tol") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    tol = dval;
    return(true);
  }  
  else if(g_param == "max_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    range_max = dval;
    return(true);
  }  
  else if(g_param == "min_range") {
    double dval = atof(g_val.c_str());
    if((dval < 0) || (!isNumber(g_val)))
      return(false);
    range_min = dval;
    return(true);
  }  
  return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_CutRangeFCPA::onRunState() 
{
  if(them_name == "") {
    postEMessage("contact ID not set.");
    return(0);
  }

  bool ok1, ok2;

  double cnCRS = getBufferDoubleVal(them_name+"_NAV_HEADING", ok1);
  double cnSPD = getBufferDoubleVal(them_name+"_NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postWMessage("contact course/speed info not found.");
    return(0);
  }

  double osCRS = getBufferDoubleVal("NAV_HEADING", ok1);
  double osSPD = getBufferDoubleVal("NAV_SPEED", ok2);
  if(!ok1 || !ok2) {
    postEMessage("ownship course/speed info not found.");
    return(0);
  }

  if(cnCRS < 0) cnCRS += 360.0;

  double cnX = getBufferDoubleVal(them_name+"_NAV_X", ok2);
  double cnY = getBufferDoubleVal(them_name+"_NAV_Y", ok1);
  if(!ok1 || !ok2) {
    postWMessage("contact x/y info not found.");
    return(0);
  }

  double osX = getBufferDoubleVal("NAV_X", ok2);
  double osY = getBufferDoubleVal("NAV_Y", ok1);
  if(!ok1 || !ok2) {
    postEMessage("ownship x/y info not found.");
    return(0);
  }


  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance(osX, osY, cnX, cnY);

  if(relevance <= 0)
    return(0);

  AOF_CutRangeFCPA aof(m_domain);
  aof.setParam("cnlat", cnY);
  aof.setParam("cnlon", cnX);
  aof.setParam("cncrs", cnCRS);
  aof.setParam("cnspd", cnSPD);
  aof.setParam("oslat", osY);
  aof.setParam("oslon", osX);
  aof.setParam("tol", tol);
  aof.initialize();

  OF_Reflector reflector(&aof, 1);
  reflector.create(m_build_info);
  IvPFunction *of = reflector.extractOF();

#if 0
  cout << "CutRange Pre-Normalize MIN-WT: " << of->getPDMap()->getMinWT() << endl;
  cout << "CutRange Pre-Normalize MAX-WT: " << of->getPDMap()->getMaxWT() << endl;
#endif

  of->getPDMap()->normalize(0.0, 100.0);

#if 0
  cout << "CutRange MIN-WT: " << of->getPDMap()->getMinWT() << endl;
  cout << "CutRange MAX-WT: " << of->getPDMap()->getMaxWT() << endl;
#endif

  of->setPWT(relevance * m_priority_wt);

  return(of);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_CutRangeFCPA::getRelevance(double osX, double osY,
				  double cnX, double cnY)
{
  double total_range = range_max - range_min;
  if(total_range <= 0)  // This would likely be an error
    return(0);


  double dist = hypot((osX - cnX), (osY - cnY));
#if 0
  cout << "BHV_CutRangeFCPA: Current Distance ------" << dist << endl;
#endif  

  if(dist > range_max)
    return(0.0);
  if(dist < range_min)
    return(0.0);

  double pct = (dist - range_min) / total_range;
  return(pct);
}







