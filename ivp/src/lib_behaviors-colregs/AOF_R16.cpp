/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_R16.cpp                                          */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <math.h> 
#include "AOF_R16.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: gcnlat  Given Contact Latitude Position
//      args: gcnlon  Given Contact Longitude Position
//      args: gcncrs  Given Contact Course
//      args: gcnspd  Given Contact Speed
//      args: goslat  Given Ownship Latitude Position
//      args: goslon  Given Ownship Latitude Position

AOF_R16::AOF_R16(IvPDomain gdomain) : AOF(gdomain)
{
  crs_ix = gdomain.getIndex("course");
  spd_ix = gdomain.getIndex("speed");
  tol_ix = gdomain.getIndex("tol");

  os_lat_set = false;
  os_lon_set = false;
  cn_lat_set = false;
  cn_lon_set = false;
  cn_crs_set = false;
  cn_spd_set = false;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R16::setParam(const string& param, double param_val)
{
  if(param == "os_lat") {
    os_lat = param_val;
    os_lat_set = true;
    return(true);
  }
  else if(param == "os_lon") {
    os_lon = param_val;
    os_lon_set = true;
    return(true);
  }
  else if(param == "cn_lat") {
    cn_lat = param_val;
    cn_lat_set = true;
    return(true);
  }
  else if(param == "cn_lon") {
    cn_lon = param_val;
    cn_lon_set = true;
    return(true);
  }
  else if(param == "cn_crs") {
    cn_crs = param_val;
    cn_crs_set = true;
    return(true);
  }
  else if(param == "cn_spd") {
    cn_spd = param_val;
    cn_spd_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_R16::initialize()
{
  if((crs_ix==-1)|| (spd_ix==-1)|| (tol_ix==-1))
    return(false);

  if(!os_lat_set || !os_lon_set || !cn_lat_set)
    return(false);

  if(!cn_lon_set || !cn_crs_set || !cn_spd_set)
    return(false);

  cpa_engine = new CPAEngine(cn_lat, cn_lon, cn_crs, 
			     cn_spd, os_lat, os_lon);

  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_R16::evalBox(const IvPBox *b) const
{
  double eval_crs, eval_spd, eval_tol, cpa_dist;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  m_domain.getVal(tol_ix, b->pt(tol_ix,0), eval_tol);

  cpa_dist = cpa_engine->evalCPA(eval_crs, eval_spd, eval_tol);

  bool crosses_bow  = cpa_engine->crossesBow(eval_crs, eval_spd);

  //  if(crosses_bow) 
  //    cpa_distance = cpa_distance * 0.1;

  if(crosses_bow) 
    cpa_dist = cpa_dist * 0.45;


  double value = metric(cpa_dist);
  
#if 0
  if((eval_crs >= 45) && (eval_crs <= 45)) {
    cout << "   osCRS: " << eval_crs; 
    cout << "   osSPD: " << eval_spd;
    cout << "   osTOL: " << eval_tol;
    cout << "   cpa: " << cpa_dist;
    cout << "   val: " << value << endl;
  }
#endif


  return(value);
}

//----------------------------------------------------------------
// Procedure: metric
//      Note: We convert a CPA distance to a utility value. The 
//            range is between 0 and 100, but it is not important
//            since the IvP problems solver normalizes the range
//            output from each behavior before applying priority
//            weights.
//
//             ^                                  100                      
//             |                             ---------------------
//             |                           /                      
//             |                        /    |                    
//             |                     /                            
//             |                  /          |                    
//             |               /                                  
//  drop_val   |            /                |                    
//          ---|         /                                        
//             |       |                     |                   
//             |       |                                          
//             |   0   |                     |                  
//             |_______|__________________________________________
//                     |                     |                             
//                    min_dist              max_dist                       
//                                                                

double AOF_R16::metric(double gval) const
{
  double min_dist = 15.0;
  double max_dist = 100.0;
  double drop_val = 25.0;

  if(gval < min_dist) 
    return(0);
  if(gval > max_dist) 
    return(100);

  double val = drop_val;
  val += (100.0 - drop_val) * (gval - min_dist) / (max_dist - min_dist);
  return(val);
}








