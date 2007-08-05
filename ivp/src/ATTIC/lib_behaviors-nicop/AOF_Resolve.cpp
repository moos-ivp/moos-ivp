/***********************************************************/
/*    NAME:                                                */
/*    FILE: AOF_Resolve.cpp                                */
/*    DATE: Sept 7th, 2005                                 */
/***********************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include <assert.h>
#include "AOF_Resolve.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_Resolve::AOF_Resolve(IvPDomain g_domain, CResolver* g_resolver)
  : AOF(g_domain)
{
  crs_ix = m_domain.getIndex("course");
  spd_ix = m_domain.getIndex("speed");
  tol_ix = m_domain.getIndex("tol");

  os_lat_set = false;
  os_lon_set = false;

  resolver = g_resolver;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_Resolve::setParam(const string& param, double param_val)
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
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_Resolve::initialize()
{
  if((crs_ix==-1) || (spd_ix==-1) || (tol_ix==-1))
    return(false);
  
  if(!os_lat_set || !os_lon_set)
    return(false);
  
  return(true);
}




//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_Resolve::evalBox(const IvPBox *b) const
{ 
  double eval_crs, eval_spd, eval_tol;

  m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs);
  m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd);
  m_domain.getVal(tol_ix, b->pt(tol_ix,0), eval_tol);

  double gamOS  = degToRadians(eval_crs);  // Angle in radians.
  double cgamOS = cos(gamOS);              // Cosine of Angle (osCRS).
  double sgamOS = sin(gamOS);              // Sine   of Angle (osCRS).

  double evalY = (cgamOS * eval_spd * eval_tol) + os_lat;
  double evalX = (sgamOS * eval_spd * eval_tol) + os_lon;

  //double ret_val = distToPoint(osX, osY, evalX, evalY);

  
#if 0
  cout << "c:" << evalCRS << " ";
  cout << "s:" << evalSPD << " ";
  cout << "t:" << evalTOL << " ";

  cout << "x:"  << os_lon << " ";
  cout << "x':" << evalX  << " ";
  cout << "y:"  << os_lat << " ";
  cout << "y':" << evalY  << endl;
#endif

  double ret_val = resolver->EvaluateAt(evalX, evalY);

  return(ret_val);
}

