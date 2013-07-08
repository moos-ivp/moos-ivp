/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include <cmath> 
#include "AOF_R16.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_R16::AOF_R16(IvPDomain gdomain) : AOF(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_osx_set = false;
  m_osy_set = false;
  m_cnx_set = false;
  m_cny_set = false;
  m_cnh_set = false;
  m_cnv_set = false;
  m_tol_set = false;

  m_cpa_engine = 0;
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R16::setParam(const string& param, double param_val)
{
  if(param == "osx") {
    m_osx = param_val;
    m_osx_set = true;
    return(true);
  }
  else if(param == "osy") {
    m_osy = param_val;
    m_osy_set = true;
    return(true);
  }
  else if(param == "cnx") {
    m_cnx = param_val;
    m_cnx_set = true;
    return(true);
  }
  else if(param == "cny") {
    m_cny = param_val;
    m_cny_set = true;
    return(true);
  }
  else if(param == "cnh") {
    m_cnh = param_val;
    m_cnh_set = true;
    return(true);
  }
  else if(param == "cnv") {
    m_cnv = param_val;
    m_cnv_set = true;
    return(true);
  }
  else if(param == "tol") {
    m_tol = param_val;
    m_tol_set = true;
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_R16::initialize()
{
  if((m_crs_ix==-1)|| (m_spd_ix==-1))
    return(false);

  if(!m_osx_set || !m_osy_set || !m_cnx_set || !m_tol_set)
    return(false);

  if(!m_cny_set || !m_cnh_set || !m_cnv_set)
    return(false);

  m_cpa_engine = new CPAEngine(m_cny, m_cnx, m_cnh, 
			       m_cnv, m_osy, m_osx);

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
  double eval_crs, eval_spd, cpa_dist;

  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix,0), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix,0), eval_spd);

  cpa_dist = m_cpa_engine->evalCPA(eval_crs, eval_spd, m_tol);

  bool crosses_bow  = m_cpa_engine->crossesBow(eval_crs, eval_spd);

  //  if(crosses_bow) 
  //    cpa_distance = cpa_distance * 0.1;

#if 1
  if(crosses_bow) 
    cpa_dist = cpa_dist * 0.045;
#endif

  double value = metric(cpa_dist);
  

  //return(cpa_dist);
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



