/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_WaypointRateClosure.cpp                          */
/*    DATE: Sep 28th 2006                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "AOF_WaypointRateClosure.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_WaypointRateClosure::AOF_WaypointRateClosure(IvPDomain g_domain) 
  : AOF(g_domain)
{
  crs_ix = g_domain.getIndex("course");
  spd_ix = g_domain.getIndex("speed");
  
  os_y_set      = false;
  os_x_set      = false;
  pt_y_set      = false;
  pt_x_set      = false;
  desired_spd_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_WaypointRateClosure::setParam(const string& param, double param_val)
{
  if(param == "oslat") {
    os_y = param_val;
    os_y_set = true;
    return(true);
  }
  else if(param == "oslon") {
    os_x = param_val;
    os_x_set = true;
    return(true);
  }
  else if(param == "ptlat") {
    pt_y = param_val;
    pt_y_set = true;
    return(true);
  }
  else if(param == "ptlon") {
    pt_x = param_val;
    pt_x_set = true;
    return(true);
  }
  else if(param == "desired_speed") {
    desired_spd = param_val;
    desired_spd_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_WaypointRateClosure::initialize()
{
  if((crs_ix==-1)||(spd_ix==-1))
    return(false);

  if(!os_y_set || !os_x_set || !pt_y_set || !pt_x_set)
    return(false);

  if(!desired_spd_set)
    return(false);

  angle_to_wpt = relAng(os_x, os_y, pt_x, pt_y);;

  return(true);
}


//----------------------------------------------------------------
// Procedure: evalBox
//                                                                
//                (pt_x,pt_y)  o                          
//                            / \                        
//                           /   \  "dist2"                 
//                          /     \                            
//                         /       \                         
//                        /         \                 
//          "direct_dist"/           \                
//                      /             o (new_x, new_y) 
//                     /            /              
//                    /          /                       
//                   /        /                           
//                  /      /                          
//                 /    /    "dist1"                 
//                /  /                                
//  (os_x,os_y)  o                             
//                                                
//                                                                
//                                                                

double AOF_WaypointRateClosure::evalBox(const IvPBox *b) const
{
  double eval_crs = 0;
  double eval_spd = 0;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  
  double angle_diff      = angle360(eval_crs - angle_to_wpt);
  double rad_diff        = degToRadians(angle_diff);
  double rate_of_closure = cos(rad_diff) * eval_spd;

  double rate_of_detour  = sin(rad_diff) * eval_spd;
  if(rate_of_detour < 0)
    rate_of_detour *= -1;

  double diff = rate_of_closure - desired_spd;
  if(diff < 0) 
    diff *= -1;
  
  double mod_val1 = (100 - (diff*100));
#if 0
  if(mod_val1 < 0)
    mod_val1 = 0;
#endif

  double mod_val2 = 50 * rate_of_detour;

  double mod_val = mod_val1 - mod_val2;

  //  if(mod_val < 0)
  // mod_val = 0;
  
  
  return(mod_val);
}

